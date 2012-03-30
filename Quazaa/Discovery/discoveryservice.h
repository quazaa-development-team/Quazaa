#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QDataStream>
#include <QString>

class CDiscoveryService
{
public:
	typedef enum { srNull = 0, srGWC = 1 } ServiceType;
	typedef enum { srNoNet = 0, srG2 = 1, srMulti = 2 } NetworkType;

public:
	CDiscoveryService();

	// Read/write rule from/to file
	static void		load(CDiscoveryService*& pService, QDataStream& oStream, const int nVersion);
	static void     save(const CDiscoveryService* const pService, QDataStream& oStream);

private:
	ServiceType	m_nServiceType;
	NetworkType m_nNetworkType;
	QString		m_sServiceURL;
	quint8		m_nRating;
};

#endif // DISCOVERYSERVICE_H
