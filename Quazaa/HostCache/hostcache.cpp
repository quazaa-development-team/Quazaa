/*
** hostcache.cpp
**
** Copyright © Quazaa Development Team, 2009-2013.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <time.h>

#include "network.h"
#include "geoiplist.h"
#include "quazaasettings.h"
#include "Misc/timedsignalqueue.h"
#include "Security/securitymanager.h"

#if QT_VERSION >= 0x050000 
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include <QDir>

#include "hostcache.h"

#include "debug_new.h"

CHostCache hostCache;

CHostCache::CHostCache():
    m_tLastSave( common::getTNowUTC() ),
    m_nSize( 0 )
{
}

CHostCache::~CHostCache()
{
	THCLLMap::iterator it = m_llHosts.begin();

	while ( it != m_llHosts.end() )
	{
		THostCacheLList list = (*it).second;
		while( !list.isEmpty() )
		{
			delete list.takeLast();
		}

		++it;
	}
}

/**
 * @brief start initializes the Host Cache. Make sure this is called after QApplication is
 * instantiated.
 * Locking: YES (asynchronous)
 */
void CHostCache::start()
{
	m_pHostCacheDiscoveryThread = TSharedThreadPtr( new QThread() );

	moveToThread( m_pHostCacheDiscoveryThread.data() );
	m_pHostCacheDiscoveryThread.data()->start( QThread::LowPriority );

	QMetaObject::invokeMethod( this, "asyncStartUpHelper", Qt::QueuedConnection );
}

void CHostCache::add(const CEndPoint host, const quint32 tTimeStamp)
{
	QMetaObject::invokeMethod( this, "addSync", Qt::QueuedConnection, Q_ARG(CEndPoint, host),
	                           Q_ARG(quint32, tTimeStamp), Q_ARG(bool, true) );
}

CHostCacheHost* CHostCache::update(const CEndPoint& oHost, const quint32 tTimeStamp)
{
	quint8 nFailures;
	THostCacheLLIterator it = find( oHost, nFailures );

	if ( it == m_llHosts[nFailures].end() )
		return NULL;

	return update( it, tTimeStamp );
}

CHostCacheHost* CHostCache::update(CHostCacheHost* pHost, const quint32 tTimeStamp)
{
	const quint8 nFailures = pHost->failures();
	THostCacheLLIterator it = find( pHost );

	if ( it == m_llHosts[nFailures].end() )
		return NULL;

	return update( it, tTimeStamp );
}

CHostCacheHost* CHostCache::update(THostCacheLLIterator itHost, const quint32 tTimeStamp)
{
	CHostCacheHost* pHost = *itHost;

	// TODO: Tix this to maintain sorted list.

	//pHost->m_tTimestamp = tTimeStamp;
	return pHost;
}

void CHostCache::remove(const CEndPoint& oHost)
{
	quint8 nFailures;
	THostCacheLLIterator it = find( oHost, nFailures );

	if ( it != m_llHosts[nFailures].end() )
	{
		delete *it;
		remove( it, nFailures );
	}
}

void CHostCache::remove(CHostCacheHost* pRemove)
{
	quint8 nFailures = pRemove->failures();
	THostCacheLLIterator it = find( pRemove );

	if ( it != m_llHosts[nFailures].end() )
	{
		remove( it, nFailures );
	}

	delete pRemove;
}

void CHostCache::remove(THostCacheLLIterator itHost, const quint8 nFailures)
{
	m_llHosts[nFailures].erase( itHost );
	--m_nSize;
}

void CHostCache::removeWorst(const quint8 nFailures)
{
	delete m_llHosts[nFailures].takeLast();
	--m_nSize;
}

void CHostCache::addXTry(QString& sHeader)
{
	// X-Try-Hubs: 86.141.203.14:6346 2010-02-23T16:17Z,91.78.12.117:1164 2010-02-23T16:17Z,89.74.83
	// .103:7972 2010-02-23T16:17Z,93.89.196.113:5649 2010-02-23T16:17Z,24.193.237.252:6346 2010-02-
	// 23T16:17Z,24.226.149.80:6346 2010-02-23T16:17Z,89.142.217.180:9633 2010-02-23T16:17Z,83.219.1
	// 12.111:6346 2010-02-23T16:17Z,201.17.187.205:6346 2010-02-23T16:17Z,213.29.19.41:6346 2010-02
	// -23T16:17Z,78.231.224.180:6346 2010-02-23T16:17Z,213.143.88.92:6346 2010-02-23T16:17Z,77.209.
	// 25.104:1515 2010-02-23T16:17Z,86.220.168.24:59153 2010-02-23T16:17Z,88.183.80.110:6346 2010-0
	// 2-23T16:17Z
	QStringList l = sHeader.split( "," );

	if ( l.isEmpty() )
	{
		return;
	}

	for ( qint32 i = 0; i < l.size(); ++i )
	{
		QStringList le = l.at( i ).split( " " );
		if ( le.size() != 2 )
		{
			continue;
		}

		CEndPoint addr( le.at( 0 ) );
		if ( !addr.isValid() )
		{
			continue;
		}

		QDateTime oTs = QDateTime::fromString( le.at( 1 ), "yyyy-MM-ddThh:mmZ" );
		oTs.setTimeSpec( Qt::UTC );
		if ( !oTs.isValid() )
			oTs = common::getDateTimeUTC();
		addSync( addr, oTs, false );
	}
}

QString CHostCache::getXTry() const
{
	const quint8  nFailures = 0;
	const quint32 nMax      = 10;

	QString sRet;
	quint32 nCount = 0;

	if ( !m_llHosts.count( nFailures ) || !m_llHosts.at( nFailures ).size() )
	{
		return QString();
	}

	foreach ( CHostCacheHost* pHost, m_llHosts.at( nFailures ) )
	{
		QDateTime tTimeStamp;
		tTimeStamp.setTimeSpec( Qt::UTC );
		tTimeStamp.setTime_t( pHost->timestamp() );

		sRet.append( pHost->address().toStringWithPort() + " " );
		sRet.append( tTimeStamp.toString( "yyyy-MM-ddThh:mmZ" ) );
		sRet.append( "," );

		++nCount;

		if ( nCount == nMax )
			break;
	}

	if ( sRet.isEmpty() )
	{
		return QString();
	}

	return "X-Try-Hubs: " + sRet.left( sRet.size() - 1 );
}

void CHostCache::onFailure(CEndPoint addr)
{
	quint8 nFailures;
	THostCacheLLIterator itHost = find( addr, nFailures );

	if ( itHost != m_llHosts[nFailures].end() )
	{
		if ( ++nFailures > quazaaSettings.Connection.FailureLimit )
		{
			remove( itHost, nFailures );
		}
		else
		{
			(*itHost)->setFailures( nFailures );
		}
	}
}

CHostCacheHost* CHostCache::getConnectable(QSet<CHostCacheHost*> oExcept, QString sCountry)
{
	bool bCountry = ( sCountry != "ZZ" );
	static bool bMaintained = false;

	if ( !m_nSize )
	{
		return NULL;
	}

	// First try untested or working hosts, then fall back to failed hosts to increase chances for
	// successful connection
	for ( quint8 nFailures = 0; nFailures < quazaaSettings.Connection.FailureLimit; ++nFailures )
	{
		foreach ( CHostCacheHost * pHost, m_llHosts[nFailures] )
		{
			if ( bCountry && pHost->address().country() != sCountry )
			{
				continue;
			}

			if ( pHost->connectable() )
			{
				if ( !oExcept.contains( pHost ) )
					return pHost;
			}
		}
	}

	CHostCacheHost* pReturn = NULL;

	if ( bMaintained )
	{
		bMaintained = false;
	}
	else
	{
		maintain();
		bMaintained = true;
		pReturn = getConnectable( oExcept, sCountry );
	}

	return pReturn;
}

bool CHostCache::save(const quint32 tNow) const
{
	ASSUME_LOCK( hostCache.m_pSection );

	bool bReturn;

	quint32 nCount = common::securredSaveFile( common::globalDataFiles, "hostcache.dat", m_sMessage,
	                                           this, &CHostCache::writeToFile );
	if ( nCount )
	{
		m_tLastSave = tNow;
		bReturn = true;

		systemLog.postLog( LogSeverity::Debug,
		                   m_sMessage + QObject::tr( "Saved %1 hosts." ).arg( nCount ) );
	}
	else
	{
		bReturn = false;
	}

	return bReturn;
}

void CHostCache::pruneOldHosts(const quint32 tNow)
{
	/*QMutableListIterator<CHostCacheHost*> it( m_lHosts );
	it.toBack();

	while ( it.hasPrevious() )
	{
		it.previous();
		if ( (qint64)( tNow - it.value()->m_tTimestamp ) > quazaaSettings.Gnutella2.HostExpire )
		{
			delete it.value();
			it.remove();
			--m_nSize;
		}
		else
		{
			break;
		}
	}*/
}

void CHostCache::pruneByQueryAck(const quint32 tNow)
{
	/*for ( THostCacheIterator it = m_lHosts.begin(); it != m_lHosts.end(); )
	{
		if ( (*it)->m_tAck && tNow - (*it)->m_tAck > quazaaSettings.Gnutella2.QueryHostDeadline )
		{
			delete *it;
			it = m_lHosts.erase( it );
			--m_nSize;
		}
		else
		{
			++it;
		}
	}*/
}

/**
  * Helper method for save()
  * Requires Locking: R
  */
quint32 CHostCache::writeToFile(const void * const pManager, QFile& oFile)
{
	QDataStream oStream( &oFile );
	CHostCache* pHostCache = (CHostCache*)pManager;

	const quint16 nVersion = HOST_CACHE_CODE_VERSION;
	const quint32 nCount   = (quint32)pHostCache->m_nSize;

	oStream << nVersion;
	oStream << nCount;

	if ( nCount )
	{
		THCLLMap::iterator itFailures = pHostCache->m_llHosts.begin();

		while ( itFailures != pHostCache->m_llHosts.end() )
		{
			THostCacheLList list = (*itFailures).second;

			foreach ( CHostCacheHost* pHost, list )
			{
				oStream << pHost->address();
				oStream << pHost->failures();

				oStream << pHost->timestamp();
				oStream << pHost->lastConnect();
			}

			++itFailures;
		}
	}

	return nCount;
}

CHostCacheHost* CHostCache::addSync(CEndPoint host, const QDateTime& ts, bool bLock)
{
	Q_ASSERT( ts.timeSpec() == Qt::UTC );
	return addSync( host, ts.toTime_t(), bLock );
}

CHostCacheHost* CHostCache::addSync(CEndPoint host, quint32 tTimeStamp, bool bLock)
{
	if ( bLock )
		m_pSection.lock();

	ASSUME_LOCK( hostCache.m_pSection );

	if ( !host.isValid() )
	{
		if ( bLock )
			m_pSection.unlock();
		return NULL;
	}

	if ( host.isFirewalled() )
	{
		if ( bLock )
			m_pSection.unlock();
		return NULL;
	}

	// At this point the security check should already have been performed.
	// Q_ASSERT( !securityManager.isDenied(host) );
	if ( securityManager.isDenied(host) )
	{
		if ( bLock )
			m_pSection.unlock();
		return NULL;
	}

	const quint32 tNow = common::getTNowUTC();

	if ( tTimeStamp > tNow )
	{
		tTimeStamp = tNow - 60 ;
	}

	quint8 nFailures;
	THostCacheLLIterator itPrev = find( host, nFailures );

	if ( itPrev != m_llHosts[nFailures].end() )
	{
		CHostCacheHost* pUpdate = update( itPrev, tTimeStamp );
		if ( bLock )
			m_pSection.unlock();
		return pUpdate;
	}

	Q_ASSERT( !nFailures ); // TODO: remove once tested.

	// create host, find place in sorted list, insert it there
	CHostCacheHost* pNew = new CHostCacheHost( host, tTimeStamp );
	THostCacheLLIterator it = m_llHosts[nFailures].begin();

	// advance until 1 element past the last element with higher timestamp
	while ( it != m_llHosts[nFailures].end() && (*it)->timestamp() > tTimeStamp )
		++it;


	if ( it == m_llHosts[nFailures].begin() ) // it hasn't been advanced
	{
		m_llHosts[nFailures].push_front( pNew );
	}
	else
	{
		// go 1 element back (to last element with higher timestamp) and insert after this element
		m_llHosts[nFailures].insert( --it, pNew );
	}

	++m_nSize;

	if ( bLock )
		m_pSection.unlock();
	return pNew;
}

void CHostCache::maintain()
{
	m_pSection.lock();

	const quint32 tNow = common::getTNowUTC();

	// First try untested or working hosts, then fall back to failed hosts to increase chances for
	// successful connection
	THCLLMap::iterator it = m_llHosts.begin();

	while ( it != m_llHosts.end() )
	{
		THostCacheLList list = (*it).second;
		foreach ( CHostCacheHost * pHost, list )
		{
			// Note: if ( !pHost->m_tLastConnect ), the following statement also evaluates to true.
			if ( !pHost->connectable() )
				pHost->setConnectable( tNow - pHost->lastConnect() >
				                       ( quazaaSettings.Gnutella.ConnectThrottle + pHost->failures()
				                         * quazaaSettings.Connection.FailurePenalty ) );

		}

		++it;
	}

	// Remove all hosts in lists with failure limit higher than settings
	const quint32 nFailureLimit = quazaaSettings.Connection.FailureLimit;
	if ( m_llHosts.size() > nFailureLimit )
	{
		for ( quint8 nFailure = 255; nFailure > nFailureLimit; --nFailure )
		{
			if ( m_llHosts.count( nFailure ) )
			{
				while ( m_llHosts[nFailure].size() )
				{
					removeWorst( nFailure );
				}

				m_llHosts.erase( nFailure );
			}
		}
	}

	const quint32 nMaxSize = quazaaSettings.Gnutella.HostCacheSize;
	if ( nMaxSize && m_nSize > nMaxSize )
	{
		const quint32 nMax = nMaxSize - nMaxSize / 4;

		Q_ASSERT( nMax > 0 );

		quint8 nFailure = nFailureLimit;
		// remove 1/4 of all hosts if the cache gets too full - failed and oldest first
		while ( m_nSize > nMax && nFailure != 255 )
		{
			removeWorst( nFailure );
		}

		save( tNow );
	}
	else if ( tNow - m_tLastSave > 600 )
	{
		save( tNow );
	}

	m_pSection.unlock();
}

THostCacheLLIterator CHostCache::find(const CEndPoint& oHost, quint8& nFailures)
{
	THostCacheLList lHosts;
	THCLLMap::iterator itMap = m_llHosts.begin();
	while ( itMap != m_llHosts.end() )
	{
		lHosts = (*itMap).second;
		for ( THostCacheLLIterator it = lHosts.begin(); it != lHosts.end(); ++it )
		{
			if ( (*it)->address() == oHost )
			{
				nFailures = (*it)->failures();
				return it;
			}
		}
		++itMap;
	}

	nFailures = 0;
	return m_llHosts[0].end();
}

THostCacheLLIterator CHostCache::find(const CHostCacheHost* const pHost)
{
	return pHost->iterator();
}

void CHostCache::load()
{
	m_pSection.lock();

	QFile file( common::getLocation( common::globalDataFiles ) + "hostcache.dat" );

	if ( !file.exists() || !file.open( QIODevice::ReadOnly ) )
		return;

	QDataStream oStream( &file );

	quint16 nVersion;
	quint32 nCount;

	oStream >> nVersion;
	oStream >> nCount;

	const quint32 tNow   = common::getTNowUTC();

	if ( nVersion == HOST_CACHE_CODE_VERSION ) // else do load defaults
	{
		CEndPoint oAddress;
		quint8  nFailures    = 0;
		quint32 tTimeStamp   = 0;
		quint32 tLastConnect = 0;

		CHostCacheHost* pHost = NULL;

		while ( nCount )
		{
			oStream >> oAddress;
			oStream >> nFailures;

			oStream >> tTimeStamp;
			oStream >> tLastConnect;

			if ( tTimeStamp - tNow > 0 )
				tTimeStamp = tNow - 60;

			pHost = addSync( oAddress, tTimeStamp, false );
			if ( pHost )
			{
				if ( tLastConnect - tNow > 0 )
					tLastConnect = tNow - 60;

				pHost->setFailures( nFailures );
				pHost->setLastConnect( tLastConnect );
			}

			--nCount;
			pHost = NULL;
		}
	}

	file.close();

	pruneOldHosts( tNow );

	systemLog.postLog( LogSeverity::Debug,
	                   m_sMessage + QObject::tr( "Loaded %1 hosts." ).arg( m_nSize ) );

	m_pSection.unlock();
}

void CHostCache::asyncStartUpHelper()
{
	// reg. meta types
	qRegisterMetaType<CEndPoint>( "CEndPoint" );

	// Includes its own locking.
	load();

	maintain();

	signalQueue.push( this, SLOT( maintain() ), 10000, true );
}

