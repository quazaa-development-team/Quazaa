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

#include "endpoint.h"
#include "network.h"

// TODO: Implement request abort time
#include "quazaasettings.h"
#include "timedsignalqueue.h"

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
 * @return the number of services for the specified network. If no type is specified or the type is
 * null, the total number of all services is returned, no matter whether they are working or not.
 * Locking: YES
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
 * @return whether loading the services was successful.
 * Locking: YES
 */
bool CDiscovery::start()
{
	// Initialize random number generator.
	qsrand ( QDateTime::currentDateTime().toTime_t() );

	// Load rules from disk.
	QMutexLocker l( &m_pSection );
	return load();
}

/**
 * @brief stop prepares the Discovery Services Manager for destruction.
 * @return true if the services have been successfully written to disk.
 * Locking: YES
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
 * @param bForceSaving: Set this to true to force saving even if there have been no important service
 * modifications, for example to make sure the hosts from the current session are saved properly.
 * @return true if saving to file was successful; false otherwise.
 * Locking: YES
 */
bool CDiscovery::save(bool bForceSaving)
{
	QMutexLocker l( &m_pSection );

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

// TODO: Follow naming convention for files and streams.

	QFile oFile( sPath );

	if ( !oFile.open( QIODevice::WriteOnly ) )
		return false;

	quint16 nVersion = DISCOVERY_CODE_VERSION;

	try
	{
		QDataStream oStream( &oFile );

		oStream << nVersion;
		oStream << count();

		// write services to stream
		foreach (  TMapPair pair, m_mServices )
		{
			CDiscoveryService::save( pair.second, oStream );
		}
	}
	catch ( ... )
	{
		return false;
	}

	m_bSaved = true;

	l.unlock();

	oFile.close();

	// We don't really care whether this fails or not, as saving to the primary file was successfull.
	QFile::remove( sBackupPath );
	QFile::copy( sPath, sBackupPath );

	return true;
}

/**
 * @brief add adds a new Service with a given URL to the manager.
 * @param sURL
 * @param eSType
 * @param oNType
 * @param nRating
 * @return the service ID used to identify the service internally; 0 if the service has not been added.
 * Locking: YES
 */
TDiscoveryID CDiscovery::add(QString sURL, const TServiceType eSType,
							 const CNetworkType& oNType, const quint8 nRating)
{
	// First check whether the URL can be parsed at all.
	QUrl oURL( sURL, QUrl::StrictMode );
	if ( !oURL.isValid() )
		return 0;

	// Then, normalize the fully encoded URL
	sURL = oURL.toEncoded();
	normalizeURL( sURL );

	CDiscoveryService* pService = CDiscoveryService::createService( sURL, eSType, oNType, nRating );

	m_pSection.lock();

	if ( add( pService ) )
	{
		m_pSection.unlock();

		// inform GUI about new service
		emit serviceAdded( pService );
		return m_nLastID;
	}
	else
	{
		m_pSection.unlock();
		return 0;
	}
}

/**
 * @brief remove removes a service by ID.
 * @param nID
 * @return true if the removal was successful (e.g. the service could be found), false otherwise.
 * Locking: YES
 */
bool CDiscovery::remove(TDiscoveryID nID)
{
	if ( !nID )
		return false; // invalid ID

	m_pSection.lock();

	TIterator iService = m_mServices.find( nID );

	if ( iService == m_mServices.end() )
	{
		m_pSection.unlock();
		return false; // Unable to find service by ID
	}
	m_pSection.unlock();

	// inform GUI about service removal
	emit serviceRemoved( nID );

	m_pSection.lock();
	CDiscoveryService* pService = (*iService).second;

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
 * @param bInformGUI: Set this to true if the GUI shall be informed about the removal of the services.
 * The default value is false, which represents the scenario on shutdown, where the GUI will be removed
 * anyway shortly.
 * Locking: YES
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
 * @return true if managed; false otherwise
 * Locking: YES
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
 * @param type
 * @return false if no service for the requested network type could be found; true otherwise.
 * Locking: YES
 */
bool CDiscovery::updateService(const CNetworkType& type)
{
	m_pSection.lock();
	CDiscoveryService* pService = getRandomService( type );
	m_pSection.unlock();

	return updateHelper( pService );
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
	return updateHelper( pService );
}

/**
 * @brief queryService
 * @param type
 * @return false if no service for the requested network type could be found; true otherwise.
 * Locking: YES
 */
bool CDiscovery::queryService(const CNetworkType& type)
{
	m_pSection.lock();
	CDiscoveryService* pService = getRandomService( type );
	m_pSection.unlock();

	return queryHelper( pService );
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
	return queryHelper( pService );
}

/**
 * @brief load retrieves stored services from the HDD.
 * @return true if loading from file was successful; false otherwise.
 * Requires locking: YES
 */
bool CDiscovery::load()
{
	QString sPath = quazaaSettings.Discovery.DataPath + "discovery.dat";

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

bool CDiscovery::load( QString sPath )
{
	QFile oFile( sPath );

	if ( ! oFile.open( QIODevice::ReadOnly ) )
		return false;

	CDiscoveryService* pService = nullptr;

	try
	{
		clear();

		QDataStream iStream( &oFile );

		quint16 nVersion;
		quint32 nCount;

		iStream >> nVersion;
		iStream >> nCount;

		while ( nCount > 0 )
		{
			CDiscoveryService::load( pService, iStream, nVersion );

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

		return false;
	}
	oFile.close();

	return true;
}

/**
 * @brief add... obvious... Note: if a duplicate is detected, the CDiscoveryService passed to the
 * method is deleted within.
 * @param pService
 * @return true if the service was added; false if not (e.g. duplicate was detected).
 * Requires locking: YES
 */
bool CDiscovery::add(CDiscoveryService* pService)
{
	if ( !pService )
		return false;

	if ( isDuplicate( pService ) )
	{
		delete pService;
		pService = nullptr;
		return false;
	}

	// assign ID to service
	pService->m_nID = ++m_nLastID;

#ifdef _DEBUG
	// Make sure to never use the same ID twice.
	TConstIterator i = m_mServices.find( m_nLastID );
	Q_ASSERT( i == m_mServices.end() );
#endif

	// push to map
	m_mServices[m_nLastID] = pService;
	return true;
}

/**
 * @brief isDuplicate checks if an identical (or very similar) service is alreads present in the manager.
 * @param pService
 * @return
 * Requires locking: YES
 */
bool CDiscovery::isDuplicate(CDiscoveryService* pService)
{
	// TODO: Implement.
	return false;
}

/**
 * @brief normalizeURL transforms a given URL string into a standard form to easa the detection of
 * duplicates, filter out websites caching a service etc.
 * @param sURL
 * Requires locking: NO
 */
void CDiscovery::normalizeURL(QString& sURL)
{
	// TODO: Implement.
}

/**
 * @brief updateHelper: Helper Method. Performs a service update.
 * @param pService
 * @return false if a NULL pointer is passed as service; true otherwise.
 * Requires locking: NO
 */
bool CDiscovery::updateHelper(CDiscoveryService *pService)
{
	if ( pService )
	{
		pService->update( Network.GetLocalAddress() );
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief queryHelper: Helper method. Performs a service query.
 * @param pService
 * @return false if a NULL pointer is passed as service; true otherwise.
 * Requires locking: NO
 */
bool CDiscovery::queryHelper(CDiscoveryService *pService)
{
	if ( pService )
	{
		pService->query();
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief getRandomService: Helper method. Allows to get a random service for a specified network.
 * @param oNType
 * @return A discovery service for the specified network; NULL if no working service could be found for
 * the specified network.
 * Requires locking: YES
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
