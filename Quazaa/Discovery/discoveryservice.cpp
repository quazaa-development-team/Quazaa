#include "discoveryservice.h"
#include "discoveryservicemanager.h"

#include "gwc.h"

CNetworkType::CNetworkType() :
	m_nNetworks( 0 )
{}

CNetworkType::CNetworkType(quint16 type) :
	m_nNetworks( type )
{}

CNetworkType::CNetworkType(NetworkType type) :
	m_nNetworks( (quint16)type )
{}

bool CNetworkType::isGnutella() const
{
	return ( m_nNetworks | gnutella );
}

void CNetworkType::setGnutella( bool )
{
	m_nNetworks &= gnutella;
}

bool CNetworkType::isG2() const
{
	return ( m_nNetworks | G2 );
}

void CNetworkType::setG2( bool )
{
	m_nNetworks &= G2;
}

bool CNetworkType::isAres() const
{
	return ( m_nNetworks | Ares );
}

void CNetworkType::setAres( bool )
{
	m_nNetworks &= Ares;
}

bool CNetworkType::isEDonkey2000() const
{
	return ( m_nNetworks | eDonkey2000 );
}

void CNetworkType::setEDonkey2000( bool )
{
	m_nNetworks &= eDonkey2000;
}

bool CNetworkType::isNetwork(const CNetworkType& type) const
{
	return ( m_nNetworks & type.toQuint16() ) == type.toQuint16();
}

void CNetworkType::setNetwork(const CNetworkType& type)
{
	m_nNetworks &= type.toQuint16();
}

bool CNetworkType::isMulti() const
{
	if ( m_nNetworks < 2 )
		return false;

	while ( m_nNetworks > 1 )
	{
		if ( m_nNetworks % 2 )
			return true;
	}
	return false;
}

quint16 CNetworkType::toQuint16() const
{
	return m_nNetworks;
}


/**
  * Empty constructor.
  * Locking: /
  */
CDiscoveryService::CDiscoveryService() :
	m_nServiceType( srNull ),
	m_oNetworkType(),
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
CDiscoveryService::CDiscoveryService(const QString& sURL, const ServiceType,
									 const CNetworkType& oNType,
									 const quint8 nRating, const QUuid& oID) :
	m_nServiceType( srNull ),
	m_oNetworkType( oNType ),
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
	oStream << (quint16)(pService->m_oNetworkType.toQuint16());
	oStream << (quint8)(pService->m_nRating);
	oStream << pService->m_sServiceURL;
}

/**
  * [static] Creates a new service...
  * Locking: /
  */
CDiscoveryService* CDiscoveryService::createService(const QString& sURL, const ServiceType nSType,
													const CNetworkType& oNType,	const quint8 nRating,
													const QUuid& oID)
{
	CDiscoveryService* pService = NULL;

	switch ( nSType )
	{
	case srGWC:
	{
		pService = new CGWC( sURL, nSType, oNType, nRating, oID );
		break;
	}
	}

	return pService;
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

	pService->m_nServiceType = (ServiceType)nServiceType;
	pService->m_oNetworkType = CNetworkType( nNetworkType );
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
  * Queries the service to recieve network hosts for initial connection and/or
  * alternative service URLs.
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
