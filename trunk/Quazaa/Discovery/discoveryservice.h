#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QDataStream>
#include <QReadWriteLock>
#include <QString>
#include <QThread>
#include <QUuid>

#include "endpoint.h"

class CDiscoveryService : QThread
{
	/* ================================================================ */
	/* ========================= Definitions  ========================= */
	/* ================================================================ */
public:
	// Extend for all new supported service types.
	typedef enum { srNull = 0, srGWC = 1 } ServiceType;
	// Extend for all new supported networks.
	typedef enum { srNoNet = 0, srG2 = 1, srMulti = 2 } NetworkType;

private:
	// Note: Extra functionality should be implemented at lower level.
	typedef enum { srNoRequest = 0, srQuery = 1, srUpdate = 2 } Request;

	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */

private:
	ServiceType	m_nServiceType;
	NetworkType m_nNetworkType;
	QString		m_sServiceURL;
	quint8		m_nRating;
	quint8      m_nProbabilityMultiplicator;
	QUuid		m_oUUID;

	QReadWriteLock	m_pRWLock;
	Request			m_nRequest;
	CEndPoint		m_oOwnIP;

public:
	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
	CDiscoveryService();
	CDiscoveryService(QString sURL, ServiceType nSType, NetworkType nNType,
					  quint8 nRating, QUuid oID = QUuid());

	virtual ~CDiscoveryService() {} // Must be implemented by subclasses.

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	// Read/write rule from/to file
	static void		load(CDiscoveryService*& pService, QDataStream& oStream, const int nVersion);
	static void     save(const CDiscoveryService* const pService, QDataStream& oStream);

	// Sends our IP to service if it supports the operation (e.g. if it is a GWC).
	void update(CEndPoint& oOwnIP);
	void query();

	/* ================================================================ */
	/* ======================= Attribute Access ======================= */
	/* ================================================================ */
private:
	inline void setRating(quint8 nRating);

	/* ================================================================ */
	/* ======================= Private Helpers  ======================= */
	/* ================================================================ */
	void run();

	virtual void doQuery() {}	// Must be implemented by subclasses.
	virtual void doUpdate() {}	// Must be implemented by subclasses.

	/* ================================================================ */
	/* ======================== Friend Classes ======================== */
	/* ================================================================ */
friend class CDiscoveryServiceManager;
};

/**
  * Sets rating as well as probability multiplicator.
  * Requires Locking: RW
  */
void CDiscoveryService::setRating(quint8 nRating)
{
	m_nRating = ( nRating > 10 ) ? 10 : nRating;
	m_nProbabilityMultiplicator = ( m_nRating < 5 ) ? m_nRating : 5;
}

#endif // DISCOVERYSERVICE_H
