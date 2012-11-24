#include "discoveryservice.h"
#include "gwc.h"

// TODO: Implement request abort time
#include "timedsignalqueue.h"

using namespace Discovery;

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
CDiscoveryService::CDiscoveryService(const QUrl& oURL, const CNetworkType& oNType, quint8 nRating) :
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


void CDiscoveryService::load(CDiscoveryService*& pService, QDataStream &fsFile, int)
{
	if ( pService )
	{
		delete pService;
		pService = NULL;
	}

	quint8		nServiceType;
	quint16		nNetworkType;
	quint8		nRating;
	QString		sURL;
	quint32		nLastHosts;
	quint32		nTotalHosts;
	quint32		tLastQueried;

	fsFile >> nServiceType;
	fsFile >> nNetworkType;
	fsFile >> nRating;
	fsFile >> sURL;
	fsFile >> nLastHosts;
	fsFile >> nTotalHosts;
	fsFile >> tLastQueried;

	pService = createService( sURL, (TServiceType)nServiceType,
							  CNetworkType( nNetworkType ), nRating );

	QWriteLocker l( &pService->m_oRWLock );

	pService->m_nLastHosts   = nLastHosts;
	pService->m_nTotalHosts  = nTotalHosts;
	pService->m_tLastQueried = tLastQueried;
}

/**
  * [static] Writes pService to given QDataStream.
  * Locking: R
  */
void CDiscoveryService::save(const CDiscoveryService* const pService, QDataStream &fsFile)
{
	QReadLocker l( &( const_cast<CDiscoveryService*>(pService) )->m_oRWLock );

	fsFile << (quint8)(pService->m_nServiceType);
	fsFile << (quint16)(pService->m_oNetworkType.toQuint16());
	fsFile << (quint8)(pService->m_nRating);
	fsFile << pService->m_oServiceURL.toString();
	fsFile << pService->m_nLastHosts;
	fsFile << pService->m_nTotalHosts;
	fsFile << pService->m_tLastQueried;
}

/**
  * [static] Creates a new service...
  * Locking: /
  */
CDiscoveryService* CDiscoveryService::createService(const QString& sURL, TServiceType eSType,
													const CNetworkType& oNType,	quint8 nRating )
{
	CDiscoveryService* pService = nullptr;

	switch ( eSType )
	{
	case stNull:
		break;
	case stGWC:
	{
		pService = new CGWC( sURL, oNType, nRating );
		break;
	}
	case stMulti:
		break;
	}

	return pService;
}

// todo: store IP in disc. services manager or get it only when needed from network
void CDiscoveryService::update()
{
	m_oRWLock.lockForWrite();

	Q_ASSERT( !m_bRunning );

	m_bRunning = true;

	doUpdate();

	m_oRWLock.unlock();
}

/**
  *
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

void CDiscoveryService::cancelRequest()
{
	// TODO: Implement.
}

/**
  *
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
