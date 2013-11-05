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

#include "searchmanager.h"
#include "managedsearch.h"
#include "queryhit.h"
#include "g2packet.h"
#include <QMutexLocker>
#include "hostcache.h"
#include "network.h"
#include <QMetaType>
#include "securitymanager.h"

#include "quazaasettings.h"
#include "commonfunctions.h"

#include "debug_new.h"

CSearchManager SearchManager;

const quint32 PacketsPerSec = 8;

CSearchManager::CSearchManager(QObject* parent) :
	QObject(parent)
{
	m_nPruneCounter = 0;
	m_nCookie = 0;

	qRegisterMetaType<QueryHitSharedPtr>("QueryHitSharedPtr");
}

void CSearchManager::Add(CManagedSearch* pSearch)
{
	QMutexLocker l(&m_pSection);

	Q_ASSERT(!m_lSearches.contains(pSearch->m_oGUID));
	m_lSearches.insert(pSearch->m_oGUID, pSearch);
	if(pSearch->thread() != SearchManager.thread())
	{
		pSearch->moveToThread(SearchManager.thread());
	}
}

void CSearchManager::Remove(CManagedSearch* pSearch)
{
	QMutexLocker l(&m_pSection);

	Q_ASSERT(m_lSearches.contains(pSearch->m_oGUID));
	m_lSearches.remove(pSearch->m_oGUID);
}

CManagedSearch* CSearchManager::Find(QUuid& oGUID)
{
	return m_lSearches.value(oGUID, 0);
}

void CSearchManager::OnTimer()
{
	QMutexLocker l( &m_pSection );

	quint32 nSearches = m_lSearches.size();

	foreach ( CManagedSearch* pSearch, m_lSearches )
	{
		pSearch->SendHits();
		if ( pSearch->m_bPaused )
			--nSearches;
	}

	if ( !nSearches )
		return;

	const QDateTime tNowDT = common::getDateTimeUTC();

	++m_nPruneCounter;
	if ( !(m_nPruneCounter % 30) )
	{
		hostCache.pruneByQueryAck( tNowDT.toTime_t() );
	}

	quint32 nPacketsLeft = PacketsPerSec;
	quint32 nPacketsPerSearch = qMin( 4u, nPacketsLeft / nSearches + 1u );


	quint32 nExecuted = 0;

	foreach ( CManagedSearch* pSearch, m_lSearches )
	{
		if ( pSearch->m_bPaused )
			continue;

		if ( pSearch->m_nCookie != m_nCookie )
		{
			quint32 nPacket = qMin( nPacketsPerSearch, nPacketsLeft );
			pSearch->Execute( tNowDT, &nPacket );
			pSearch->m_nCookie = m_nCookie;
			nPacketsLeft -= ( (nPacketsLeft < nPacketsPerSearch) ?
							  (nPacketsLeft - nPacket) : (nPacketsPerSearch - nPacket) );
			++nExecuted;

			if ( !nPacketsLeft )
				break;
		}
	}

	if ( nPacketsLeft || nExecuted == nSearches )
	{
		++m_nCookie;
	}
}

bool CSearchManager::OnQueryAcknowledge(G2Packet* pPacket, CEndPoint& addr, QUuid& oGUID)
{
	if ( !pPacket->m_bCompound )
	{
		return false;
	}

	pPacket->skipCompound();			// skip children to get search GUID
	if ( pPacket->getRemaining() < 16 )	// must be at least 16 bytes for GUID
	{
		return false;
	}

	oGUID = pPacket->readGUID();		// Read search GUID

	QMutexLocker l( &m_pSection );

	if ( CManagedSearch* pSearch = Find( oGUID ) )	// is it our Query Ack?
	{
		// YES, this is ours, let's parse the packet and process it

		CEndPoint oFromIp = addr;
		QList<CEndPoint>  lDone;
		QHash<CEndPoint, quint32>  lSuggested;
		quint32 nRetryAfter = 0;
		qint64 tAdjust = 0;
		QString sVendor;
		bool bLikelyFoxy = false;
		const quint32 tNow = common::getTNowUTC();

		quint32 nHubs = 0, nLeaves = 0, nSuggestedHubs = 0;

		pPacket->m_nPosition = 0;	// reset position

		char szType[9];
		quint32 nLength = 0, nNext = 0;

		while ( pPacket->readPacket( &szType[0], nLength ) )
		{
			nNext = pPacket->m_nPosition + nLength;

			if ( strcmp( "D", szType) == 0 && nLength >= 4 )
			{
				CEndPoint ha;
				if ( nLength >= 16 )
				{
					// IPv6
					Q_IPV6ADDR nIP;
					pPacket->read( &nIP, 16 );
					ha.setAddress( nIP );

					if ( nLength >= 18 )
					{
						quint16 nPort = pPacket->readIntLE<quint16>();
						ha.setPort(nPort);
					}

					if ( nLength >= 20 )
					{
						nLeaves += pPacket->readIntLE<quint16>();
					}
				}
				else
				{
					// IPv4
					quint32 nIP = pPacket->readIntBE<quint32>();
					ha.setAddress( nIP );

					if ( nLength >= 6 )
					{
						quint16 nPort = pPacket->readIntLE<quint16>();
						ha.setPort(nPort);
					}

					if ( nLength >= 8 )
					{
						nLeaves += pPacket->readIntLE<quint16>();
					}
				}
				lDone.append( ha );

				++nHubs;
			}
			else if ( strcmp( "S", szType ) == 0 && nLength >= 6 )
			{
				CEndPoint a;
				pPacket->readHostAddress( &a, !( nLength >= 18 ) );
				const quint32 tTimeStamp = ( nLength >= (a.protocol() == 0 ? 10u : 22u) ) ?
											 pPacket->readIntLE<quint32>() + tAdjust : tNow - 60;

				lSuggested.insert(a, tTimeStamp);

				++nSuggestedHubs;
			}
			else if ( strcmp("TS", szType) == 0 && nLength >= 4 )
			{
				tAdjust = tNow - pPacket->readIntLE<quint32>();
			}
			else if ( strcmp("RA", szType) == 0 && nLength >= 2 )
			{
				if ( nLength >= 4 )
				{
					nRetryAfter = pPacket->readIntLE<quint32>();
				}
				else if ( nLength >= 2 )
				{
					nRetryAfter = pPacket->readIntLE<quint16>();
				}

				hostCache.m_pSection.lock();
				CHostCacheHost* pHost = hostCache.take( oFromIp );
				if ( pHost )
				{
					pHost->m_tRetryAfter = tNow + nRetryAfter;
				}
				hostCache.m_pSection.unlock();
			}
			else if ( strcmp("FR", szType) == 0 && nLength >= 4 )
			{
				if ( nLength >= 16 )
				{
					Q_IPV6ADDR ip;
					pPacket->read( &ip, 16 );
					oFromIp.setAddress( ip );
				}
				else
				{
					quint32 nFromIp = pPacket->readIntBE<quint32>();
					oFromIp.setAddress( nFromIp );
				}
			}
			else if( strcmp("V", szType) == 0 && nLength >= 4 )
			{
				sVendor = pPacket->readString( 4 );
			}

			pPacket->m_nPosition = nNext;
		}

		// we already know QA GUID

		bLikelyFoxy = (nHubs > 20 && nSuggestedHubs < 6 && nRetryAfter == 300);
		systemLog.postLog( LogSeverity::Debug, Components::Network,
						   "Processing query acknowledge from %s (time adjust %+d seconds): %d hubs, %d leaves, %d suggested hubs, retry after %d seconds, %s, likely foxy: %s).",
						   oFromIp.toString().toLocal8Bit().constData(), int( tAdjust ), nHubs,
						   nLeaves, nSuggestedHubs, nRetryAfter,
						   ( sVendor.isEmpty() ? "unknown" : qPrintable( sVendor ) ), (bLikelyFoxy ? "yes" : "no") );

		// Add hubs to the cache
		if( Q_LIKELY(!bLikelyFoxy) )
		{
			hostCache.m_pSection.lock();

			for( QList<CEndPoint>::iterator itHub = lDone.begin(); itHub != lDone.end(); itHub++ )
			{
				if( (*itHub).port() )
					hostCache.add(*itHub, tNow);
			}

			for( QHash<CEndPoint, quint32>::iterator itHub = lSuggested.begin(); itHub != lSuggested.end(); itHub++ )
			{
				if( itHub.key().port() )
					hostCache.add(itHub.key(), itHub.value());
			}

			hostCache.m_pSection.unlock();
		}
		else
		{
			CEndPoint oBanned = addr;

			if( Network.IsFirewalled() && !lDone.isEmpty() )
			{
				oBanned = lDone[0]; // 1st done hub, Shareaza does not include /FR child packet
			}

			securityManager.ban(oBanned, RuleTime::SixHours, true, tr("Likely Foxy client"));

			hostCache.m_pSection.lock();
			for( QList<CEndPoint>::iterator itHub = lDone.begin(); itHub != lDone.end(); itHub++ )
			{
				hostCache.remove(*itHub);
			}
			hostCache.m_pSection.unlock();
		}

		pSearch->m_nHubs += nHubs;
		pSearch->m_nLeaves += nLeaves;

		const QDateTime tNowDT = common::getDateTimeUTC();
		pSearch->OnHostAcknowledge( oFromIp, tNowDT );

		for ( int i = 0; i < lDone.size(); ++i )
		{
			pSearch->OnHostAcknowledge( lDone[i], tNowDT );
		}

		emit pSearch->StatsUpdated();

		return false;
	}

	// not our ack - tell caller to route it
	return true;

}

bool CSearchManager::OnQueryHit(G2Packet* pPacket, QueryHitInfo* pHitInfo)
{
	QMutexLocker l(&m_pSection);

	if(CManagedSearch* pSearch = Find(pHitInfo->m_oGUID))
	{
		// our search

		CQueryHit* pHit = CQueryHit::readPacket(pPacket, pHitInfo);

		if(pHit)
		{
			pSearch->OnQueryHit(pHit);
		}

		return false;
	}

	return true;
}

