#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QDataStream>
#include <QReadWriteLock>
#include <QUrl>

#include "endpoint.h"
#include "types.h"

// Requirements of all subclasses
#include <QNetworkAccessManager>
#include "hostcache.h"
#include "systemlog.h"

namespace Discovery
{

class CNetworkType
{
private:
	quint16 m_nNetworks;

public:
	CNetworkType();
	CNetworkType(quint16 type);
	CNetworkType(DiscoveryProtocol type);

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


class CDiscoveryService : public QObject
{
	Q_OBJECT

	/* ================================================================ */
	/* ========================= Definitions  ========================= */
	/* ================================================================ */
public:
	// Extend for all new supported service types.
	typedef enum { stNull = 0, stMulti = 1, stGWC = 2 } ServiceType;


	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */

	// TODO: Fix locking
	QReadWriteLock	m_oRWLock;

protected:
	ServiceType		m_nServiceType;
	CNetworkType	m_oNetworkType;
	QUrl			m_oServiceURL;
	quint8			m_nRating;
	quint8			m_nProbabilityMultiplicator;

	bool			m_bBanned;

	quint32			m_nLastHosts;
	quint32			m_nTotalHosts;
	quint32			m_tLastQueried;

	bool			m_bRunning;

	CEndPoint		m_oOwnIP;

	// List of pointers that will be set to 0 if this instance of CDiscoveryService is deleted.
	// Note that the content of this list is not forwarded to copies of this rule.
	std::list<const CDiscoveryService**> m_lPointers;

	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
public:
//	CDiscoveryService();
	CDiscoveryService(const QUrl& oURL, const CNetworkType& oNType, const quint8 nRating);
	CDiscoveryService(const CDiscoveryService& pService);

	virtual ~CDiscoveryService(); /** Must be implemented by subclasses. */

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

	/** Use this to generate valid services. Must be modified when writing subclasses. */
	static CDiscoveryService* createService(const QUrl& oURL, const ServiceType nSType,
											const CNetworkType& oNType, const quint8 nRating);

	// Registers a pointer to a Discovery Service to assure it is set to NULL if the Discovery
	// Service is deleted. Note that a pointer who has been registered needs to be unregistered
	// before freeing its memory.
	void registerPointer(const CDiscoveryService** pService) const;
	// Call this before removing a pointer you have previously registered.
	void unRegisterPointer(const CDiscoveryService** pService) const;

	// Sends our IP to service if it supports the operation (e.g. if it is a GWC).
	void update(CEndPoint& oOwnIP);
	void query();

	void cancelRequest();

signals:
	void updateRequestComplete(CDiscoveryService*); /** Must be emitted by subclasses */
	void queryRequestComplete(CDiscoveryService*);  /** Must be emitted by subclasses */

	/* ================================================================ */
	/* ======================= Attribute Access ======================= */
	/* ================================================================ */
public:
	inline bool isRunning() const;

	inline ServiceType serviceType() const;
	inline CNetworkType networkType() const;

	inline QUrl url() const;
//	inline QUuid uuid() const;

	inline quint32 lastHosts() const;
	inline quint32 totalHosts() const;

	virtual QString type() = 0; /** Must be implemented by subclasses. */

	inline bool isBanned() const;
	inline void setBanned(bool bBanned = true);

	inline quint8 rating() const;
private:
	inline void setRating(quint8 nRating);

	/* ================================================================ */
	/* ======================= Private Helpers  ======================= */
	/* ================================================================ */
private:
	virtual void doQuery()  throw() = 0;	/** Must be implemented by subclasses. */
	virtual void doUpdate() throw() = 0;	/** Must be implemented by subclasses. */

	/* ================================================================ */
	/* ======================== Friend Classes ======================== */
	/* ================================================================ */
};

/**
 * @brief CDiscoveryService::isRunning checks whether the current service is currently runnung.
 * @return
 * Requires Locking: R
 */
bool CDiscoveryService::isRunning() const
{
	return m_bRunning;
}

/**
 * @brief CDiscoveryService::serviceType
 * @return
 * Requires Locking: R
 */
CDiscoveryService::ServiceType CDiscoveryService::serviceType() const
{
	return m_nServiceType;
}

/**
 * @brief CDiscoveryService::networkType
 * @return
 * Requires Locking: R
 */
CNetworkType CDiscoveryService::networkType() const
{
	return m_oNetworkType;
}

/**
 * @brief CDiscoveryService::url
 * @return
 * Requires Locking: R
 */
QUrl CDiscoveryService::url() const
{
	return m_oServiceURL;
}

/**
 * @brief CDiscoveryService::uuid
 * @return
 * Requires Locking: R
 */
//QUuid CDiscoveryService::uuid() const
//{
//	return m_oUUID;
//}

/**
 * @brief CDiscoveryService::lastHosts
 * @return
 * Requires Locking: R
 */
quint32 CDiscoveryService::lastHosts() const
{
	return m_nLastHosts;
}

/**
 * @brief CDiscoveryService::totalHosts
 * @return
 * Requires Locking: R
 */
quint32 CDiscoveryService::totalHosts() const
{
	return m_nTotalHosts;
}

/**
 * @brief CDiscoveryService::isBlocked
 * @return
 * Requires Locking: R
 */
bool CDiscoveryService::isBanned() const
{
	return m_bBanned;
}

/**
 * @brief CDiscoveryService::setBlocked
 * @param bBlocked
 * Requires Locking: RW
 */
void CDiscoveryService::setBanned(bool bBanned)
{
	m_bBanned = bBanned;
}

/**
 * @brief CDiscoveryService::rating
 * @return
 * Requires Locking: R
 */
quint8 CDiscoveryService::rating() const
{
	return m_nRating;
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
