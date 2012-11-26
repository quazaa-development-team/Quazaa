/*
** discovery.h
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

#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QObject>
#include <QMutex>
#include <QString>

#include <map>
#include <list>

#include "networktype.h"

// Increment this if there have been made changes to the way of storing discovery services.
#define DISCOVERY_CODE_VERSION	0
// History:
// 0 - Initial implementation

namespace Discovery
{
// Note on the usage of locking: By design, CDiscovery is the only one allowed to modify the CDiscoveryService
// objects it manages. This means that everyone else needs to lock them for read when accessing them, while the
// manager does not, it only needs to lock them for write if need be.
class CDiscoveryService;

/**
 * @brief TServiceType: Must be updated when implementing new subclasses of CDiscoveryService.
 */
typedef enum { stNull = 0, stGWC = 1 } TServiceType;

/**
 * @brief TDiscoveryID: ID type used to identify and manage discovery services. All IDs are positive.
 * 0 indicates an invalid ID.
 */
typedef quint16 TDiscoveryID;

class CDiscovery : public QObject
{
	Q_OBJECT

	/* ================================================================ */
	/* ========================= Definitions  ========================= */
	/* ================================================================ */
private:
	/**
	 * @brief TDiscoveryServicesMap: Used to store and retrieve services based on their ID.
	 */
	typedef std::map < TDiscoveryID, CDiscoveryService*  > TDiscoveryServicesMap;

	/**
	 * @brief TMapPair: pair of an ID and a discovery service pointer for use in the map.
	 */
	typedef std::pair< TDiscoveryID, CDiscoveryService*  > TMapPair;

	/**
	 * @brief TDiscoveryServicesList: Used to temorarily store services.
	 */
	typedef std::list < CDiscoveryService*  > TDiscoveryServicesList;

	/**
	 * @brief TConstIterator: Constant map iterator.
	 */
	typedef TDiscoveryServicesMap::const_iterator TConstIterator;

	/**
	 * @brief TIterator: Modifiable map iterator
	 */
	typedef TDiscoveryServicesMap::iterator TIterator;

	/**
	 * @brief TListIterator: Constant list iterator
	 */
	typedef TDiscoveryServicesList::const_iterator TListIterator;

	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */
private:
	/**
	 * @brief m_pSection: Mutex for locking the Discovery services manager before using it.
	 */
	QMutex m_pSection;

	// discovery service map
	TDiscoveryServicesMap m_mServices;

	// true if current Discovery Service Manager state has already been saved to file, false otherwise.
	bool                  m_bSaved;

	// next ID to be assigned by the manager.
	TDiscoveryID          m_nLastID;

public:
	// text preceeding a message from the manager on the log
	QString               m_sMessage;

	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
public:
	/**
	 * @brief CDiscovery: Constructs a new discovery services manager.
	 * @param parent
	 */
	CDiscovery(QObject *parent = nullptr);

	/**
	 * @brief ~CDiscovery: Destructor. Make sure you have stopped the magager befor destroying it.
	 */
	~CDiscovery();

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
public:
	/**
	 * @brief count allows you to access the number of working services for a given network.
	 * Locking: YES
	 * @return the number of services for the specified network. If no type is specified or the type is
	 * null, the total number of all services is returned, no matter whether they are working or not.
	 */
	quint32	count(const CNetworkType& oType = CNetworkType());

	/**
	 * @brief start initializes the Discovery Services Manager.
	 * Locking: YES
	 * @return whether loading the services was successful.
	 */
	bool	start();

	/**
	 * @brief stop prepares the Discovery Services Manager for destruction.
	 * Locking: YES
	 * @return true if the services have been successfully written to disk.
	 */
	bool	stop();		// prepares the Discovery Services Manager for destruction

	/**
	 * @brief save saves all discovery services to disk, if there have been important modifications to at
	 * least one service or bForceSaving is set to true.
	 * Locking: YES
	 * @param bForceSaving: Set this to true to force saving even if there have been no important service
	 * modifications, for example to make sure the hosts from the current session are saved properly.
	 * @return true if saving to file was successful; false otherwise.
	 */
	bool	save(bool bForceSaving);

	/**
	 * @brief add adds a new Service with a given URL to the manager.
	 * Locking: YES
	 * @param sURL
	 * @param eSType
	 * @param oNType
	 * @param nRating
	 * @return the service ID used to identify the service internally; 0 if the service has not been added.
	 */
	TDiscoveryID add(QString sURL, const TServiceType eSType,
					 const CNetworkType& oNType, const quint8 nRating = 7);

	/**
	 * @brief remove removes a service by ID.
	 * Locking: YES
	 * @param nID
	 * @return true if the removal was successful (e.g. the service could be found), false otherwise.
	 */
	bool         remove(TDiscoveryID nID);

	/**
	 * @brief clear removes all services from the manager.
	 * Locking: YES
	 * @param bInformGUI: Set this to true if the GUI shall be informed about the removal of the services.
	 * The default value is false, which represents the scenario on shutdown, where the GUI will be removed
	 * anyway shortly.
	 */
	void	clear(bool bInformGUI = false);

	/**
	 * @brief check verifies whether the given service is managed by the manager.
	 * Locking: YES
	 * @return true if managed; false otherwise
	 */
	bool	check(const CDiscoveryService* const pService);

signals:
	/**
	 * @brief serviceAdded is emitted each time a new service is added to the manager.
	 * @param pService
	 */
	void	serviceAdded(const CDiscoveryService* pService);

	/**
	 * @brief serviceRemoved is emitted (almost) each time a service is removed from the manager.
	 * @param nServiceID
	 */
	// CRITICAL: After emitting this signal, the GUI may not access the service in question any more.
	void	serviceRemoved(TDiscoveryID nServiceID);

	/**
	 * @brief serviceInfo is emitted once for each service on request via requestRuleList().
	 * @param pService
	 */
	void	serviceInfo(const CDiscoveryService* pService);

public slots:
	/**
	 * @brief requestServiceList can be used to obtain a complete list of all currently managed services.
	 * Connect to the serviceInfo() signal to recieve them.
	 * Locking: YES
	 */
	void requestServiceList();

	/**
	 * @brief updateService updates a service for a given network type with our IP. Note that not all
	 * service types might support or require such updates.
	 * Locking: YES
	 * @param type
	 * @return false if no service for the requested network type could be found; true otherwise.
	 */
	bool updateService(const CNetworkType& type); // Random service access
	bool updateService(TDiscoveryID nID);         // Manual service access

	/**
	 * @brief queryService
	 * Locking: YES
	 * @param type
	 * @return false if no service for the requested network type could be found; true otherwise.
	 */
	bool queryService(const CNetworkType& type); // Random service access
	bool queryService(TDiscoveryID nID);         // Manual service access

private:
	/**
	 * @brief load retrieves stored services from the HDD.
	 * Requires locking: YES
	 * @return true if loading from file was successful; false otherwise.
	 */
	bool load();
	bool load(QString sPath);

	/**
	 * @brief add... obvious... Note: if a duplicate is detected, the CDiscoveryService passed to the
	 * method is deleted within.
	 * Requires locking: YES
	 * @param pService
	 * @return true if the service was added; false if not (e.g. duplicate was detected).
	 */
	bool add(CDiscoveryService* pService);

	/**
	 * @brief manageDuplicates checks if an identical (or very similar) service is alreads present in the
	 * manager, decides which service to remove and frees unnecessary data.
	 * Requires locking: YES
	 * @param pService
	 * @return true if a duplicate was detected. pService is deleted and set to nullptr in that case. false
	 * otherwise.
	 */
	bool manageDuplicates(CDiscoveryService *&pService);

	/**
	 * @brief normalizeURL transforms a given URL string into a standard form to easa the detection of
	 * duplicates, filter out websites caching a service etc.
	 * Requires locking: NO
	 * @param sURL
	 */
	void normalizeURL(QString& sURL);

	/**
	 * @brief updateHelper: Helper Method. Performs a service update.
	 * Requires locking: YES
	 * @param pService
	 * @return false if a NULL pointer is passed as service; true otherwise.
	 */
	bool updateHelper(CDiscoveryService* pService, const CNetworkType& type);

	/**
	 * @brief queryHelper: Helper method. Performs a service query.
	 * Requires locking: YES
	 * @param pService
	 * @return false if a NULL pointer is passed as service; true otherwise.
	 */
	bool queryHelper(CDiscoveryService *pService, const CNetworkType& type);

	/**
	 * @brief getRandomService: Helper method. Allows to get a random service for a specified network.
	 * Requires locking: YES
	 * @param oNType
	 * @return A discovery service for the specified network; Null if no working service could be found for
	 * the specified network.
	 */
	CDiscoveryService* getRandomService(const CNetworkType& oNType);
};

} // namespace Discovery

/**
 * @brief discoveryManager is the static instance of this class used to manage discovery services
 * system wide.
 */
extern Discovery::CDiscovery discoveryManager;

#endif // DISCOVERY_H
