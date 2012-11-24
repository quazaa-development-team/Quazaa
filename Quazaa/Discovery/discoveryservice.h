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
	TServiceType	m_nServiceType; // GWC, UKHL, ...
	CNetworkType	m_oNetworkType; // could be several in case of GWC for instance
	QUrl			m_oServiceURL;
	quint8			m_nRating;      // 0: bad; 10: very good
	quint8			m_nProbabilityMultiplicator; // [0-5] based on rating

	bool			m_bBanned;      // service URL is blocked

	TDiscoveryID	m_nID;          // ID used by the manager to identify the service

	quint32			m_nLastHosts;   // hosts returned by the service on last query
	quint32			m_nTotalHosts;  // all hosts we ever got from the service
	quint32			m_tLastQueried; // last time we accessed the host
	quint32			m_tLastSuccess; // last time we queried the service successfully
	quint8			m_nFailures;    // query failures in a row

	bool			m_bRunning;     // service is currently doing network communication

	// List of pointers that will be set to 0 if this instance of CDiscoveryService is deleted.
	// Note that the content of this list is not forwarded to copies of this service.
	std::list<const CDiscoveryService**> m_lPointers;

	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
public:
	/**
	 * @brief CDiscoveryService: Constructor. Creates a new service.
	 * @param oURL
	 * @param oNType
	 * @param nRating
	 */
	CDiscoveryService(const QUrl& oURL, const CNetworkType& oNType, quint8 nRating);

	/**
	 * @brief CDiscoveryService: Copy constructor. Copies all but the list of registered pointers.
	 * @param pService
	 */
	CDiscoveryService(const CDiscoveryService& pService);

	/**
	 * @brief ~CDiscoveryService
	 */
	virtual ~CDiscoveryService(); /** Must be implemented by subclasses. */

private:
	/* ================================================================ */
	/* ========================== Operators  ========================== */
	/* ================================================================ */
	/**
	 * @brief operator ==: Allows to compare two services. Services are considered to be equal if they are
	 * of the same service type, serve the same networks and have the same URL.
	 * Requires locking: R
	 * @param pService
	 * @return
	 */
	virtual bool	operator==(const CDiscoveryService& pService) const;

	/**
	 * @brief operator !=: Allows to compare two services. Services are considered to be equal if they are
	 * of the same service type, serve the same networks and have the same URL.
	 * Requires locking: R
	 * @param pService
	 * @return
	 */
	bool			operator!=(const CDiscoveryService& pService) const;

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	/**
	 * @brief load reads a service from the provided QDataStream and creates a new Object from the data.
	 * Note that if a non-NULL pointer is given to the function, that object is deleted.
	 * Locking: / (static member)
	 * @param pService
	 * @param fsStream
	 * @param nVersion
	 */
	static void		load(CDiscoveryService*& pService, QDataStream& fsFile, const int nVersion);

	/**
	 * @brief save writes pService to given QDataStream.
	 * Locking: / (static member)
	 * @param pService
	 * @param fsFile
	 */
	static void     save(const CDiscoveryService* const pService, QDataStream& fsFile);

	/**
	 * @brief createService allows to create valid services.
	 * Locking: / (static member)
	 * @param sURL
	 * @param eSType
	 * @param oNType
	 * @param nRating
	 * @return
	 */
	/** Must be modified when writing subclasses. */
	static CDiscoveryService* createService(const QString &sURL, TServiceType eSType,
											const CNetworkType& oNType, quint8 nRating);

	/**
	 * @brief update sends our own IP to service if the service supports the operation (e.g. if it is a GWC).
	 * Locking: RW
	 * @param oOwnIP
	 */
	void update();

	/**
	 * @brief query accesses the service to recieve network hosts for initial connection and/or
	 * alternative service URLs.
	 * Locking: RW
	 */
	void query();

	/**
	 * @brief cancelRequest stops any update or query operation currently in progress.
	 * Locking: RW
	 */
	void cancelRequest();

public:
	/**
	 * @brief registerPointer registers a pointer pointing to the service. All registered pointers to this
	 * service will be set to NULL upon deletion of the service. All registered pointers need to be
	 * unregistered before removing them from memory.
	 * Note that this method is const, because it does not change the data defining the DiscoveryService in
	 * question. It does, however, add the pointer pService to an internal storage structure. So make sure to
	 * lock the service for write when using.
	 * Requires locking: RW
	 * @param pService
	 */
	void registerPointer(const CDiscoveryService** pService) const;

	/**
	 * @brief unRegisterPointer unregisters a pointer previously registered.
	 * Note that this method is const, because it does not change the data defining the DiscoveryService in
	 * question. It does, however, add the pointer pService to an internal storage structure. So be careful
	 * when using and make sure you have a write lock on the service.
	 * Requires locking: RW
	 * @param pService
	 */
	void unRegisterPointer(const CDiscoveryService** pService) const;

signals:
	/**
	 * @brief updateRequestComplete informs the caller of update() that his request has been completed.
	 */
	void updateRequestComplete(CDiscoveryService*); /** Must be emitted by subclasses */
	/**
	 * @brief queryRequestComplete informs the caller of query() that his request has been completed.
	 */
	void queryRequestComplete(CDiscoveryService*);  /** Must be emitted by subclasses */

	/* ================================================================ */
	/* ======================= Attribute Access ======================= */
	/* ================================================================ */
public:
	/**
	 * @brief serviceType
	 * Requires locking: R
	 * @return
	 */
	inline TServiceType serviceType() const;

	/**
	 * @brief networkType
	 * Requires locking: R
	 * @return
	 */
	inline CNetworkType networkType() const;

	/**
	 * @brief type
	 * Requires locking: R
	 * @return a string representation of the full type of this service for usage in the GUI
	 */
	virtual QString type() const = 0; /** Must be implemented by subclasses. */

	/**
	 * @brief url
	 * Requires locking: R
	 * @return
	 */
	inline QString url() const;

	/**
	 * @brief rating
	 * Requires locking: R
	 * @return
	 */
	inline quint8 rating() const;

	/**
	 * @brief isBanned
	 * Requires locking: R
	 * @return
	 */
	inline bool isBanned() const;

	/**
	 * @brief id
	 * Requires locking: R
	 * @return
	 */
	inline TDiscoveryID id() const;

	/**
	 * @brief lastHosts
	 * Requires locking: R
	 * @return
	 */
	inline quint32 lastHosts() const;

	/**
	 * @brief totalHosts
	 * Requires locking: R
	 * @return
	 */
	inline quint32 totalHosts() const;

	inline quint32 lastQueried() const;
	inline quint32 lastSuccess() const;
	inline quint8 failures() const;

	/**
	 * @brief isRunning
	 * Requires locking: R
	 * @return
	 */
	inline bool isRunning() const;

private:
	/**
	 * @brief setRating: Helper method to set rating and probability multiplicator simultaneously.
	 * Requires locking: RW
	 * @param nRating
	 */
	inline void setRating(quint8 nRating);

	/* ================================================================ */
	/* ======================= Private Helpers  ======================= */
	/* ================================================================ */
private:
	/**
	 * @brief doQuery: Helper method for query(). Performs the actual querying.
	 * Requires locking: RW
	 */
	virtual void doQuery()  throw() = 0;	/** Must be implemented by subclasses. */

	/**
	 * @brief doUpdate: Helper method for update(). Performs the actual updating.
	 * Requires locking: RW
	 */
	virtual void doUpdate() throw() = 0;	/** Must be implemented by subclasses. */

	/* ================================================================ */
	/* ======================== Friend Classes ======================== */
	/* ================================================================ */
	friend class CDiscovery;
};

TServiceType CDiscoveryService::serviceType() const
{
	return m_nServiceType;
}

CNetworkType CDiscoveryService::networkType() const
{
	return m_oNetworkType;
}

QString CDiscoveryService::url() const
{
	return m_oServiceURL.toString();
}

quint8 CDiscoveryService::rating() const
{
	return m_nRating;
}

bool CDiscoveryService::isBanned() const
{
	return m_bBanned;
}

TDiscoveryID CDiscoveryService::id() const
{
	return m_nID;
}

quint32 CDiscoveryService::lastHosts() const
{
	return m_nLastHosts;
}

quint32 CDiscoveryService::totalHosts() const
{
	return m_nTotalHosts;
}

quint32 CDiscoveryService::lastQueried() const
{
	return m_tLastQueried;
}

quint32 CDiscoveryService::lastSuccess() const
{
	return m_tLastSuccess;
}

quint8 CDiscoveryService::failures() const
{
	return m_nFailures;
}

bool CDiscoveryService::isRunning() const
{
	return m_bRunning;
}

void CDiscoveryService::setRating(quint8 nRating)
{
	m_nRating = ( nRating > 10 ) ? 10 : nRating;
	m_nProbabilityMultiplicator = ( m_nRating < 5 ) ? m_nRating : 5;
}

}

#endif // DISCOVERYSERVICE_H
