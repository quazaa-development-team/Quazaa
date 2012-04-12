#include "discoveryservice.h"
#include "discovery.h"

#include "gwc.h"

using namespace Discovery;

CNetworkType::CNetworkType() :
	m_nNetworks( 0 )
{}

CNetworkType::CNetworkType(quint16 type) :
	m_nNetworks( type )
{}

CNetworkType::CNetworkType(DiscoveryProtocol type) :
	m_nNetworks( (quint16)type )
{}

bool CNetworkType::operator==(const CNetworkType& type) const
{
	return ( m_nNetworks == type.m_nNetworks );
}

bool CNetworkType::operator!=(const CNetworkType& type) const
{
	return ( m_nNetworks != type.m_nNetworks );
}

bool CNetworkType::isGnutella() const
{
	return ( m_nNetworks & dpgnutella );
}

void CNetworkType::setGnutella( bool )
{
	m_nNetworks |= dpgnutella;
}

bool CNetworkType::isG2() const
{
	return ( m_nNetworks & dpG2 );
}

void CNetworkType::setG2( bool )
{
	m_nNetworks |= dpG2;
}

bool CNetworkType::isAres() const
{
	return ( m_nNetworks & dpAres );
}

void CNetworkType::setAres( bool )
{
	m_nNetworks |= dpAres;
}

bool CNetworkType::isEDonkey2000() const
{
	return ( m_nNetworks & dpeDonkey2000 );
}

void CNetworkType::setEDonkey2000( bool )
{
	m_nNetworks |= dpeDonkey2000;
}

bool CNetworkType::isNetwork(const CNetworkType& type) const
{
	return ( m_nNetworks & type.toQuint16() ) == type.toQuint16();
}

void CNetworkType::setNetwork(const CNetworkType& type)
{
	m_nNetworks |= type.m_nNetworks;
}

bool CNetworkType::isMulti() const
{
	if ( !m_nNetworks || m_nNetworks && !(m_nNetworks & (m_nNetworks - 1)) )
		return false;	// m_nNetworks is 0 or power of 2
	else				// m_nNetworks is not 0 or a power of 2 - meaning there are at
		return true;	// least 2 different bits set to 1 within the 16 bit uint
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
	m_nServiceType( stNull ),
	m_oNetworkType(),
	m_oServiceURL(),
	m_nRating( 0 ),
	m_nProbabilityMultiplicator( 0 ),
	m_oUUID(),
	m_bQueued( false ),
	m_nRequest( srNoRequest )
{
}

/**
  * Default constructor.
  * Locking: /
  */
CDiscoveryService::CDiscoveryService(const QUrl& oURL, const ServiceType,
									 const CNetworkType& oNType,
									 const quint8 nRating, const QUuid& oID) :
	m_nServiceType( stNull ),
	m_oNetworkType( oNType ),
	m_oServiceURL( oURL ),
	m_nRating( nRating ),
	m_oUUID( oID ),
	m_bQueued( false ),
	m_nRequest( srNoRequest )
{
	m_nProbabilityMultiplicator = ( nRating < 5 ) ? nRating : 5;
}

/**
  * Custom copy constructor. Note that the registered pointers are NOT copied
  * when duplicating a CDiscoveryService.
  * Locking: /
  */
CDiscoveryService::CDiscoveryService(const CDiscoveryService& pService) :
	m_bQueued( false )
{
	// The usage of a custom copy constructor makes sure the list of registered
	// pointers is NOT forwarded to a copy of this rule.

	m_nServiceType	= pService.m_nServiceType;
	m_oNetworkType  = pService.m_oNetworkType;
	m_oServiceURL	= pService.m_oServiceURL;
	m_nRating       = pService.m_nRating;
	m_oUUID         = pService.m_oUUID;
	m_nLastHosts	= pService.m_nLastHosts;
	m_nTotalHosts	= pService.m_nTotalHosts;

	m_nProbabilityMultiplicator	= pService.m_nProbabilityMultiplicator;
}

/**
  * Destructor.
  * Locking: /
  */
CDiscoveryService::~CDiscoveryService()
{
	// Set all pointers to this rule to NULL to notify them about the deletion of this object.
	for ( std::list<CDiscoveryService**>::iterator i = m_lPointers.begin();
		  i != m_lPointers.end(); ++i )
	{
		*(*i) = NULL;
	}
}

/**
  * Operator ==
  * Locking: /
  */
bool CDiscoveryService::operator==(const CDiscoveryService& pService) const
{
	return ( m_nServiceType	== pService.m_nServiceType	&&
			 m_oNetworkType	== pService.m_oNetworkType	&&
			 m_oServiceURL	== pService.m_oServiceURL	&&
			 m_nRating		== pService.m_nRating		&&
			 m_oUUID		== pService.m_oUUID			&&
			 m_nLastHosts	== pService.m_nLastHosts	&&
			 m_nTotalHosts	== pService.m_nTotalHosts );
}

/**
  * Operator !=
  * Locking: /
  */
bool CDiscoveryService::operator!=(const CDiscoveryService& pService) const
{
	return !( *this == pService );
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

	oStream >> nServiceType;
	oStream >> nNetworkType;

	pService->m_nServiceType = (ServiceType)nServiceType;
	pService->m_oNetworkType = CNetworkType( nNetworkType );

	oStream >> nRating;

	pService->m_nRating = nRating;
	pService->m_nProbabilityMultiplicator = ( nRating < 5 ) ? nRating : 5;

	oStream >> pService->m_oServiceURL;
	oStream >> pService->m_nLastHosts;
	oStream >> pService->m_nTotalHosts;
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
	oStream << pService->m_oServiceURL;
	oStream << pService->m_nLastHosts;
	oStream << pService->m_nTotalHosts;
}

/**
  * [static] Creates a new service...
  * Locking: /
  */
CDiscoveryService* CDiscoveryService::createService(const QUrl& oURL, const ServiceType nSType,
													const CNetworkType& oNType,	const quint8 nRating,
													const QUuid& oID)
{
	CDiscoveryService* pService = NULL;

	switch ( nSType )
	{
	case stGWC:
	{
		pService = new CGWC( oURL, nSType, oNType, nRating, oID );
		break;
	}
	}

	return pService;
}

/**
  * Registers a pointer.
  * Locking: RW
  */
void CDiscoveryService::registerPointer(CDiscoveryService** pRule)
{
	m_pRWLock.lockForWrite();
	m_lPointers.push_back( pRule );
	m_pRWLock.unlock();
}

/**
  * Unregisters a pointer.
  * Locking: RW
  */
void CDiscoveryService::unRegisterPointer(CDiscoveryService** pRule)
{
	m_pRWLock.lockForWrite();
	m_lPointers.remove( pRule );
	m_pRWLock.unlock();
}

/**
  * Updates the service with the own IP if the service supports it.
  * Locking: RW
  */
void CDiscoveryService::update(CEndPoint& oOwnIP, bool bExecute)
{
	m_pRWLock.lockForWrite();
	m_nRequest = srUpdate;
	m_oOwnIP = oOwnIP;
	m_pRWLock.unlock();

	if ( bExecute )
		start(); // Start new thread.
	else
		m_bQueued = true;
}

/**
  * Queries the service to recieve network hosts for initial connection and/or
  * alternative service URLs.
  * Locking: RW
  */
void CDiscoveryService::query(bool bExecute)
{
	m_pRWLock.lockForWrite();
	m_nRequest = srQuery;
	m_pRWLock.unlock();

	if ( bExecute )
		start(); // Start new thread.
	else
		m_bQueued = true;
}

/**
  * Executes queued command.
  * Locking: /
  */
void CDiscoveryService::execute()
{
	m_bQueued = false;
	start();
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
