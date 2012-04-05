#include <QFile>
#include <QUrl>

#include "endpoint.h"
#include "discoveryservicemanager.h"
#include "discoveryservice.h"
#include "network.h"
#include "quazaasettings.h"

#include <QDateTime>

CDiscoveryServiceManager::CDiscoveryServiceManager(QObject *parent) :
	QObject( parent ),
	m_bSaved( true ),
	m_bIsRunning( true )
{
}

/**
  * Initializes the Discovery Services Manager.
  * Locking: RW
  */
bool CDiscoveryServiceManager::start()
{
	qsrand ( QDateTime::currentDateTime().toTime_t() ); // Initialize random number generator.
	return load();
}

/**
  * Prepares the Discovery Services Manager for destruction.
  * Locking: RW
  */
bool CDiscoveryServiceManager::stop()
{
	bool bSaved = save( true );
	clear();

	return bSaved;
}

/**
  * Loads the discovery services from HDD.
  * Locking: RW
  */
bool CDiscoveryServiceManager::load()
{
	QString sPath = quazaaSettings.Discovery.DataPath + "discovery.dat";

	QWriteLocker l( &m_pRWLock );

	if ( load( sPath ) )
	{
		return true;
	}
	else
	{
		sPath = quazaaSettings.Discovery.DataPath + "discovery_backup.dat";
		return load( sPath );
	}
}

/**
  * Saves the discovery services to HDD.
  * Locking: R (+RW - very short)
  */
bool CDiscoveryServiceManager::save(bool bForceSaving)
{
	QReadLocker mutex( &m_pRWLock );

	if ( m_bSaved && !bForceSaving )		// Saving not required ATM.
	{
		return true;
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

		for ( CIterator i = m_lServices.begin(); i != m_lServices.end(); ++i )
		{
			const CDiscoveryService* pService = *i;
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
QUuid CDiscoveryServiceManager::add(const QString& sURL, const CDiscoveryService::ServiceType nSType,
									const CNetworkType& oNType, const quint8 nRating)
{
	QUrl oURL( sURL );

	if ( !oURL.isValid() )
		return QUuid();

	CDiscoveryService* pService = CDiscoveryService::createService( oURL, nSType, oNType, nRating );

	QWriteLocker l( &m_pRWLock );

	if ( add( pService ) )
		return pService->m_oUUID;
	else
		return QUuid();
}

/**
  * Removes a given service.
  * Locking: RW
  */
bool CDiscoveryServiceManager::remove(const QUuid& oServiceID)
{
	if ( oServiceID.isNull() )
		return false;

	QWriteLocker l( &m_pRWLock );

	for ( CDiscoveryServicesList::iterator i = m_lServices.begin();
		  i != m_lServices.end(); ++i )
	{
		if ( (*i)->m_oUUID == oServiceID )
		{
			m_lServices.erase( i );
			return true;
		}
	}

	return false;
}

/**
  * Removes all services.
  * Locking: RW
  */
void CDiscoveryServiceManager::clear()
{
	QWriteLocker l( &m_pRWLock );

	qDeleteAll( m_lServices );
	m_lServices.clear();
}

/**
  * Publishes the own IP (async).
  * Locking: RW
  */
bool CDiscoveryServiceManager::updateService(CDiscoveryService::ServiceType type)
{
	QWriteLocker lock( &m_pRWLock );

	CDiscoveryService* pService = getRandomService( type );

	if ( pService )
	{
		CEndPoint oOwnIP = Network.GetLocalAddress();

		connect( pService, SIGNAL(finished()), SLOT(serviceActionFinished()), Qt::QueuedConnection );
		m_bIsRunning = true;
		lock.unlock();

		pService->update( oOwnIP );

		return true;
	}
	else
	{
		return false;
	}
}

/**
  * Queries a service for a given NetworkType (async).
  * Locking: RW
  */
bool CDiscoveryServiceManager::queryService( CNetworkType type )
{
	QWriteLocker lock( &m_pRWLock );

	CDiscoveryService* pService = getRandomService( type );

	if ( pService )
	{
		connect( pService, SIGNAL(finished()), SLOT(serviceActionFinished()), Qt::QueuedConnection );
		m_bIsRunning = true;

		lock.unlock();

		pService->query();

		return true;
	}
	else
	{
		return false;
	}
}

/**
  * Slot to be triggered once service action has been finished.
  * Locking: RW
  */
void CDiscoveryServiceManager::serviceActionFinished()
{
	m_pRWLock.lockForWrite();
	disconnect( this, SLOT(serviceActionFinished()) );
	m_bIsRunning = false;
	m_pRWLock.unlock();
}

/**
  * Private helper method for load()
  * Requires Locking: RW
  */
bool CDiscoveryServiceManager::load( QString sPath )
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
			nCount--;
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
bool CDiscoveryServiceManager::add(CDiscoveryService* pService)
{
	if ( !pService )
		return false;

	normalizeURL( pService->m_oServiceURL );

	for ( CDiscoveryServicesList::iterator i = m_lServices.begin();
		  i != m_lServices.end(); ++i )
	{

		// TODO: Improve this test.

		if ( (*i)->m_oServiceURL == pService->m_oServiceURL )
		{
			delete pService;
			pService = NULL;
			return false;
		}
	}

	m_lServices.push_back( pService );
	return true;
}

/**
  * Used to normalize URLs to avoid adding multiple copies of the same service
  * Locking: /
  */
void CDiscoveryServiceManager::normalizeURL(QUrl& /*oURL*/)
{

	// TODO: Implement.

}

/**
  * Returns a (pseudo) random service of a given ServiceType.
  * Requires Locking: R
  */
CDiscoveryService* CDiscoveryServiceManager::getRandomService(CDiscoveryService::ServiceType nSType)
{
	CDiscoveryServicesList list;
	quint16 nTotalRating = 0;

	foreach ( CDiscoveryService* pService, m_lServices )
	{
		if ( pService->m_nServiceType == nSType && pService->m_nRating != 0 )
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
CDiscoveryService* CDiscoveryServiceManager::getRandomService(CNetworkType oNType)
{
	CDiscoveryServicesList list;
	quint16 nTotalRating = 0;

	foreach ( CDiscoveryService* pService, m_lServices )
	{
		if ( pService->m_oNetworkType.isNetwork( oNType ) && pService->m_nRating != 0 )
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
