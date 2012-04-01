#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QDataStream>
#include <QReadWriteLock>
#include <QString>
#include <QThread>
#include <QUuid>

#include "endpoint.h"

class CNetworkType
{
private:
	quint16 m_nNetworks;

public:
	// Extend for all new supported networks.
	typedef enum { NoNetwork = 0, gnutella = 1, G2 = 2, Ares = 4, eDonkey2000 = 8 } NetworkType;

	CNetworkType();
	CNetworkType(quint16 type);
	CNetworkType(NetworkType type);

	bool isGnutella() const;
	void setGnutella( bool );

	bool isG2() const;
	void setG2( bool );

	bool isAres() const;
	void setAres( bool );

	bool isEDonkey2000() const;
	void setEDonkey2000( bool );

	bool isNetwork(const CNetworkType& type) const;
	void setNetwork(const CNetworkType& type);

	bool isMulti() const;

	quint16 toQuint16() const;
};


class CDiscoveryService : QThread
{
	/* ================================================================ */
	/* ========================= Definitions  ========================= */
	/* ================================================================ */
public:
	// Extend for all new supported service types.
	typedef enum { srNull = 0, srGWC = 1 } ServiceType;

private:
	// Note: Extra functionality should be implemented at lower level.
	typedef enum { srNoRequest = 0, srQuery = 1, srUpdate = 2 } Request;

	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */

private:
	ServiceType	 m_nServiceType;
	CNetworkType m_oNetworkType;
	QString		 m_sServiceURL;
	quint8		 m_nRating;
	quint8       m_nProbabilityMultiplicator;
	QUuid		 m_oUUID;

	QReadWriteLock	m_pRWLock;
	Request			m_nRequest;
	CEndPoint		m_oOwnIP;

public:
	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
	CDiscoveryService();
	CDiscoveryService(const QString& sURL, const ServiceType nSType, const CNetworkType& oNType,
					  const quint8 nRating, const QUuid& oID = QUuid());

	virtual ~CDiscoveryService() {} // Must be implemented by subclasses.

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	// Read/write rule from/to file
	static void		load(CDiscoveryService*& pService, QDataStream& oStream, const int nVersion);
	static void     save(const CDiscoveryService* const pService, QDataStream& oStream);

	// Use this to generate valid services. Must be modified when writing subclasses.
	static CDiscoveryService* createService(const QString& sURL, const ServiceType nSType,
											const CNetworkType& oNType, const quint8 nRating,
											const QUuid& oID = QUuid());

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
