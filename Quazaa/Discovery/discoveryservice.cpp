/*
** discoveryservice.cpp
**
** Copyright © Quazaa Development Team, 2012.
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

#include "discoveryservice.h"
#include "gwc.h"
#include "banneddiscoveryservice.h"

#include "quazaasettings.h"
#include "timedsignalqueue.h"

using namespace Discovery;

/**
 * @brief CDiscoveryService: Constructor. Creates a new service.
 * @param oURL
 * @param oNType
 * @param nRating
 */
CDiscoveryService::CDiscoveryService(const QUrl& oURL, const CNetworkType& oNType, quint8 nRating) :
	m_nServiceType( stNull ),
	m_oNetworkType( oNType ),
	m_oServiceURL( oURL ),
	m_nRating( nRating ),
	m_nProbaMult( ( nRating > DISCOVERY_MAX_PROBABILITY ) ? DISCOVERY_MAX_PROBABILITY : nRating ),
	m_bBanned( false ),
	m_bZero( true ),     // mark new service as having been zero -> gets downrated fast if non functional
	m_nID( 0 ),          // invalid ID
	m_nLastHosts( 0 ),
	m_nTotalHosts( 0 ),
	m_tLastQueried( 0 ),
	m_tLastSuccess( 0 ),
	m_nFailures( 0 ),
	m_nZeroRevivals( 0 ),
	m_bRunning( false )
{
}

/**
 * @brief CDiscoveryService: Copy constructor. Copies all but the list of registered pointers.
 * @param pService
 */
CDiscoveryService::CDiscoveryService(const CDiscoveryService& pService) :
	m_bRunning( false )
{
	// The usage of a custom copy constructor makes sure the list of registered
	// pointers is NOT forwarded to a copy of this service.

	m_nServiceType  = pService.m_nServiceType;
	m_oNetworkType  = pService.m_oNetworkType;
	m_oServiceURL   = pService.m_oServiceURL;
	m_nRating       = pService.m_nRating;
	m_bBanned       = pService.m_bBanned;
	m_bZero         = pService.m_bZero;
	m_nID           = pService.m_nID;
	m_nLastHosts    = pService.m_nLastHosts;
	m_nTotalHosts   = pService.m_nTotalHosts;
	m_tLastQueried  = pService.m_tLastQueried;
	m_tLastSuccess  = pService.m_tLastSuccess;
	m_nFailures     = pService.m_nFailures;

	m_nZeroRevivals = pService.m_nZeroRevivals;
	m_nProbaMult    = pService.m_nProbaMult;
}

/**
 * @brief ~CDiscoveryService
 */
CDiscoveryService::~CDiscoveryService()
{
	// Set all pointers to this rule to NULL to notify them about the deletion of this object.
	for ( std::list<const CDiscoveryService**>::iterator i = m_lPointers.begin();
		  i != m_lPointers.end(); ++i )
	{
		*(*i) = nullptr;
	}
}

/**
 * @brief operator ==: Allows to compare two services. Services are considered to be equal if they are
 * of the same service type, serve the same networks and have the same URL.
 * Requires locking: R (both services)
 * @param pService
 * @return
 */
bool CDiscoveryService::operator==(const CDiscoveryService& pService) const
{
	return ( m_nServiceType	== pService.m_nServiceType	&&
			 m_oNetworkType	== pService.m_oNetworkType	&&
			 m_oServiceURL	== pService.m_oServiceURL	);
}

/**
 * @brief operator !=: Allows to compare two services. Services are considered to be equal if they are
 * of the same service type, serve the same networks and have the same URL.
 * Requires locking: R (both services)
 * @param pService
 * @return
 */
bool CDiscoveryService::operator!=(const CDiscoveryService& pService) const
{
	return !( *this == pService );
}

/**
 * @brief load reads a service from the provided QDataStream and creates a new Object from the data.
 * Note that if a non-NULL pointer is given to the function, that object is deleted.
 * Locking: / (static member)
 * @param pService
 * @param fsStream
 * @param nVersion
 */
void CDiscoveryService::load(CDiscoveryService*& pService, QDataStream &fsFile, int)
{
	if ( pService )
	{
		delete pService;
		pService = nullptr;
	}

	quint8     nServiceType;        // GWC, UKHL, ...
	quint16    nNetworkType;        // could be several in case of GWC for instance
	QString    sURL;
	quint8     nRating;             // 0: bad; 10: very good
	bool       bBanned;             // service URL is blocked
	bool       bZero;
	TServiceID nID;                 // ID used by the manager to identify the service
	quint32    nLastHosts;          // hosts returned by the service on last query
	quint32    nTotalHosts;         // all hosts we ever got from the service
	quint32    tLastQueried;        // last time we accessed the host
	quint32    tLastSuccess;        // last time we queried the service successfully
	quint8     nFailures;
	quint8     nZeroRatingFailures;

	fsFile >> nServiceType;
	fsFile >> nNetworkType;
	fsFile >> sURL;
	fsFile >> nRating;
	fsFile >> bBanned;
	fsFile >> bZero;
	fsFile >> nID;
	fsFile >> nLastHosts;
	fsFile >> nTotalHosts;
	fsFile >> tLastQueried;
	fsFile >> tLastSuccess;
	fsFile >> nFailures;
	fsFile >> nZeroRatingFailures;

	pService = createService( sURL, (TServiceType)nServiceType,
							  CNetworkType( nNetworkType ), nRating );

	pService->m_bBanned       = bBanned;
	pService->m_bZero         = bZero;
	pService->m_nID           = nID;
	pService->m_nLastHosts    = nLastHosts;
	pService->m_nTotalHosts   = nTotalHosts;
	pService->m_tLastQueried  = tLastQueried;
	pService->m_tLastSuccess  = tLastSuccess;
	pService->m_nFailures     = nFailures;
	pService->m_nZeroRevivals = nZeroRatingFailures;
}

/**
 * @brief save writes pService to given QDataStream.
 * Locking: / (static member)
 * @param pService
 * @param fsFile
 */
void CDiscoveryService::save(const CDiscoveryService* const pService, QDataStream &fsFile)
{
	fsFile << (quint8)(pService->m_nServiceType);
	fsFile << (quint16)(pService->m_oNetworkType.toQuint16());
	fsFile << pService->m_oServiceURL.toString();
	fsFile << (quint8)(pService->m_nRating);
	fsFile << pService->m_bBanned;
	fsFile << pService->m_bZero;
	fsFile << pService->m_nID;
	fsFile << pService->m_nLastHosts;
	fsFile << pService->m_nTotalHosts;
	fsFile << pService->m_tLastQueried;
	fsFile << pService->m_tLastSuccess;
	fsFile << pService->m_nFailures;
	fsFile << pService->m_nZeroRevivals;
}

/**
 * @brief createService allows to create valid services.
 * Locking: / (static member)
 * @param sURL
 * @param eSType
 * @param oNType
 * @param nRating
 * @return
 */
CDiscoveryService* CDiscoveryService::createService(const QString& sURL, TServiceType eSType,
													const CNetworkType& oNType,	quint8 nRating )
{
	CDiscoveryService* pService = nullptr;

	switch ( eSType )
	{
	case stNull:
		break;
	case stBanned:
	{
		pService = new CBannedDiscoveryService( sURL, oNType, nRating );
		break;
	}
	case stGWC:
	{
		pService = new CGWC( sURL, oNType, nRating );
		break;
	}
	default:
		systemLog.postLog( LogSeverity::Error, discoveryManager.m_sMessage
						   + tr( "Internal error: Creation of service with unknown type requested: Type " )
						   + QString( eSType ) );

		Q_ASSERT( false ); // unsupported service type
	}

	return pService;
}

/**
 * @brief update sends our own IP to service if the service supports the operation (e.g. if it is a GWC).
 * Locking: RW
 * @param oOwnIP
 */
void CDiscoveryService::update()
{
	m_oRWLock.lockForWrite();

	Q_ASSERT( !m_bRunning );

	m_bRunning = true;

	doUpdate();

	m_oRWLock.unlock();

	quint32 tNow = static_cast< quint32 >( QDateTime::currentDateTimeUtc().toTime_t() );
	m_oUUID = signalQueue.push( this, SLOT( cancelRequest() ),
								tNow + quazaaSettings.Discovery.ServiceTimeout );
}

/**
 * @brief query accesses the service to recieve network hosts for initial connection and/or
 * alternative service URLs.
 * Locking: RW
 */
void CDiscoveryService::query()
{
	qDebug() << "[Discovery] Querying service.";





	// TODO: Find out why this is locked when it shouldn't be.





	m_oRWLock.lockForWrite();

	qDebug() << "[Discovery] Got service lock.";

	Q_ASSERT( !m_bRunning );

	m_bRunning = true;

	doQuery();

	m_oRWLock.unlock();

	qDebug() << "[Discovery] Released service lock.";

	quint32 tNow = static_cast< quint32 >( QDateTime::currentDateTimeUtc().toTime_t() );
	m_oUUID = signalQueue.push( this, SLOT( cancelRequest() ),
								tNow + quazaaSettings.Discovery.ServiceTimeout );

	qDebug() << "[Discovery] Finished querying service.";
}

/**
 * @brief cancelRequest stops any update or query operation currently in progress.
 * Locking: RW
 */
void CDiscoveryService::cancelRequest()
{
	m_oRWLock.lockForWrite();

	if ( m_bRunning )
	{
		doCancelRequest();
	}

	m_oRWLock.unlock();
}

void CDiscoveryService::lockForRead() const
{
	CDiscoveryService* pModifiable = const_cast<CDiscoveryService*>( this );
	pModifiable->m_oRWLock.lockForRead();
}

void CDiscoveryService::unlock() const
{
	CDiscoveryService* pModifiable = const_cast<CDiscoveryService*>( this );
	pModifiable->m_oRWLock.unlock();
}

void CDiscoveryService::updateStatistics(bool bQuery, quint16 nHosts)
{
	if ( bQuery )
		m_nLastHosts = nHosts;

	if ( m_bZero ) // We're dealing with a newly revived service that has been known to fail in the past.
	{
		if ( nHosts ) // access successful
		{
			// increase rating
			setRating( m_nRating + 1 );

			if ( bQuery )
			{
				m_nTotalHosts += nHosts;
			}

			m_tLastSuccess = m_tLastQueried;
			m_nFailures = 0;

			// revival try successful
			m_bZero = false;
			m_nZeroRevivals = 0;
		}
		else // fail
		{
			// revival failed
			setRating( 0 );
			++m_nFailures;

			if ( m_nZeroRevivals >= quazaaSettings.Discovery.ZeroRatingRevivalTries )
			{
				// Maximum allowed revival tries reached. Assume service no longer operational.
				m_bBanned = true;
			}
		}
	}
	else // We're dealing with a normal service.
	{
		if ( nHosts ) // access successful
		{
			// increase rating
			setRating( m_nRating + 1 );

			if ( bQuery )
			{
				m_nTotalHosts += nHosts;
			}

			m_tLastSuccess = m_tLastQueried;
			m_nFailures = 0;
		}
		else // fail
		{
			++m_nFailures;

			if ( quazaaSettings.Discovery.FailureLimit &&
				 m_nFailures >= quazaaSettings.Discovery.FailureLimit )
			{
				// maximum allowed failures reached
				setRating( 0 );
			}
			else
			{
				setRating( (m_nRating > 0) ? m_nRating - 1 : 0 );
			}

			if ( !m_nRating )
			{
				m_bZero = true;

				// if no retrial at a later time is wanted, ban the service for good
				if ( !quazaaSettings.Discovery.ZeroRatingRevivalTries )
				{
					m_bBanned = true;
				}
			}
		}
	}

	// remove cancel request from signal queue
	signalQueue.pop( m_oUUID );
	m_oUUID = QUuid();
}

void CDiscoveryService::setRating(quint8 nRating)
{
	m_nRating    = ( nRating > quazaaSettings.Discovery.MaximumServiceRating ) ?
					   quazaaSettings.Discovery.MaximumServiceRating : nRating;
	m_nProbaMult = ( nRating > DISCOVERY_MAX_PROBABILITY ) ? DISCOVERY_MAX_PROBABILITY : nRating;
}
