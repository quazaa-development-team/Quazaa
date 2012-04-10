#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QDataStream>
#include <QReadWriteLock>
#include <QUrl>
#include <QThread>
#include <QUuid>

#include "endpoint.h"

namespace Discovery
{

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

	bool operator==(const CNetworkType& type) const;
	bool operator!=(const CNetworkType& type) const;

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
	typedef enum { srNull = 0, srMulti = 1, srGWC = 2 } Type;

private:
	// Note: Extra functionality should be implemented at lower level.
	typedef enum { srNoRequest = 0, srQuery = 1, srUpdate = 2 } Request;

	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */

private:
	Type			m_nServiceType;
	CNetworkType	m_oNetworkType;
	QUrl			m_oServiceURL;
	quint8			m_nRating;
	quint8			m_nProbabilityMultiplicator;
	QUuid			m_oUUID;

	quint32			m_nLastHosts;
	quint32			m_nTotalHosts;

	QReadWriteLock	m_pRWLock;
	Request			m_nRequest;
	CEndPoint		m_oOwnIP;

	// List of pointers that will be set to 0 if this instance of CDiscoveryService is deleted.
	// Note that the content of this list is not forwarded to copies of this rule.
	std::list<CDiscoveryService**> m_lPointers;

public:
	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
	CDiscoveryService();
	CDiscoveryService(const QUrl& oURL, const Type nSType, const CNetworkType& oNType,
					  const quint8 nRating, const QUuid& oID = QUuid());
	CDiscoveryService(const CDiscoveryService& pService);

	virtual ~CDiscoveryService(); // Must be implemented by subclasses.

	/* ================================================================ */
	/* ========================== Operators  ========================== */
	/* ================================================================ */
	virtual bool	operator==(const CDiscoveryService& pService) const;
	bool			operator!=(const CDiscoveryService& pService) const;

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	// Read/write rule from/to file
	static void		load(CDiscoveryService*& pService, QDataStream& oStream, const int nVersion);
	static void     save(const CDiscoveryService* const pService, QDataStream& oStream);

	// Use this to generate valid services. Must be modified when writing subclasses.
	static CDiscoveryService* createService(const QUrl& oURL, const Type nSType,
											const CNetworkType& oNType, const quint8 nRating,
											const QUuid& oID = QUuid());

	// Registers a pointer to a Discovery Service to assure it is set to NULL if the Discovery
	// Service is deleted. Note that a pointer who has been registered needs to be unregistered
	// before freeing its memory.
	void registerPointer(CDiscoveryService** pService);
	// Call this before removing a pointer you have previously registered.
	void unRegisterPointer(CDiscoveryService** pService);

	// Sends our IP to service if it supports the operation (e.g. if it is a GWC).
	void update(CEndPoint& oOwnIP);
	void query();

	inline Type getType() const;

	inline QUrl getUrl() const;

	inline void count();
	inline quint32 getTodayCount() const;
	inline quint32 getTotalCount() const;

	virtual QString type() { return QString(); } // Must be implemented by subclasses.

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
friend class CDiscovery;
};

CDiscoveryService::Type CDiscoveryService::getType() const
{
	return m_nServiceType;
}

QUrl CDiscoveryService::getUrl() const
{
	return m_oServiceURL;
}

void CDiscoveryService::count()
{
	++m_nLastHosts;
	++m_nTotalHosts;
}

quint32 CDiscoveryService::getTodayCount() const
{
	return m_nLastHosts;
}

quint32 CDiscoveryService::getTotalCount() const
{
	return m_nTotalHosts;
}

/**
  * Sets rating as well as probability multiplicator.
  * Requires Locking: RW
  */
void CDiscoveryService::setRating(quint8 nRating)
{
	m_nRating = ( nRating > 10 ) ? 10 : nRating;
	m_nProbabilityMultiplicator = ( m_nRating < 5 ) ? m_nRating : 5;
}

}

#endif // DISCOVERYSERVICE_H
