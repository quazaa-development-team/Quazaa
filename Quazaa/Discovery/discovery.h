#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QObject>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QString>

#include "discoveryservice.h"

// Increment this if there have been made changes to the way of storing discovery services.
#define DISCOVERY_CODE_VERSION	0
// History:
// 0 - Initial implementation

// http://www.gnucleus.com/gwebcache/newgwc.html

namespace Discovery
{

class CDiscovery : public QObject
{
	Q_OBJECT
	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */
public:
	typedef std::pair< const QString, CDiscoveryService*  > CMapPair;

	typedef std::list< CDiscoveryService*				> CDiscoveryServicesList;
	typedef std::map < const QString, CDiscoveryService*	> CDiscoveryServicesMap;

	typedef CDiscoveryServicesMap::const_iterator CConstIterator;
	typedef CDiscoveryServicesMap::iterator CIterator;

	typedef CDiscoveryServicesList::const_iterator CListIterator;

	QReadWriteLock m_pRWLock;

private:
	CDiscoveryServicesMap  m_mServices;
	CDiscoveryServicesList	m_lAsyncTODO;

	// true if current Discovery Service Manager state has already been saved to file, false otherwise.
	bool					m_bSaved;
	bool					m_bIsRunning;

public:
	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
	explicit CDiscovery(QObject *parent = 0);

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	inline quint32	getCount() const;

	bool	start();	// connects signals etc.
	bool	stop();		// makes the Discovery Services Manager ready for destruction
	bool	load();
	bool	save(bool bForceSaving);

	bool	add(const QString& sURL, const CDiscoveryService::ServiceType nSType,
				const CNetworkType& oNType, const quint8 nRating = 7);
	bool	remove(const QString &oServiceURL);
	void	clear(bool bInformGUI = false);

	bool	check(const CDiscoveryService* const pService);

	inline bool	isRunning();

signals:
	void	serviceAdded(const CDiscoveryService* pService);
	void	serviceRemoved(QSharedPointer<CDiscoveryService> pService);

	void	serviceInfo(const CDiscoveryService* pService);

public slots:
	// Trigger this to let the Discovery Services Manager emit all rules
	void requestRuleList();

	bool updateService(CDiscoveryService::ServiceType type); // sends our IP to service (e.g. GWC)
	bool queryService(const CNetworkType& type);

	bool updateService(const QString& oServiceURL);
	bool queryService(const QString& oServiceURL);

	void serviceActionFinished();

private:
	bool load(QString sPath);
	bool add(CDiscoveryService* pService);
	void normalizeURL(QUrl& sURL);

	bool updateHelper(CDiscoveryService* pService, QWriteLocker& lock);
	bool queryHelper(CDiscoveryService *pService, QWriteLocker& lock);

	CDiscoveryService* getRandomService(CDiscoveryService::ServiceType nSType);
	CDiscoveryService* getRandomService(const CNetworkType& oNType);
};

/**
  * Returns the number of Discovery Services.
  * Requires Locking: R
  */
quint32 CDiscovery::getCount() const
{
	return m_mServices.size();
}

/**
  * Returns true if there is currently a request operation running.
  * Locking: R
  */
bool CDiscovery::isRunning()
{
	QReadLocker l( &m_pRWLock );
	return m_bIsRunning;
}

}

extern Discovery::CDiscovery discoveryManager;

#endif // DISCOVERY_H
