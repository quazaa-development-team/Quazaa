/*
** discovery.cpp
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

#include <QFile>
#include <QDateTime>

#include "discovery.h"
#include "discoveryservice.h"

#include "quazaasettings.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

Discovery::CDiscovery discoveryManager;

using namespace Discovery;


/**
 * @brief CDiscovery: Constructs a new discovery services manager.
 * @param parent
 */
CDiscovery::CDiscovery(QObject *parent) :
	QObject( parent ),
	m_bSaved( true ),
	m_nLastID( 0 )

{
}

/**
 * @brief ~CDiscovery: Destructor. Make sure you have stopped the magager befor destroying it.
 */
CDiscovery::~CDiscovery()
{
	Q_ASSERT( m_mServices.empty() );
}

/**
 * @brief count allows you to access the number of working services for a given network.
 * Locking: YES
 * @return the number of services for the specified network. If no type is specified or the type is
 * null, the total number of all services is returned, no matter whether they are working or not.
 */
quint32	CDiscovery::count(const CNetworkType& oType)
{
	QMutexLocker l( &m_pSection );

	if ( oType.isNull() )
	{
		return m_mServices.size();
	}
	else
	{
		quint16 nCount = 0;
		CDiscoveryService* pService;

		foreach ( TMapPair pair, m_mServices )
		{
			pService = pair.second;
			pService->m_oRWLock.lockForRead();

			// Count all services that have the correct type, are not blocked and have a rating > 0
			if ( pService->m_oNetworkType.isNetwork( oType ) && !pService->m_bBanned && pService->m_nRating )
			{
				++nCount;
			}
			pService->m_oRWLock.unlock();
		}

		return nCount;
	}
}

/**
 * @brief start initializes the Discovery Services Manager.
 * Locking: YES
 * @return whether loading the services was successful.
 */
bool CDiscovery::start()
{
	// Initialize random number generator.
	qsrand ( QDateTime::currentDateTime().toTime_t() );

	// Load rules from disk.
	QMutexLocker l( &m_pSection );

	m_sMessage = tr( "[Discovery] " );
	return load();
}

/**
 * @brief stop prepares the Discovery Services Manager for destruction.
 * Locking: YES
 * @return true if the services have been successfully written to disk.
 */
bool CDiscovery::stop()
{
	bool bSaved = save( true );
	clear();

	return bSaved;
}

/**
 * @brief save saves all discovery services to disk, if there have been important modifications to at
 * least one service or bForceSaving is set to true.
 * Locking: YES
 * @param bForceSaving: Set this to true to force saving even if there have been no important service
 * modifications, for example to make sure the hosts from the current session are saved properly.
 * @return true if saving to file was successful; false otherwise.
 */
bool CDiscovery::save(bool bForceSaving)
{
	QMutexLocker l( &m_pSection );

	if ( m_bSaved && !bForceSaving )
	{
		return true; // Saving not required ATM.
	}

	systemLog.postLog( LogSeverity::Notice, m_sMessage + tr( "Saving Discovery Services Manager state." ) );

	QString sPath          = quazaaSettings.Discovery.DataPath + "discovery.dat";
	QString sBackupPath    = quazaaSettings.Discovery.DataPath + "discovery_backup.dat";
	QString sTemporaryPath = sBackupPath + "_tmp";

	if ( QFile::exists( sTemporaryPath ) && !QFile::remove( sTemporaryPath ) )
	{
		systemLog.postLog( LogSeverity::Error, m_sMessage +
						   tr( "Error: Could not free space required for data backup: " ) + sPath );
		return false;
	}

	QFile oFile( sTemporaryPath );

	if ( !oFile.open( QIODevice::WriteOnly ) )
	{
		systemLog.postLog( LogSeverity::Error,
						   m_sMessage + tr( "Error: Could open data file for write: " ) + sTemporaryPath );
		return false;
	}

	quint16 nVersion = DISCOVERY_CODE_VERSION;

	try
	{
		QDataStream fsFile( &oFile );

		fsFile << nVersion;
		fsFile << count();

		// write services to stream
		foreach (  TMapPair pair, m_mServices )
		{
			CDiscoveryService::save( pair.second, fsFile );
		}
	}
	catch ( ... )
	{
		systemLog.postLog( LogSeverity::Error,
						   m_sMessage + tr( "Error while writing discovery services to disk." ) );
		return false;
	}

	m_bSaved = true;

	l.unlock();

	oFile.close();

	if ( QFile::exists( sPath ) )
	{
		if ( !QFile::remove( sPath ) )
		{
			systemLog.postLog( LogSeverity::Error,
							   m_sMessage + tr( "Error: Could not remove old data file: " ) + sPath );
			return false;
		}

		if ( !QFile::rename( sTemporaryPath, sPath ) )
		{
			systemLog.postLog( LogSeverity::Error,
							   m_sMessage + tr( "Error: Could not rename data file: " ) + sPath );
			return false;
		}
	}

	if ( QFile::exists( sBackupPath ) && !QFile::remove( sBackupPath ) )
	{
		systemLog.postLog( LogSeverity::Warning,
						   m_sMessage + tr( "Error: Could not remove old backup file: " ) + sBackupPath );
	}

	if ( !QFile::copy( sPath, sBackupPath ) )
	{
		systemLog.postLog( LogSeverity::Warning, m_sMessage +
						   tr( "Warning: Could not create create new backup file: " ) + sBackupPath );
	}

	return true;
}

/**
 * @brief add adds a new Service with a given URL to the manager.
 * Locking: YES
 * @param sURL
 * @param eSType
 * @param oNType
 * @param nRating
 * @return the service ID used to identify the service internally; 0 if the service has not been added.
 */
TDiscoveryID CDiscovery::add(QString sURL, const TServiceType eSType,
							 const CNetworkType& oNType, const quint8 nRating)
{
	// First check whether the URL can be parsed at all.
	QUrl oURL( sURL, QUrl::StrictMode );
	if ( !oURL.isValid() )
	{
		systemLog.postLog( LogSeverity::Error,
						   m_sMessage
						   + tr( "Error: Could not add invalid URL as a discovery service: " )
						   + sURL );
		return 0;
	}

	// Then, normalize the fully encoded URL
	sURL = oURL.toString();
	normalizeURL( sURL );

	CDiscoveryService* pService = CDiscoveryService::createService( sURL, eSType, oNType, nRating );

	m_pSection.lock();

	if ( add( pService ) )
	{
		// make sure to return the right ID
		TDiscoveryID nTmp = m_nLastID;
		m_pSection.unlock();

		systemLog.postLog( LogSeverity::Notice,
						   m_sMessage + tr( "Notice: New discovery service added: " ) + sURL );

		// inform GUI about new service
		emit serviceAdded( pService );
		return nTmp;
	}
	else
	{
		m_pSection.unlock();

		systemLog.postLog( LogSeverity::Error,
						   m_sMessage + tr( "Error adding service." ) );
		return 0;
	}
}

/**
 * @brief remove removes a service by ID.
 * Locking: YES
 * @param nID
 * @return true if the removal was successful (e.g. the service could be found), false otherwise.
 */
bool CDiscovery::remove(TDiscoveryID nID)
{
	if ( !nID )
	{
		systemLog.postLog( LogSeverity::Error,
						   m_sMessage + tr( "Internal error: Got request to remove invalid ID: " ) + nID );
		return false; // invalid ID
	}

	m_pSection.lock();

	TIterator iService = m_mServices.find( nID );

	if ( iService == m_mServices.end() )
	{
		m_pSection.unlock();

		systemLog.postLog( LogSeverity::Error,
						   m_sMessage + tr( "Internal error: Got request to remove invalid ID: " ) + nID );
		return false; // Unable to find service by ID
	}
	m_pSection.unlock();

	// inform GUI about service removal
	emit serviceRemoved( nID );

	m_pSection.lock();
	CDiscoveryService* pService = (*iService).second;

	systemLog.postLog( LogSeverity::Notice,
					   m_sMessage + tr( "Removing discovery service: " ) + pService->m_oServiceURL.toString() );

	// stop it
	pService->cancelRequest();

	// Make sure any outstanding read operations are finished before destruction.
	pService->m_oRWLock.lockForWrite();

	// delete it
	delete pService;
	m_mServices.erase( nID );
	m_pSection.unlock();

	return true;
}

/**
 * @brief clear removes all services from the manager.
 * Locking: YES
 * @param bInformGUI: Set this to true if the GUI shall be informed about the removal of the services.
 * The default value is false, which represents the scenario on shutdown, where the GUI will be removed
 * anyway shortly.
 */
void CDiscovery::clear(bool bInformGUI)
{
	m_pSection.lock();

	if ( bInformGUI ) // more efficient if the if statemant is outside the foreach loop
	{
		foreach ( TMapPair pair, m_mServices )
		{
			emit serviceRemoved( pair.second->m_nID ); // inform GUI

			// Make sure any outstanding read operations are finished before destruction.
			pair.second->m_oRWLock.lockForWrite();
			delete pair.second;
		}
	}
	else
	{
		foreach ( TMapPair pair, m_mServices )
		{
			delete pair.second;
		}
	}

	m_mServices.clear();
	m_pSection.unlock();
}

/**
 * @brief check verifies whether the given service is managed by the manager.
 * Locking: YES
 * @return true if managed; false otherwise
 */
bool CDiscovery::check(const CDiscoveryService* const pService)
{
	QMutexLocker l( &m_pSection );

	TConstIterator iService = m_mServices.find( pService->m_nID );

	if ( iService == m_mServices.end() )
		return false; // Unable to find service by ID

	const CDiscoveryService* const pExService = (*iService).second;

	if ( *pExService == *pService )
		return true;
	else
		return false;
}

/**
 * @brief CDiscovery::requestNAMgr provides a shared pointer to the discovery services network access
 * manager. Note that the caller needs to hold his copy of the shared pointer until he has finished his
 * network operation to prevent the access manager from being deleted too early.
 * Locking: YES
 * @return
 */
QSharedPointer<QNetworkAccessManager> CDiscovery::requestNAMgr()
{
	QMutexLocker l( &m_pSection );

	QSharedPointer<QNetworkAccessManager> pReturnVal;
	if ( m_pNetAccessMgr )
	{
		// if the network access manager still exists (e.g. is currently used), do recycle it
		pReturnVal = m_pNetAccessMgr.toStrongRef();
	}
	else
	{
		// else create a new access manager (will be deleted if nobody is using it anymore)
		pReturnVal = QSharedPointer<QNetworkAccessManager>( new QNetworkAccessManager(),
															&QObject::deleteLater );
		m_pNetAccessMgr = pReturnVal.toWeakRef();
	}

	return pReturnVal;
}

//////////////////////////////////////////////////////////////////////
// Qt slots
/**
 * @brief requestServiceList can be used to obtain a complete list of all currently managed services.
 * Connect to the serviceInfo() signal to recieve them.
 * Locking: YES
 */
void CDiscovery::requestServiceList()
{
	m_pSection.lock();
	foreach ( TMapPair pair, m_mServices )
	{
		emit serviceInfo( pair.second );
	}
	m_pSection.unlock();
}

/**
 * @brief updateService updates a service for a given network type with our IP. Note that not all
 * service types might support or require such updates.
 * Locking: YES
 * @param type
 * @return false if no service for the requested network type could be found; true otherwise.
 */
bool CDiscovery::updateService(const CNetworkType& type)
{
	m_pSection.lock();
	CDiscoveryService* pService = getRandomService( type );
	m_pSection.unlock();

	return updateHelper( pService, type );
}

bool CDiscovery::updateService(TDiscoveryID nID)
{
	m_pSection.lock();

	TIterator iService = m_mServices.find( nID );

	if ( iService == m_mServices.end() )
	{
		m_pSection.unlock();
		return false; // Unable to find service by ID
	}

	CDiscoveryService* pService = (*iService).second;

	m_pSection.unlock();
	return updateHelper( pService, pService->m_oNetworkType );
}

/**
 * @brief queryService
 * Locking: YES
 * @param type
 * @return false if no service for the requested network type could be found; true otherwise.
 */
bool CDiscovery::queryService(const CNetworkType& type)
{
	m_pSection.lock();
	CDiscoveryService* pService = getRandomService( type );
	m_pSection.unlock();

	return queryHelper( pService, type );
}

bool CDiscovery::queryService(TDiscoveryID nID)
{
	m_pSection.lock();

	TIterator iService = m_mServices.find( nID );

	if ( iService == m_mServices.end() )
	{
		m_pSection.unlock();
		return false; // Unable to find service by ID
	}

	CDiscoveryService* pService = (*iService).second;

	m_pSection.unlock();
	return queryHelper( pService, pService->m_oNetworkType );
}

/**
 * @brief load retrieves stored services from the HDD.
 * Requires locking: YES
 * @return true if loading from file was successful; false otherwise.
 */
bool CDiscovery::load()
{
	QString sPath = quazaaSettings.Discovery.DataPath + "discovery.dat";

	if ( load( sPath ) )
	{
		systemLog.postLog( LogSeverity::Debug,
						   m_sMessage + tr( "Loading discovery services from file: " ) + sPath );
		return true;
	}
	else // Unable to load default file. Switch to backup one instead.
	{
		sPath = quazaaSettings.Discovery.DataPath + "discovery_backup.dat";

		systemLog.postLog( LogSeverity::Warning,
						   m_sMessage
						   + tr( "Failed to load discovery services from primary file. Switching to backup: " )
						   + sPath );

		if ( load( sPath ) )
		{
			return true;
		}
		else
		{
			systemLog.postLog( LogSeverity::Error,
							   m_sMessage + tr( "Failed to load discovery services!" ) );
			return false;
		}
	}
}

bool CDiscovery::load( QString sPath )
{
	QFile oFile( sPath );

	if ( ! oFile.open( QIODevice::ReadOnly ) )
		return false;

	CDiscoveryService* pService = nullptr;

	try
	{
		clear();

		QDataStream fsFile( &oFile );

		quint16 nVersion;
		quint32 nCount;

		fsFile >> nVersion;
		fsFile >> nCount;

		QMutexLocker l( &m_pSection );
		while ( nCount > 0 )
		{
			CDiscoveryService::load( pService, fsFile, nVersion );

			add( pService );
			pService = nullptr;
			--nCount;
		}
	}
	catch ( ... )
	{
		if ( pService )
			delete pService;

		clear();
		oFile.close();

		systemLog.postLog( LogSeverity::Error,
						   m_sMessage + tr( "Error: Caught an exception while loading services from file!" ) );

		return false;
	}
	oFile.close();

	return true;
}

/**
 * @brief add... obvious... Note: if a duplicate is detected, the CDiscoveryService passed to the
 * method is deleted within.
 * Requires locking: YES
 * @param pService
 * @return true if the service was added; false if not (e.g. duplicate was detected).
 */
bool CDiscovery::add(CDiscoveryService* pService)
{
	if ( !pService )
		return false;

	if ( manageDuplicates( pService ) )
	{
		return false;
	}

	if ( pService->m_nID && m_mServices.find( m_nLastID ) != m_mServices.end() )
	{
		// We need to assign a new ID, as the previous ID of this service is already in use.
		// This should not happen as the previous services should be loaded into the manager before
		// any new services that might be added during the session.
		Q_ASSERT( false );

		pService->m_nID = 0; // set ID to invalid.
	}

	if ( !pService->m_nID )
	{
		TConstIterator i;
		do
		{
			// Check the next ID until a free one is found.
			i = m_mServices.find( ++m_nLastID );
		}
		while ( i == m_mServices.end() );

		// assign ID to service
		pService->m_nID = m_nLastID;
	}

	// push to map
	m_mServices[pService->m_nID] = pService;
	return true;
}

/**
 * @brief manageDuplicates checks if an identical (or very similar) service is alreads present in the
 * manager, decides which service to remove and frees unnecessary data.
 * Requires locking: YES
 * @param pService
 * @return true if a duplicate was detected. pService is deleted and set to nullptr in that case. false
 * otherwise.
 */
bool CDiscovery::manageDuplicates(CDiscoveryService*& pService)
{
	QString sURL = pService->m_oServiceURL.toString();

	foreach ( TMapPair pair, m_mServices )
	{
		// already existing service
		CDiscoveryService* pExService = pair.second;
		QString sExURL = pExService->m_oServiceURL.toString();

		// check for services with the same URL; Note: all URLs should be case insesitive due to
		// normalizeURLs(), so case sensitivity is not a problem here.
		if ( !sExURL.compare( sURL ) )
		{
			if ( pService->m_nServiceType == pExService->m_nServiceType )
			{
				// Make sure the existing service is set to handle the networks pService handles, too.
				// (90% of the time, this should be the case anyway, but this is more efficient than using
				// an if statement to test the condition.)
				pExService->m_oNetworkType.setNetwork( pService->m_oNetworkType );

				systemLog.postLog( LogSeverity::Debug, m_sMessage
								   + tr( "Detected a duplicate service. Not going to add the new one." ) );

				delete pService;
				pService = nullptr;
				return true;
			}
			else // This should not happen. Two services with the same URL should be of the same type.
			{
				// Generate nice assert so that this can be analized.
				QString sError = "Services of type %1 and %2 detected sharing the same URL: ";
				sError.arg( pService->type(), pExService->type() );
				sError.append( sURL );
				Q_ASSERT_X( false, "manageDuplicates", sError.toLatin1().data() );
			}
		}

		// check for services with only part of the URL of an other service
		// this filters out "www.example.com"/"www.example.com/gwc.php" pairs
		if ( sExURL.startsWith( sURL ) )
		{
// TODO: implement
		}
		else if ( sURL.startsWith( sExURL ) )
		{
// TODO: implement
		}
	}

	return false;
}

/**
 * @brief normalizeURL transforms a given URL string into a standard form to easa the detection of
 * duplicates, filter out websites caching a service etc.
 * Requires locking: NO
 * @param sURL
 */
void CDiscovery::normalizeURL(QString& sURL)
{
	sURL = sURL.toLower();

	// TODO: Implement


	/*// Check it has a valid protocol
	if ( _tcsnicmp( pszAddress, _T("http://"),  7 ) == 0 )
		pszAddress += 7;
	else if ( _tcsnicmp( pszAddress, _T("https://"), 8 ) == 0 )
		pszAddress += 8;
	else if ( _tcsnicmp( pszAddress, _T("gnutella1:host:"), 15 ) == 0 )
		return TRUE;
	else if ( _tcsnicmp( pszAddress, _T("gnutella2:host:"), 15 ) == 0 )
		return TRUE;
	else if ( _tcsnicmp( pszAddress, _T("uhc:"), 4 ) == 0 )
		return TRUE;
	else if ( _tcsnicmp( pszAddress, _T("ukhl:"), 5 ) == 0 )
		return TRUE;
	else
		return FALSE;*/
}

/**
 * @brief updateHelper: Helper Method. Performs a service update.
 * Requires locking: NO
 * @param pService
 * @return false if a NULL pointer is passed as service; true otherwise.
 */
bool CDiscovery::updateHelper(CDiscoveryService *pService, const CNetworkType &type)
{
	if ( pService )
	{
		systemLog.postLog( LogSeverity::Notice, m_sMessage + tr( "Updating service: " ) + pService->url() );

		pService->update();
		return true;
	}
	else
	{
		systemLog.postLog( LogSeverity::Warning, m_sMessage
						   + tr( "Unable to update service for network: " ) + type.toString() );

		return false;
	}
}

/**
 * @brief queryHelper: Helper method. Performs a service query.
 * Requires locking: NO
 * @param pService
 * @return false if a NULL pointer is passed as service; true otherwise.
 */
bool CDiscovery::queryHelper(CDiscoveryService *pService, const CNetworkType &type)
{
	if ( pService )
	{
		systemLog.postLog( LogSeverity::Notice, m_sMessage + tr( "Querying service: " ) + pService->url() );

		pService->query();
		return true;
	}
	else
	{
		systemLog.postLog( LogSeverity::Warning, m_sMessage
						   + tr( "Unable to query service for network: " ) + type.toString() );

		return false;
	}
}

/**
 * @brief getRandomService: Helper method. Allows to get a random service for a specified network.
 * Requires locking: YES
 * @param oNType
 * @return A discovery service for the specified network; NULL if no working service could be found for
 * the specified network.
 */
CDiscoveryService* CDiscovery::getRandomService(const CNetworkType& oNType)
{
	TDiscoveryServicesList list;
	quint16 nTotalRating = 0;		// Used to store accumulative rating of all services
									// taken under consideration as return value.
	CDiscoveryService* pService;

	foreach ( TMapPair pair, m_mServices )
	{
		pService = pair.second;

		// Consider all services that have the correct type, have a rating > 0 and are not in use.
		if ( pService->m_oNetworkType.isNetwork( oNType ) && pService->m_nRating && !pService->m_bRunning )
		{
			list.push_back( pService );
			nTotalRating += pService->m_nRating;
		}
	}

	if ( list.empty() )
	{
		return nullptr;
	}
	else
	{
		// Make sure our selection is within [1 ; nTotalRating]
		quint16 nSelectedRating = ( qrand() % nTotalRating ) + 1;
		TDiscoveryServicesList::const_iterator current = list.begin();
		CDiscoveryService* pSelected = *current;

		// Iterate threw list until the selected service has been found.
		while ( nSelectedRating > pSelected->m_nRating )
		{
			nSelectedRating -= pSelected->m_nRating;
			pSelected = *( ++current );
		}

		return pSelected;
	}
}
