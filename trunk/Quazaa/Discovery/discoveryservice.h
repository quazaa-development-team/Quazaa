#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QDataStream>
#include <QString>
#include <QUuid>

class CDiscoveryService
{
	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */
public:
	typedef enum { srNull = 0, srGWC = 1 } ServiceType;
	typedef enum { srNoNet = 0, srG2 = 1, srMulti = 2 } NetworkType;

private:
	ServiceType	m_nServiceType;
	NetworkType m_nNetworkType;
	QString		m_sServiceURL;
	quint8		m_nRating;
	quint8      m_nProbabilityMultiplicator;
	QUuid		m_oUUID;

public:
	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
	CDiscoveryService();
	CDiscoveryService(QString sURL, ServiceType nSType, NetworkType nNType,
					  quint8 nRating, QUuid oID = QUuid());

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	// Read/write rule from/to file
	static void		load(CDiscoveryService*& pService, QDataStream& oStream, const int nVersion);
	static void     save(const CDiscoveryService* const pService, QDataStream& oStream);

	void update();	// Sends our IP to service if it supports the operation (e.g. if it is a GWC).
	void query();

	/* ================================================================ */
	/* ======================= Attribute Access ======================= */
	/* ================================================================ */
private:
	inline void setRating(quint8 nRating);

	/* ================================================================ */
	/* ======================= Private Helpers  ======================= */
	/* ================================================================ */


	/* ================================================================ */
	/* ======================== Friend Classes ======================== */
	/* ================================================================ */
friend class CDiscoveryServiceManager;
};

void CDiscoveryService::setRating(quint8 nRating)
{
	m_nRating = ( nRating > 10 ) ? 10 : nRating;
	m_nProbabilityMultiplicator = ( m_nRating < 5 ) ? m_nRating : 5;
}

#endif // DISCOVERYSERVICE_H
