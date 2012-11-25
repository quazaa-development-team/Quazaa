#include "discoveryservice.h"
#include "gwc.h"

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
	m_nProbabilityMultiplicator( ( nRating < 5 ) ? nRating : 5 ),
	m_bBanned( false ),
	m_nID( 0 ), // invalid ID
	m_nLastHosts( 0 ),
	m_nTotalHosts( 0 ),
	m_tLastQueried( 0 ),
	m_tLastSuccess( 0 ),
	m_nFailures( 0 ),
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

	m_nServiceType	= pService.m_nServiceType;
	m_oNetworkType	= pService.m_oNetworkType;
	m_oServiceURL	= pService.m_oServiceURL;
	m_nRating		= pService.m_nRating;
	m_bBanned		= pService.m_bBanned;
	m_nID			= pService.m_nID;
	m_nLastHosts	= pService.m_nLastHosts;
	m_nTotalHosts	= pService.m_nTotalHosts;
	m_tLastQueried	= pService.m_tLastQueried;
	m_tLastSuccess	= pService.m_tLastSuccess;
	m_nFailures		= pService.m_nFailures;

	m_nProbabilityMultiplicator	= pService.m_nProbabilityMultiplicator;
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

	quint8			nServiceType; // GWC, UKHL, ...
	quint16			nNetworkType; // could be several in case of GWC for instance
	QString			sURL;
	quint8			nRating;      // 0: bad; 10: very good
	bool			bBanned;      // service URL is blocked
	TDiscoveryID	nID;          // ID used by the manager to identify the service
	quint32			nLastHosts;   // hosts returned by the service on last query
	quint32			nTotalHosts;  // all hosts we ever got from the service
	quint32			tLastQueried; // last time we accessed the host
	quint32			tLastSuccess; // last time we queried the service successfully
	quint8			nFailures;

	fsFile >> nServiceType;
	fsFile >> nNetworkType;
	fsFile >> sURL;
	fsFile >> nRating;
	fsFile >> bBanned;
	fsFile >> nID;
	fsFile >> nLastHosts;
	fsFile >> nTotalHosts;
	fsFile >> tLastQueried;
	fsFile >> tLastSuccess;
	fsFile >> nFailures;

	pService = createService( sURL, (TServiceType)nServiceType,
							  CNetworkType( nNetworkType ), nRating );

	pService->m_bBanned      = bBanned;
	pService->m_nID          = nID;
	pService->m_nLastHosts   = nLastHosts;
	pService->m_nTotalHosts  = nTotalHosts;
	pService->m_tLastQueried = tLastQueried;
	pService->m_tLastSuccess = tLastSuccess;
	pService->m_nFailures    = nFailures;
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
	fsFile << pService->m_nID;
	fsFile << pService->m_nLastHosts;
	fsFile << pService->m_nTotalHosts;
	fsFile << pService->m_tLastQueried;
	fsFile << pService->m_tLastSuccess;
	fsFile << pService->m_nFailures;
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
		Q_ASSERT( false ); // should not happen anyway
		break;
	case stGWC:
	{
		pService = new CGWC( sURL, oNType, nRating );
		break;
	}
	default:
		systemLog.postLog( LogSeverity::Error, discoveryManager.m_sMessage
+ tr( "Internal error: Creation of service with unknown type requested in CDiscoveryService::createService: " )
						   + eSType );

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

// TODO: Add interval from settings to cancellation
	m_oUUID = signalQueue.push( this, SLOT( cancelRequest() ), (quint32)QDateTime::currentDateTime().toTime_t() );
}

/**
 * @brief query accesses the service to recieve network hosts for initial connection and/or
 * alternative service URLs.
 * Locking: RW
 */
void CDiscoveryService::query()
{
	m_oRWLock.lockForWrite();

	Q_ASSERT( !m_bRunning );

	m_bRunning = true;

	doQuery();

	m_oRWLock.unlock();

// TODO: Add interval from settings to cancellation
	m_oUUID = signalQueue.push( this, SLOT( cancelRequest() ), (quint32)QDateTime::currentDateTime().toTime_t() );
}

void CDiscoveryService::serviceActionFinished()
{
	Q_ASSERT( !m_oUUID.isNull() );

// TODO: Do more checks.

	signalQueue.pop( m_oUUID );

	m_oUUID = QUuid();
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

/**
 * @brief registerPointer registers a pointer pointing to the service. All registered pointers to this
 * service will be set to NULL upon deletion of the service. All registered pointers need to be
 * unregistered before removing them from memory.
 * Note that this method is const, because it does not change the data defining the DiscoveryService in
 * question. It does, however, add the pointer pService to an internal storage structure. So make sure to
 * lock the service for write when using.
 * Requires locking: RW
 * @param pService
 */
void CDiscoveryService::registerPointer(const CDiscoveryService** pService) const
{
	// Children, don't repeat this at home! :D
	CDiscoveryService* pModifiable = const_cast<CDiscoveryService*>(this);

	pModifiable->m_oRWLock.lockForWrite();
	pModifiable->m_lPointers.push_back( pService );
	pModifiable->m_oRWLock.unlock();
}

/**
 * @brief unRegisterPointer unregisters a pointer previously registered.
 * Note that this method is const, because it does not change the data defining the DiscoveryService in
 * question. It does, however, add the pointer pService to an internal storage structure. So be careful
 * when using and make sure you have a write lock on the service.
 * Requires locking: RW
 * @param pService
 */
void CDiscoveryService::unRegisterPointer(const CDiscoveryService** pService) const
{
	// Children, don't repeat this at home! :D
	CDiscoveryService* pModifiable = const_cast<CDiscoveryService*>(this);

	pModifiable->m_oRWLock.lockForWrite();
	pModifiable->m_lPointers.remove( pService );
	pModifiable->m_oRWLock.unlock();
}
