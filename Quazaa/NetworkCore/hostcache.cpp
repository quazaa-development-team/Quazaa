/*
** $Id$
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

#include "hostcache.h"
#include <QFile>
#include <QDataStream>
#include <QStringList>
#include "network.h"
#include <time.h>
#include "geoiplist.h"
#include "Security/securitymanager.h"

#if QT_VERSION >= 0x050000 
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include <QDir>
#include "quazaasettings.h"

#include "debug_new.h"

CHostCache hostCache;

template<>
class qLess <CHostCacheHost*>
{
public:
	inline bool operator()(const CHostCacheHost* l, const CHostCacheHost* r) const
	{
		return l->m_tTimestamp > r->m_tTimestamp;
	}
};

bool CHostCacheHost::canQuery(QDateTime tNow)
{
	if ( tNow.isNull() )
	{
		tNow = common::getDateTimeUTC();
	}

	if ( !m_tAck.isNull() && m_nQueryKey ) // if waiting for an ack, and we have a query key
	{
		return false;
	}

	if ( m_tTimestamp.secsTo( tNow ) > quazaaSettings.Gnutella2.HostCurrent ) // host is not too old
	{
		return false;
	}

	if ( !m_tRetryAfter.isNull() && tNow < m_tRetryAfter ) // honor retry-after
	{
		return false;
	}

	if ( m_tLastQuery.isNull() ) // host not already queried
	{
		return true;
	}

	return m_tLastQuery.secsTo(tNow) > (long)quazaaSettings.Gnutella2.QueryHostThrottle;
}

void CHostCacheHost::setKey(quint32 nKey, CEndPoint* pHost)
{
	m_tAck      = QDateTime();
	m_nFailures = 0;
	m_nQueryKey = nKey;
	m_nKeyTime  = common::getDateTimeUTC();
	m_nKeyHost  = pHost ? *pHost : Network.GetLocalAddress();
}

CHostCache::CHostCache():
	m_tLastSave( common::getDateTimeUTC() ),
	m_nMaxCacheHosts( 3000 )
{
}

CHostCache::~CHostCache()
{
	while( !m_lHosts.isEmpty() )
	{
		delete m_lHosts.takeFirst();
	}
}

CHostCacheHost* CHostCache::add(CEndPoint host, QDateTime ts)
{
	if ( !host.isValid() )
	{
		return NULL;
	}

	if ( host.isFirewalled() )
		return NULL;

	// At this point the security check should already have been performed.
	// Q_ASSERT( !securityManager.isDenied(host) );
	if ( securityManager.isDenied(host) )
		return NULL;

	if ( (quint32)m_lHosts.size() > m_nMaxCacheHosts )
	{
		int nMax = m_nMaxCacheHosts / 2;
		while ( m_lHosts.size() > nMax )
		{
			delete m_lHosts.takeLast();
		}

		save();
	}

	QDateTime tNow = common::getDateTimeUTC();

	if ( m_tLastSave.secsTo( tNow ) > 600 )
		save();

	if ( ts.isNull() || ts > tNow )
	{
		ts = tNow.addSecs( -60 );
	}

	CHostCacheIterator itPrev = find( host );

	if ( itPrev != m_lHosts.end() )
	{
		return update( itPrev );
	}

	Q_ASSERT( ts.timeSpec() == Qt::UTC );

	CHostCacheHost* pNew = new CHostCacheHost();
	pNew->m_oAddress   = host;
	pNew->m_tTimestamp = ts;

	CHostCacheIterator it = qLowerBound( m_lHosts.begin(), m_lHosts.end(),
										 pNew, qLess<CHostCacheHost*>() );
	m_lHosts.insert( it, pNew );

	return pNew;
}

CHostCacheIterator CHostCache::find(CEndPoint oHost)
{
	for ( CHostCacheIterator it = m_lHosts.begin(); it != m_lHosts.end(); ++it )
	{
		if ( (*it)->m_oAddress == oHost )
			return it;
	}

	return m_lHosts.end();
}

CHostCacheIterator CHostCache::find(CHostCacheHost *pHost)
{
	CHostCacheIterator it = qFind( m_lHosts.begin(), m_lHosts.end(), pHost );
	return it;
}

CHostCacheHost* CHostCache::update(CEndPoint oHost)
{
	CHostCacheIterator it = find( oHost );

	if ( it == m_lHosts.end() )
		return NULL;

	return update( it );
}

CHostCacheHost* CHostCache::update(CHostCacheHost* pHost)
{
	CHostCacheIterator it = find( pHost );

	if ( it == m_lHosts.end() )
		return NULL;

	return update( it );
}

CHostCacheHost* CHostCache::update(CHostCacheIterator itHost)
{
	CHostCacheHost* pHost = *itHost;
	m_lHosts.erase( itHost );
	pHost->m_tTimestamp = common::getDateTimeUTC();
	m_lHosts.prepend( pHost );
	return pHost;
}

void CHostCache::remove(CHostCacheHost* pRemove)
{
	CHostCacheIterator it = find( pRemove );

	if ( it != m_lHosts.end() )
	{
		m_lHosts.erase( it );
	}

	delete pRemove;
}

void CHostCache::remove(CEndPoint oHost)
{
	CHostCacheIterator it = find( oHost );

	if ( it != m_lHosts.end() )
	{
		delete *it;
		m_lHosts.erase( it );
	}
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
		add( addr, oTs );
	}
}

QString CHostCache::getXTry()
{
	const quint32 nMax = 10;

	QString sRet;
	quint32 nCount = 0;

	if ( !m_lHosts.size() )
	{
		return QString();
	}

	foreach ( CHostCacheHost* pHost, m_lHosts )
	{
		if ( !pHost->m_nFailures )
		{
			sRet.append( pHost->m_oAddress.toStringWithPort() + " " );
			sRet.append( pHost->m_tTimestamp.toString( "yyyy-MM-ddThh:mmZ" ) );
			sRet.append( "," );

			++nCount;

			if ( nCount == nMax )
				break;
		}
	}

	if ( sRet.isEmpty() )
	{
		return QString();
	}

	return "X-Try-Hubs: " + sRet.left( sRet.size() - 1 );
}

void CHostCache::onFailure(CEndPoint addr)
{
	CHostCacheIterator itHost = find( addr );

	if ( itHost != m_lHosts.end() )
	{
		if ( (int)(++(*itHost)->m_nFailures) > quazaaSettings.Connection.FailureLimit )
		{
			remove( addr );
		}
	}
}

CHostCacheHost* CHostCache::get()
{
	CHostCacheHost* pHost = NULL;

	if ( !m_lHosts.size() )
	{
		return pHost;
	}

	pHost = m_lHosts.first();
	m_lHosts.removeFirst();

	return pHost;
}

CHostCacheHost* CHostCache::getConnectable(QDateTime tNow, QList<CHostCacheHost*> oExcept,
										   QString sCountry)
{
	bool bCountry = ( sCountry != "ZZ" );

	if ( m_lHosts.isEmpty() )
	{
		return NULL;
	}

	// First try untested or working hosts, then fall back to failed hosts to increase chances for
	// successful connection
	for ( int nFailures = 0; nFailures < quazaaSettings.Connection.FailureLimit; ++nFailures )
	{
		foreach ( CHostCacheHost * pHost, m_lHosts )
		{
			if ( nFailures != pHost->m_nFailures )
				continue;

			if ( !pHost->m_bCountryObtained )
			{
				pHost->m_bCountryObtained = true;
			}

			if ( bCountry && pHost->m_oAddress.country() != sCountry )
			{
				continue;
			}

			if ( pHost->m_tLastConnect.isValid() ||
			     pHost->m_tLastConnect.secsTo( tNow ) > ( quazaaSettings.Gnutella.ConnectThrottle
			         + pHost->m_nFailures * quazaaSettings.Connection.FailurePenalty ) )
			{
				if ( !oExcept.contains( pHost ) )
					return pHost;
			}
		}
	}

	return NULL;
}

bool CHostCache::save()
{
	ASSUME_LOCK( hostCache.m_pSection );

	bool bReturn;

	quint32 nCount = common::securredSaveFile( common::globalDataFiles, "hostcache.dat", m_sMessage,
	                                           this, &CHostCache::writeToFile );
	if ( nCount )
	{
		m_tLastSave = common::getDateTimeUTC();
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

void CHostCache::load()
{
	m_sMessage = QObject::tr( "[Host Cache] " );

	ASSUME_LOCK( hostCache.m_pSection );

	QFile file( common::getLocation( common::globalDataFiles ) + "hostcache.dat" );

	if ( !file.exists() || !file.open( QIODevice::ReadOnly ) )
		return;

	QDataStream oStream( &file );

	quint16 nVersion;
	quint32 nCount;

	oStream >> nVersion;
	oStream >> nCount;

	if ( nVersion == HOST_CACHE_CODE_VERSION ) // else do load defaults
	{
		CEndPoint oAddress;
		quint32 nFailures    = 0;
		quint32 tTimeStamp   = 0;
		quint32 tLastConnect = 0;
		QDateTime timeStamp;

		const quint32 tNow   = common::getTNowUTC();

		CHostCacheHost* pHost = NULL;

		while ( nCount )
		{
			oStream >> oAddress;
			oStream >> nFailures;

			oStream >> tTimeStamp;
			oStream >> tLastConnect;

			timeStamp.setTimeSpec( Qt::UTC );
			timeStamp.setTime_t( tTimeStamp );

			pHost = add( oAddress, timeStamp );
			if ( pHost )
			{
				if ( tLastConnect - tNow > 0 ) // Note: add() handles the same for tTimeStamp.
					tLastConnect = tNow - 60;

				pHost->m_nFailures    = nFailures;
				pHost->m_tLastConnect.setTimeSpec( Qt::UTC);
				pHost->m_tLastConnect.setTime_t( tLastConnect );

				Q_ASSERT( pHost->m_tLastConnect.timeSpec() == Qt::UTC );
				Q_ASSERT( pHost->m_tLastConnect.toTime_t() <= common::getTNowUTC() );
			}

			--nCount;
			pHost = NULL;
		}
	}

	file.close();

	pruneOldHosts();

	systemLog.postLog( LogSeverity::Debug,
	                   m_sMessage + QObject::tr( "Loaded %1 hosts." ).arg( m_lHosts.size() ) );
}

void CHostCache::pruneOldHosts()
{
	QDateTime tNow = common::getDateTimeUTC();

	QMutableListIterator<CHostCacheHost*> it( m_lHosts );
	it.toBack();

	while ( it.hasPrevious() )
	{
		it.previous();
		if ( it.value()->m_tTimestamp.secsTo( tNow ) > quazaaSettings.Gnutella2.HostExpire )
		{
			delete it.value();
			it.remove();
		}
		else
		{
			break;
		}
	}
}

void CHostCache::pruneByQueryAck()
{
	QDateTime tNow = common::getDateTimeUTC();

	for ( CHostCacheIterator it = m_lHosts.begin(); it != m_lHosts.end(); )
	{
		if ( !(*it)->m_tAck.isNull() &&
			 (*it)->m_tAck.secsTo(tNow) > (long)quazaaSettings.Gnutella2.QueryHostDeadline )
		{
			delete *it;
			it = m_lHosts.erase( it );
		}
		else
		{
			++it;
		}
	}
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
	const quint32 nCount   = (quint32)pHostCache->m_lHosts.size();

	oStream << nVersion;
	oStream << nCount;

	if ( nCount )
	{
		foreach ( CHostCacheHost* pHost, pHostCache->m_lHosts )
		{
			oStream << pHost->m_oAddress;
			oStream << pHost->m_nFailures;

			Q_ASSERT(pHost->m_tTimestamp.timeSpec() == Qt::UTC);
			Q_ASSERT(pHost->m_tLastConnect.isValid() && pHost->m_tLastConnect.timeSpec() == Qt::UTC);

			oStream << (quint32)(pHost->m_tTimestamp.toTime_t());
			oStream << (quint32)(pHost->m_tLastConnect.toTime_t());
		}
	}

	return nCount;
}

