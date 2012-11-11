#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QDataStream>
#include <QReadWriteLock>
#include <QUrl>
#include <QThread>
#include <QUuid>

#include "endpoint.h"
#include "types.h"

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


class CDiscoveryService : public QThread
{
	/* ================================================================ */
	/* ========================= Definitions  ========================= */
	/* ================================================================ */
public:
	/** Must be extended when writing subclasses. */
	typedef enum { stNull = 0, stGWC = 1 } ServiceType;

private:
	// Note: Extra functionality should be implemented at lower level.
	typedef enum { srNoRequest = 0, srQuery = 1, srUpdate = 2 } Request;

	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */

protected:
	ServiceType		m_nServiceType;     // GWC, UHKL,...
	CNetworkType	m_oNetworkType;     // all networks bootstrapped by this service
	QUrl			m_oServiceURL;
	quint8			m_nRating;          //
	quint8			m_nProbabilityMultiplicator;
	QUuid			m_oUUID;
	bool			m_bQueued;
	bool            m_bBlocked;         // Manually blocked in discovery service manager?

	quint32			m_nLastHosts;
	quint32			m_nTotalHosts;

	QReadWriteLock	m_oRWLock;
	Request			m_nRequest;
	CEndPoint		m_oOwnIP;

	// List of pointers that will be set to 0 if this instance of CDiscoveryService is deleted.
	// Note that the content of this list is not forwarded to copies of this rule.
	std::list<const CDiscoveryService**> m_lPointers;

public:
	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
	CDiscoveryService();
	CDiscoveryService(const QUrl& oURL, const ServiceType nSType, const CNetworkType& oNType,
					  const quint8 nRating, const QUuid& oID = QUuid());
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
											const CNetworkType& oNType, const quint8 nRating,
											const QUuid& oID = QUuid());

	// Registers a pointer to a Discovery Service to assure it is set to NULL if the Discovery
	// Service is deleted. Note that a pointer who has been registered needs to be unregistered
	// before freeing its memory.
	void registerPointer(const CDiscoveryService** pService) const;
	// Call this before removing a pointer you have previously registered.
	void unRegisterPointer(const CDiscoveryService** pService) const;

	// Sends our IP to service if it supports the operation (e.g. if it is a GWC).
	void update(CEndPoint& oOwnIP,bool bExecute = true);
	void query(bool bExecute = true);
	void execute();

	inline ServiceType serviceType() const;
	inline CNetworkType networkType() const;

	inline QUrl url() const;
	inline QUuid uuid() const;

	inline void count();
	inline quint32 todayCount() const;
	inline quint32 totalCount() const;

	inline bool isQueued() const;

	virtual QString type() { return QString(); } /** Must be implemented by subclasses. */

	/* ================================================================ */
	/* ======================= Attribute Access ======================= */
	/* ================================================================ */
private:
	inline void setRating(quint8 nRating);

	/* ================================================================ */
	/* ======================= Private Helpers  ======================= */
	/* ================================================================ */
	void run();

	virtual void doQuery() {}	/** Must be implemented by subclasses. */
	virtual void doUpdate() {}	/** Must be implemented by subclasses. */

	/* ================================================================ */
	/* ======================== Friend Classes ======================== */
	/* ================================================================ */
friend class CDiscovery;
};

CDiscoveryService::ServiceType CDiscoveryService::serviceType() const
{
	return m_nServiceType;
}

CNetworkType CDiscoveryService::networkType() const
{
	return m_oNetworkType;
}

QUrl CDiscoveryService::url() const
{
	return m_oServiceURL;
}

QUuid CDiscoveryService::uuid() const
{
	return m_oUUID;
}

void CDiscoveryService::count()
{
	++m_nLastHosts;
	++m_nTotalHosts;
}

quint32 CDiscoveryService::todayCount() const
{
	return m_nLastHosts;
}

quint32 CDiscoveryService::totalCount() const
{
	return m_nTotalHosts;
}

bool CDiscoveryService::isQueued() const
{
	return m_bQueued;
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
