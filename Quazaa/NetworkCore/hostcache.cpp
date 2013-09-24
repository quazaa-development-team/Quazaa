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
		tNow = QDateTime::currentDateTimeUtc();
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
	m_nKeyTime  = QDateTime::currentDateTimeUtc();
	m_nKeyHost  = pHost ? *pHost : Network.GetLocalAddress();
}

CHostCache::CHostCache():
	m_tLastSave( QDateTime::currentDateTimeUtc() ),
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

	QDateTime tNow = QDateTime::currentDateTimeUtc();

	if ( m_tLastSave.secsTo( tNow ) > 600 )
		save();

	if ( ts.isNull() || ts > tNow )
	{
		ts = tNow.addSecs( -60 );
	}

	CHostCacheIterator itPrev = find( host );

	if ( itPrev != m_lHosts.end() )
	{
		update( itPrev );
		return *m_lHosts.begin();
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

void CHostCache::update(CEndPoint oHost)
{
	CHostCacheIterator it = find( oHost );

	if ( it == m_lHosts.end() )
		return;

	update( it );
}

void CHostCache::update(CHostCacheHost* pHost)
{
	CHostCacheIterator it = find( pHost );

	if ( it == m_lHosts.end() )
		return;

	update( it );
}

void CHostCache::update(CHostCacheIterator itHost)
{
	CHostCacheHost* pHost = *itHost;
	m_lHosts.erase( itHost );
	pHost->m_tTimestamp = QDateTime::currentDateTimeUtc();
	m_lHosts.prepend( pHost );
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
			oTs = QDateTime::currentDateTimeUtc();
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
				++geoIP.m_nDebugOldCalls;
			}

			if ( bCountry && pHost->m_oAddress.country() != sCountry )
			{
				continue;
			}

			if ( pHost->m_tLastConnect.isNull() ||
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

void CHostCache::save()
{
	ASSUME_LOCK( hostCache.m_pSection );

#if QT_VERSION >= 0x050000
	QString sLocation = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
#else
	QString sLocation = QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
#endif

	sLocation = QString( "%1/.quazaa/" ).arg( sLocation );

	{
		QDir path = QDir( sLocation );
		if ( !path.exists() )
			path.mkpath( sLocation );
	}

	sLocation = QString( "%1hostcache.dat" ).arg( sLocation );

	if ( QFile::exists( sLocation ) && !QFile::remove( sLocation ) )
		return;

	QFile f( sLocation );

	if ( !f.open( QFile::WriteOnly ) )
		return;

	quint16 nVersion = HOST_CACHE_CODE_VERSION;
	quint32 nCount   = (quint32)m_lHosts.size();

	QDataStream s( &f );
	s << nVersion;
	s << nCount;

	if ( !m_lHosts.isEmpty() )
	{
		foreach ( CHostCacheHost* pHost, m_lHosts )
		{
			s << pHost->m_oAddress;
			s << pHost->m_tTimestamp;
			s << pHost->m_nFailures;
			s << pHost->m_tLastConnect;
		}
	}

	f.close();

	m_tLastSave = QDateTime::currentDateTimeUtc();

	systemLog.postLog( LogSeverity::Debug,
					   QObject::tr( "[Host Cache] Saved %1 hosts." ).arg( nCount ) );
}

void CHostCache::load()
{
	ASSUME_LOCK( hostCache.m_pSection );

#if QT_VERSION >= 0x050000
	QString sLocation = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
#else
	QString sLocation = QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
#endif

	sLocation = QString( "%1/.quazaa/" ).arg( sLocation );

	{
		QDir path = QDir( sLocation );
		if ( !path.exists() )
			path.mkpath( sLocation );
	}

	sLocation = QString( "%1hostcache.dat" ).arg( sLocation );

	QFile file( sLocation );

	if ( !file.exists() || !file.open( QIODevice::ReadOnly ) )
		return;

	QDataStream s( &file );

	quint16 nVersion;
	quint32 nCount;

	s >> nVersion;
	s >> nCount;

	if ( nVersion == HOST_CACHE_CODE_VERSION ) // else do load defaults
	{
		CEndPoint addr;
		QDateTime ts, lc;
		quint32 nFailures = 0;
		CHostCacheHost* pHost = NULL;

		while ( nCount )
		{
			s >> addr;
			s >> ts;
			s >> nFailures;
			s >> lc;

			if ( ts.timeSpec() == Qt::UTC && ( lc.isNull() || lc.timeSpec() == Qt::UTC ) )
			{
				pHost = add( addr, ts );
				if ( pHost )
				{
					pHost->m_nFailures    = nFailures;
					pHost->m_tLastConnect = lc;
				}
			}
			else
			{
				qDebug() << "[Host Cache] Caught problem with timestamp or last query time.";

				//TODO: Find out why the assert causes trouble with Qt4.8.4/MinGW4.4
				//Q_ASSERT( false );

				//break;
			}

			--nCount;
			pHost = NULL;
		}
	}

	file.close();

	pruneOldHosts();

	systemLog.postLog( LogSeverity::Debug,
					   QObject::tr( "[Host Cache] Loaded %1 hosts." ).arg( m_lHosts.size() ) );
}

void CHostCache::pruneOldHosts()
{
	QDateTime tNow = QDateTime::currentDateTimeUtc();

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
	QDateTime tNow = QDateTime::currentDateTimeUtc();

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

