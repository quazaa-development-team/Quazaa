#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QDataStream>
#include <QReadWriteLock>
#include <QUrl>

#include "endpoint.h"
#include "discovery.h"

// Requirements of all subclasses
#include <QNetworkAccessManager>
#include "hostcache.h"
#include "systemlog.h"

namespace Discovery
{

class CDiscoveryService : public QObject
{
	Q_OBJECT

	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */
public:
	// TODO: Fix locking
	QReadWriteLock	m_oRWLock;

protected:
	TServiceType	m_nServiceType;
	CNetworkType	m_oNetworkType;
	QUrl			m_oServiceURL;
	quint8			m_nRating;
	quint8			m_nProbabilityMultiplicator;

	bool			m_bBanned;

	TDiscoveryID	m_nID;

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
	CDiscoveryService(const QUrl& oURL, const CNetworkType& oNType, quint8 nRating);
	CDiscoveryService(const CDiscoveryService& pService);

	virtual ~CDiscoveryService(); /** Must be implemented by subclasses. */

private:
	/* ================================================================ */
	/* ========================== Operators  ========================== */
	/* ================================================================ */
	virtual bool	operator==(const CDiscoveryService& pService) const;
	bool			operator!=(const CDiscoveryService& pService) const;

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	// Read/write rule from/to file
	static void		load(CDiscoveryService*& pService, QDataStream& fsStream, const int nVersion);
	static void     save(const CDiscoveryService* const pService, QDataStream& oStream);

	/** Use this to generate valid services. Must be modified when writing subclasses. */
	static CDiscoveryService* createService(const QString &sURL, TServiceType eSType,
											const CNetworkType& oNType, quint8 nRating);

public:
	// Registers a pointer to a Discovery Service to assure it is set to NULL if the Discovery
	// Service is deleted. Note that a pointer who has been registered needs to be unregistered
	// before freeing its memory.
	void registerPointer(const CDiscoveryService** pService) const;
	// Call this before removing a pointer you have previously registered.
	void unRegisterPointer(const CDiscoveryService** pService) const;

private:
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

	inline TServiceType serviceType() const;
	inline CNetworkType networkType() const;

	inline QUrl url() const;
	inline TDiscoveryID id() const;

	inline quint32 lastHosts() const;
	inline quint32 totalHosts() const;

	virtual QString type() const = 0; /** Must be implemented by subclasses. */

	inline bool isBanned() const;

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
	friend class CDiscovery;
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
TServiceType CDiscoveryService::serviceType() const
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
 * @brief CDiscoveryService::id
 * @return
 * Requires Locking: R
 */
TDiscoveryID CDiscoveryService::id() const
{
	return m_nID;
}

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
