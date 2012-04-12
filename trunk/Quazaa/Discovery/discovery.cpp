#include <QFile>
#include <QUrl>

#include "endpoint.h"
#include "discovery.h"
#include "discoveryservice.h"
#include "network.h"
#include "quazaasettings.h"

#include <QDateTime>

Discovery::CDiscovery discoveryManager;

using namespace Discovery;

/**
  * Default constructor.
  * Locking: /
  */
CDiscovery::CDiscovery(QObject *parent) :
	QObject( parent ),
	m_bSaved( true ),
	m_bIsRunning( true )
{
}

/**
  * Initializes the Discovery Services Manager.
  * Locking: RW
  */
bool CDiscovery::start()
{
	// Register QSharedPointer< CDiscoveryService > to allow using this type
	// with queued signal/slot connections.
	qRegisterMetaType< QSharedPointer< CDiscoveryService > >("QSharedPointer<CSecureRule>");

	// Initialize random number generator.
	qsrand ( QDateTime::currentDateTime().toTime_t() );

	// Load rules from disk.
	return load();
}

/**
  * Prepares the Discovery Services Manager for destruction.
  * Locking: RW
  */
bool CDiscovery::stop()
{
	bool bSaved = save( true );
	clear();

	return bSaved;
}

/**
  * Loads the discovery services from HDD.
  * Locking: RW
  */
bool CDiscovery::load()
{
	QString sPath = quazaaSettings.Discovery.DataPath + "discovery.dat";

	QWriteLocker l( &m_pRWLock );

	if ( load( sPath ) )
	{
		return true;
	}
	else // Unable to load default file. Switch to backup one instead.
	{
		sPath = quazaaSettings.Discovery.DataPath + "discovery_backup.dat";
		return load( sPath );
	}
}

/**
  * Saves the discovery services to HDD.
  * Locking: R (+RW - very short)
  */
bool CDiscovery::save(bool bForceSaving)
{
	QReadLocker mutex( &m_pRWLock );

	if ( m_bSaved && !bForceSaving )
	{
		return true; // Saving not required ATM.
	}

	QString sPath		= quazaaSettings.Discovery.DataPath + "discovery.dat";
	QString sBackupPath = quazaaSettings.Discovery.DataPath + "discovery_backup.dat";

	if ( QFile::exists( sBackupPath ) && !QFile::remove( sBackupPath ) )
		return false; // Error while removing old backup file.

	if ( QFile::exists( sPath ) && !QFile::rename( sPath, sBackupPath ) )
		return false; // Error while renaming current database file to replace backup file.

	QFile oFile( sPath );

	if ( !oFile.open( QIODevice::WriteOnly ) )
		return false;

	unsigned short nVersion = DISCOVERY_CODE_VERSION;

	try
	{
		QDataStream oStream( &oFile );

		oStream << nVersion;
		oStream << getCount();

		// write services to stream
		for ( CIterator i = m_mServices.begin(); i != m_mServices.end(); ++i )
		{
			const CDiscoveryService* pService = (*i).second;
			CDiscoveryService::save( pService, oStream );
		}
	}
	catch ( ... )
	{
		return false;
	}

	mutex.unlock();
	QWriteLocker writeLock( &m_pRWLock ); // temporary switch to write lock

	m_bSaved = true;

	writeLock.unlock();

	oFile.close();

	// We don't really care whether this fails or not, as saving to the primary file was successfull.
	QFile::remove( sBackupPath );
	QFile::copy( sPath, sBackupPath );

	return true;
}

/**
  * Adds a given service.
  * Locking: RW
  */
QUuid CDiscovery::add(const QString& sURL, const CDiscoveryService::ServiceType nSType,
									const CNetworkType& oNType, const quint8 nRating)
{
	QUrl oURL( sURL );

	if ( !oURL.isValid() ) // Creating a service without a valid URL is nonsense.
		return QUuid();

	CDiscoveryService* pService = CDiscoveryService::createService( oURL, nSType, oNType, nRating );

	QWriteLocker l( &m_pRWLock );

	if ( add( pService ) )
	{
		// inform GUI about new service
		emit serviceAdded( pService );

		return pService->m_oUUID;
	}
	else
		return QUuid();
}

/**
  * Removes a given service.
  * Locking: RW
  */
bool CDiscovery::remove(const QUuid& oServiceID)
{
	if ( oServiceID.isNull() )
		return false;

	QWriteLocker l( &m_pRWLock );

	try
	{
		// std::map::at() throws std::out_of_range if oServiceID cannot be found in the map
		CDiscoveryService* pService = m_mServices.at( oServiceID );

		if ( pService->isRunning() ) // Do not remove a service that is currently active.
			return false;

		m_lAsyncTODO.remove( pService ); // Remove service from TODO list if present.

		// inform GUI about service removal
		emit serviceRemoved( QSharedPointer<CDiscoveryService>( pService ) );

		m_mServices.erase( oServiceID );
		return true;
	}
	catch ( std::out_of_range )
	{
		return false;
	}
}

/**
  * Removes all services.
  * Locking: RW
  */
void CDiscovery::clear(bool bInformGUI)
{
	QWriteLocker l( &m_pRWLock );

	if ( bInformGUI )
	{
		foreach ( CMapPair pair, m_mServices )
		{
			// QSharedPointer makes sure the services are deleted, once the GUI has been informed.
			emit serviceRemoved( QSharedPointer<CDiscoveryService>( pair.second ) );

			m_mServices.erase( pair.first );
		}
	}
	else
	{
		foreach ( CMapPair pair, m_mServices )
		{
			delete pair.second;
		}

		m_mServices.clear();
	}
}

/**
  * Checks whether a service is already part of the manager.
  * Locking: R
  */
bool CDiscovery::check(const CDiscoveryService* const pService)
{
	QReadLocker l( &m_pRWLock );

	try
	{
		// std::map::at() throws std::out_of_range if pService->m_oUUID cannot be found in the map
		const CDiscoveryService* const pExService = m_mServices.at( pService->m_oUUID );

		if ( *pExService == *pService )
			return true;
		else
			return false;
	}
	catch ( std::out_of_range )
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// Qt slots
/**
  * Qt slot. Triggers the Discovery Service Manager to emit all rules using the ruleInfo() signal.
  * Locking: R
  */
void CDiscovery::requestRuleList()
{
	QReadLocker l( &m_pRWLock );
	for ( CIterator i = m_mServices.begin() ; i != m_mServices.end(); i++ )
	{
		emit serviceInfo( (*i).second );
	}
}

/**
  * Publishes the own IP (async).
  * Locking: RW
  */
bool CDiscovery::updateService(CDiscoveryService::ServiceType type)
{
	QWriteLocker lock( &m_pRWLock );

	return updateHelper( getRandomService( type ), lock );
}

/**
  * Queries a service for a given NetworkType (async).
  * Locking: RW
  */
bool CDiscovery::queryService(const CNetworkType& type)
{
	QWriteLocker lock( &m_pRWLock );

	return queryHelper( getRandomService( type ), lock );
}

/**
  * Publishes the own IP (async).
  * Locking: RW
  */
bool CDiscovery::updateService(const QUuid& oServiceID)
{
	QWriteLocker lock( &m_pRWLock );

	try
	{
		// std::map::at() throws std::out_of_range if oServiceID cannot be found in the map
		return updateHelper( m_mServices.at( oServiceID ), lock );
	}
	catch ( std::out_of_range )
	{
		return false;
	}
}

/**
  * Queries a service for a given NetworkType (async).
  * Locking: RW
  */
bool CDiscovery::queryService(const QUuid& oServiceID)
{
	QWriteLocker lock( &m_pRWLock );

	try
	{
		// std::map::at() throws std::out_of_range if oServiceID cannot be found in the map
		return queryHelper( m_mServices.at( oServiceID ), lock );
	}
	catch ( std::out_of_range )
	{
		return false;
	}
}

/**
  * Slot to be triggered once service action has been finished.
  * Locking: RW
  */
void CDiscovery::serviceActionFinished()
{
	m_pRWLock.lockForWrite();

	disconnect( this, SLOT(serviceActionFinished()) );

	if ( !m_lAsyncTODO.size() )
	{
		m_bIsRunning = false;
	}
	else
	{
		CDiscoveryService* pService = m_lAsyncTODO.front();
		m_lAsyncTODO.pop_front();

		connect( pService, SIGNAL(finished()), SLOT(serviceActionFinished()), Qt::QueuedConnection );
		pService->execute();
	}

	m_pRWLock.unlock();
}

/**
  * Private helper method for load()
  * Requires Locking: RW
  */
bool CDiscovery::load( QString sPath )
{
	QFile oFile( sPath );

	if ( ! oFile.open( QIODevice::ReadOnly ) )
		return false;

	CDiscoveryService* pService = NULL;

	try
	{
		clear();

		QDataStream iStream( &oFile );

		unsigned short nVersion;
		iStream >> nVersion;

		unsigned int nCount;
		iStream >> nCount;

		while ( nCount > 0 )
		{
			CDiscoveryService::load( pService, iStream, nVersion );

			add( pService );
			pService = NULL;
			--nCount;
		}
	}
	catch ( ... )
	{
		if ( pService )
			delete pService;

		clear();
		oFile.close();

		return false;
	}
	oFile.close();

	return true;
}

/**
  * Private helper method to add a discovery service.
  * Requires Locking: RW
  */
bool CDiscovery::add(CDiscoveryService* pService)
{
	if ( !pService )
		return false;

	normalizeURL( pService->m_oServiceURL );

	for ( CDiscoveryServicesMap::iterator i = m_mServices.begin(); i != m_mServices.end(); ++i )
	{

		// TODO: Improve this test.

		if ( (*i).second->m_oServiceURL == pService->m_oServiceURL )
		{
			delete pService;
			pService = NULL;
			return false;
		}
	}

	m_mServices[pService->m_oUUID] = pService;
	return true;
}

/**
  * Used to normalize URLs to avoid adding multiple copies of the same service
  * Locking: /
  */
void CDiscovery::normalizeURL(QUrl& /*oURL*/)
{

	// TODO: Implement.

}

/**
  * Private helper for updateService()
  * Locking required: RW
  */
bool CDiscovery::updateHelper(CDiscoveryService *pService, QWriteLocker &lock)
{
	if ( pService )
	{
		CEndPoint oOwnIP = Network.GetLocalAddress();

		if ( !isRunning() )
		{
			connect( pService, SIGNAL(finished()), SLOT(serviceActionFinished()), Qt::QueuedConnection );
			m_bIsRunning = true;

			lock.unlock();

			pService->update( oOwnIP );
			return true;
		}
		else // queue the service for delayed updating
		{
			m_lAsyncTODO.push_back( pService );

			lock.unlock();

			pService->update( oOwnIP, false );
			return true;
		}
	}
	else
	{
		return false;
	}
}

/**
  * Private helper for queryeService()
  * Locking required: RW
  */
bool CDiscovery::queryHelper(CDiscoveryService *pService, QWriteLocker &lock)
{
	if ( pService )
	{
		if ( !isRunning() )
		{
			connect( pService, SIGNAL(finished()), SLOT(serviceActionFinished()), Qt::QueuedConnection );
			m_bIsRunning = true;

			lock.unlock();

			pService->query();
			return true;
		}
		else // queue the service for delayed querying
		{
			m_lAsyncTODO.push_back( pService );

			lock.unlock();

			pService->query( false );
			return true;
		}
	}
	else
	{
		return false;
	}
}

/**
  * Returns a (pseudo) random service of a given ServiceType.
  * Requires Locking: R
  */
CDiscoveryService* CDiscovery::getRandomService(CDiscoveryService::ServiceType nSType)
{
	CDiscoveryServicesList list;
	quint16 nTotalRating = 0;
	CDiscoveryService* pService;

	foreach ( CMapPair pair, m_mServices )
	{
		pService = pair.second;

		if ( pService->m_nServiceType == nSType && pService->m_nRating && !pService->isQueued() )
		{
			list.push_back( pService );
			nTotalRating += pService->m_nRating;
		}
	}

	if ( list.empty() )
	{
		return NULL;
	}
	else
	{
		quint16 nSelectedRating = qrand() % nTotalRating;
		CDiscoveryServicesList::const_iterator current = list.begin();
		CDiscoveryService* pSelected = *current;

		while ( nSelectedRating > pSelected->m_nRating )
		{
			nSelectedRating -= pSelected->m_nRating;
			pSelected = *( ++current );
		}

		return pSelected;
	}
}

/**
  * Returns a (pseudo) random service of a given NetworkType.
  * Requires Locking: R
  */
CDiscoveryService* CDiscovery::getRandomService(const CNetworkType& oNType)
{
	CDiscoveryServicesList list;
	quint16 nTotalRating = 0;
	CDiscoveryService* pService;

	foreach ( CMapPair pair, m_mServices )
	{
		pService = pair.second;

		if ( pService->m_oNetworkType.isNetwork( oNType ) && pService->m_nRating && !pService->isQueued() )
		{
			list.push_back( pService );
			nTotalRating += pService->m_nRating;
		}
	}

	if ( list.empty() )
	{
		return NULL;
	}
	else
	{
		quint16 nSelectedRating = qrand() % nTotalRating;
		CDiscoveryServicesList::const_iterator current = list.begin();
		CDiscoveryService* pSelected = *current;

		while ( nSelectedRating > pSelected->m_nRating )
		{
			nSelectedRating -= pSelected->m_nRating;
			pSelected = *( ++current );
		}

		return pSelected;
	}
}
