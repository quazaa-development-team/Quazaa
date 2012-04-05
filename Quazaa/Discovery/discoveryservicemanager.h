#ifndef DISCOVERYSERVICEMANAGER_H
#define DISCOVERYSERVICEMANAGER_H

#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QUuid>

#include "discoveryservice.h"

// Increment this if there have been made changes to the way of storing discovery services.
#define DISCOVERY_CODE_VERSION	0
// History:
// 0 - Initial implementation

// http://www.gnucleus.com/gwebcache/newgwc.html

class CDiscoveryServiceManager : public QObject
{
	Q_OBJECT
	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */
public:
	typedef std::list< CDiscoveryService*  > CDiscoveryServicesList;
	typedef CDiscoveryServicesList::const_iterator CIterator;

	QReadWriteLock m_pRWLock;

private:
	CDiscoveryServicesList  m_lServices;

	// true if current Discovery Service Manager state has already been saved to file, false otherwise.
	bool					m_bSaved;
	bool					m_bIsRunning;

public:
	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
	explicit CDiscoveryServiceManager(QObject *parent = 0);

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	inline quint32	getCount() const;

	bool	start();	// connects signals etc.
	bool	stop();		// makes the Discovery Services Manager ready for destruction
	bool	load();
	bool	save(bool bForceSaving);

	inline bool	isRunning();

	QUuid	add(const QString& sURL, const CDiscoveryService::ServiceType nSType,
				const CNetworkType& oNType, const quint8 nRating = 7);
	bool	remove(const QUuid& oServiceID);
	void	clear();

signals:


public slots:
	bool updateService(CDiscoveryService::ServiceType type); // sends our IP to service (e.g. GWC)
	bool queryService(CNetworkType type);
	void serviceActionFinished();

private:
	bool load(QString sPath);
	bool add(CDiscoveryService* pService);
	void normalizeURL(QUrl& sURL);

	CDiscoveryService* getRandomService(CDiscoveryService::ServiceType nSType);
	CDiscoveryService* getRandomService(CNetworkType oNType);
};

/**
  * Returns the number of Discovery Services.
  * Requires Locking: R
  */
quint32 CDiscoveryServiceManager::getCount() const
{
	return m_lServices.size();
}

/**
  * Returns true if there is currently a request operation running.
  * Locking: R
  */
bool CDiscoveryServiceManager::isRunning()
{
	QReadLocker l( &m_pRWLock );
	return m_bIsRunning;
}

extern CDiscoveryServiceManager discoveryManager;

#endif // DISCOVERYSERVICEMANAGER_H
