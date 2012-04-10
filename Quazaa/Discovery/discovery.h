#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QObject>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QString>
#include <QUuid>

#include "discoveryservice.h"

// Increment this if there have been made changes to the way of storing discovery services.
#define DISCOVERY_CODE_VERSION	0
// History:
// 0 - Initial implementation

// http://www.gnucleus.com/gwebcache/newgwc.html



//  reuse existing network enum (the one found in types.h)



namespace Discovery
{

class CDiscovery : public QObject
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
	explicit CDiscovery(QObject *parent = 0);

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	inline quint32	getCount() const;

	bool	start();	// connects signals etc.
	bool	stop();		// makes the Discovery Services Manager ready for destruction
	bool	load();
	bool	save(bool bForceSaving);

	QUuid	add(const QString& sURL, const CDiscoveryService::Type nSType,
				const CNetworkType& oNType, const quint8 nRating = 7);
	bool	remove(const QUuid& oServiceID);
	void	clear();

	bool	check(const CDiscoveryService* const pService);

	inline bool	isRunning();

signals:
	void	serviceAdded(CDiscoveryService* pService);
	void	serviceRemoved(QSharedPointer<CDiscoveryService> pService);

	void	serviceInfo(CDiscoveryService* pService);

public slots:
	// Trigger this to let the Discovery Services Manager emit all rules
	void requestRuleList();

	bool updateService(CDiscoveryService::Type type); // sends our IP to service (e.g. GWC)
	bool queryService(CNetworkType type);
	void serviceActionFinished();

private:
	bool load(QString sPath);
	bool add(CDiscoveryService* pService);
	void normalizeURL(QUrl& sURL);

	CDiscoveryService* getRandomService(CDiscoveryService::Type nSType);
	CDiscoveryService* getRandomService(CNetworkType oNType);
};

/**
  * Returns the number of Discovery Services.
  * Requires Locking: R
  */
quint32 CDiscovery::getCount() const
{
	return m_lServices.size();
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
