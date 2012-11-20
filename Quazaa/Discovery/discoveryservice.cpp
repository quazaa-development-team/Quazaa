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
	return ( m_nNetworks & dpGnutella );
}

void CNetworkType::setGnutella( bool )
{
	m_nNetworks |= dpGnutella;
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
	if ( !m_nNetworks || ( m_nNetworks && !(m_nNetworks & (m_nNetworks - 1)) ) )
		return false;	// m_nNetworks is 0 or power of 2
	else				// m_nNetworks is not 0 or a power of 2 - meaning there are at
		return true;	// least 2 different bits set to 1 within the 16 bit uint
}

quint16 CNetworkType::toQuint16() const
{
	return m_nNetworks;
}

///**
//  * Empty constructor.
//  * Locking: /
//  */
//CDiscoveryService::CDiscoveryService() :
//    m_nServiceType( stNull ),
//    m_oNetworkType(),
//    m_oServiceURL(),
//    m_nRating( 0 ),
//    m_nProbabilityMultiplicator( 0 ),
//    m_oUUID(),
//    m_bQueued( false ),
//    m_nLastHosts( 0 ),
//    m_nTotalHosts( 0 ),
//    m_tLastQueried( 0 ),
//    m_nRequest( srNoRequest )
//{
//}

/**
  * Default constructor.
  * Locking: /
  */
CDiscoveryService::CDiscoveryService(const QUrl& oURL, const CNetworkType& oNType, const quint8 nRating) :
	m_nServiceType( stNull ),
	m_oNetworkType( oNType ),
	m_oServiceURL( oURL ),
	m_nRating( nRating ),
	m_nProbabilityMultiplicator( ( nRating < 5 ) ? nRating : 5 ),
	m_nLastHosts( 0 ),
	m_nTotalHosts( 0 ),
	m_tLastQueried( 0 ),
	m_bRunning( false )
{
}

/**
  * Custom copy constructor. Note that the registered pointers are NOT copied
  * when duplicating a CDiscoveryService.
  * Locking: /
  */
CDiscoveryService::CDiscoveryService(const CDiscoveryService& pService) :
	m_bRunning( false )
{
	// The usage of a custom copy constructor makes sure the list of registered
	// pointers is NOT forwarded to a copy of this rule.

	m_nServiceType	= pService.m_nServiceType;
	m_oNetworkType	= pService.m_oNetworkType;
	m_oServiceURL	= pService.m_oServiceURL;
	m_nRating		= pService.m_nRating;
//	m_oUUID			= pService.m_oUUID;
	m_nLastHosts	= pService.m_nLastHosts;
	m_nTotalHosts	= pService.m_nTotalHosts;
	m_tLastQueried	= pService.m_tLastQueried;
	m_nProbabilityMultiplicator	= pService.m_nProbabilityMultiplicator;
}

/**
  * Destructor.
  * Locking: /
  */
CDiscoveryService::~CDiscoveryService()
{
	// Set all pointers to this rule to NULL to notify them about the deletion of this object.
	for ( std::list<const CDiscoveryService**>::iterator i = m_lPointers.begin();
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
			 m_oServiceURL	== pService.m_oServiceURL	);
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

	quint8		nServiceType;
	quint16		nNetworkType;
	quint8		nRating;
	QUrl		oServiceURL;
	quint32		nLastHosts;
	quint32		nTotalHosts;
	quint32		tLastQueried;

	oStream >> nServiceType;
	oStream >> nNetworkType;
	oStream >> nRating;
	oStream >> oServiceURL;
	oStream >> nLastHosts;
	oStream >> nTotalHosts;
	oStream >> tLastQueried;

	pService = createService(oServiceURL,
							 (ServiceType)nServiceType,
							 CNetworkType( nNetworkType ),
							 nRating);

	QWriteLocker l( &pService->m_oRWLock );

	pService->m_nLastHosts   = nLastHosts;
	pService->m_nTotalHosts  = nTotalHosts;
	pService->m_tLastQueried = tLastQueried;
}

/**
  * [static] Writes pService to given QDataStream.
  * Locking: R
  */
void CDiscoveryService::save(const CDiscoveryService* const pService, QDataStream &oStream)
{
	QReadLocker l( &( const_cast<CDiscoveryService*>(pService) )->m_oRWLock );

	oStream << (quint8)(pService->m_nServiceType);
	oStream << (quint16)(pService->m_oNetworkType.toQuint16());
	oStream << (quint8)(pService->m_nRating);
	oStream << pService->m_oServiceURL;
	oStream << pService->m_nLastHosts;
	oStream << pService->m_nTotalHosts;
	oStream << pService->m_tLastQueried;
}

/**
  * [static] Creates a new service...
  * Locking: /
  */
CDiscoveryService* CDiscoveryService::createService(const QUrl& oURL, const ServiceType nSType,
													const CNetworkType& oNType,	const quint8 nRating)
{
	CDiscoveryService* pService = NULL;

	switch ( nSType )
	{
	case stNull:
		break;
	case stGWC:
	{
		pService = new CGWC( oURL, oNType, nRating/*, oID*/ );
		break;
	}
	case stMulti:
		break;
	}

	return pService;
}

/**
  * Registers a pointer. Note that this method is const, because it does not change the
  * data defining the DiscoveryService in question. It does, however, add the pointer pService
  * to an internal storage structure. So be careful when using.
  * Locking: RW
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
  * Unregisters a pointer. Note that this method is const, because it does not change the
  * data defining the DiscoveryService in question. It does, however, add the pointer pService
  * to an internal storage structure. So be careful when using.
  * Locking: RW
  */
void CDiscoveryService::unRegisterPointer(const CDiscoveryService** pService) const
{
	// Children, don't repeat this at home! :D
	CDiscoveryService* pModifiable = const_cast<CDiscoveryService*>(this);

	pModifiable->m_oRWLock.lockForWrite();
	pModifiable->m_lPointers.remove( pService );
	pModifiable->m_oRWLock.unlock();
}

/**
  * Updates the service with the own IP if the service supports it.
  * Locking: RW
  */
void CDiscoveryService::update(CEndPoint& oOwnIP)
{
	m_oRWLock.lockForWrite();

	Q_ASSERT( !m_bRunning );

	m_bRunning = true;
	m_oOwnIP = oOwnIP;

	doUpdate();

	m_oRWLock.unlock();
}

/**
  * Queries the service to recieve network hosts for initial connection and/or
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
}
