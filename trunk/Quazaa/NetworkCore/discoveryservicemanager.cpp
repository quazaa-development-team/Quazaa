#include <QFile>

#include "discoveryservicemanager.h"
#include "quazaasettings.h"

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
	qRegisterMetaType< QueryResult >("QueryResult");
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

	QWriteLocker tmp( &m_pRWLock ); // temporary switch to write lock
	m_bSaved = true;
	tmp.unlock();

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
bool CDiscoveryServiceManager::add(CDiscoveryService* pService)
{
	return pService != NULL;
}

/**
  * Removes a given service.
  * Locking: RW
  */
bool CDiscoveryServiceManager::remove(CDiscoveryService* pService)
{
	return pService != NULL;
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
  * Locking:
  */
void CDiscoveryServiceManager::updateService(CDiscoveryService::NetworkType /*type*/)
{

}

/**
  * Queries a service for a given NetworkType (async).
  * Locking:
  */
void CDiscoveryServiceManager::queryService( CDiscoveryService::NetworkType /*type*/ )
{

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

