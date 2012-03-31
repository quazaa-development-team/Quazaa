#include "discoveryservice.h"
#include "discoveryservicemanager.h"


/**
  * Empty constructor.
  * Locking: /
  */
CDiscoveryService::CDiscoveryService() :
	m_nServiceType( srNull ),
	m_nNetworkType( srNoNet ),
	m_sServiceURL( "" ),
	m_nRating( 0 ),
	m_nProbabilityMultiplicator( 0 ),
	m_oUUID(),
	m_nRequest( srNoRequest )
{
}

/**
  * Default constructor.
  * Locking: /
  */
CDiscoveryService::CDiscoveryService(QString sURL, ServiceType nSType,
									 NetworkType nNType, quint8 nRating, QUuid oID) :
	m_nServiceType( nSType ),
	m_nNetworkType( nNType ),
	m_sServiceURL( sURL ),
	m_nRating( nRating ),
	m_oUUID( oID ),
	m_nRequest( srNoRequest )
{
	m_nProbabilityMultiplicator = ( nRating < 5 ) ? nRating : 5;
}

/**
  * [static] Writes pService to given QDataStream.
  * Locking: R
  */
void CDiscoveryService::save(const CDiscoveryService* const pService, QDataStream &oStream)
{
	QReadLocker l( &( const_cast<CDiscoveryService*>(pService) )->m_pRWLock );

	oStream << (quint8)(pService->m_nServiceType);
	oStream << (quint8)(pService->m_nNetworkType);
	oStream << (quint8)(pService->m_nRating);
	oStream << pService->m_sServiceURL;
}

/**
  * [static] Reads a service from the provided QDataStream and creates a new Object from the data.
  * Note that if a non-NULL pointer is given to the function, that object is deleted.
  * Locking: RW
  */
void CDiscoveryService::load(CDiscoveryService*& pService, QDataStream &oStream, int)
{
	if ( pService )
	{
		delete pService;
		pService = NULL;
	}

	pService = new CDiscoveryService();
	QWriteLocker l( &pService->m_pRWLock );

	quint8		nServiceType;
	quint8		nNetworkType;
	quint8		nRating;
	QString		sServiceURL;

	oStream >> nServiceType;
	oStream >> nNetworkType;
	oStream >> nRating;
	oStream >> sServiceURL;

	switch ( nServiceType )
	{
	default:
		;
	}

	pService->m_nServiceType = (ServiceType)nServiceType;
	pService->m_nNetworkType = (NetworkType)nNetworkType;
	pService->m_nRating      = nRating;
	pService->m_sServiceURL  = sServiceURL;

	pService->m_nProbabilityMultiplicator = ( nRating < 5 ) ? nRating : 5;
}

/**
  * Updates the service with the own IP if the service supports it.
  * Locking: RW
  */
void CDiscoveryService::update(CEndPoint& oOwnIP)
{
	m_pRWLock.lockForWrite();
	m_nRequest = srUpdate;
	m_oOwnIP = oOwnIP;
	m_pRWLock.unlock();

	run(); // Start new thread.
}

/**
  * Queries the service to recieve network hosts for initial connection and/or alternative service URLs.
  * Locking: RW
  */
void CDiscoveryService::query()
{
	m_pRWLock.lockForWrite();
	m_nRequest = srQuery;
	m_pRWLock.unlock();

	run(); // Start new thread.
}

/**
  * Starts new thread.
  * Locking: R
  */
void CDiscoveryService::run()
{
	// Lock the service while running the thread to prevent service
	// access or removal while the service is active.
	QReadLocker l( &m_pRWLock );

	switch ( m_nRequest )
	{
	case srQuery:
	{
		doQuery();
		break;
	}

	case srUpdate:
	{
		doUpdate();
		break;
	}

	default:
		; // Do nothing.
	}
}
