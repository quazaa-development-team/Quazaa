#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

#include <QString>

class CDiscoveryService
{
public:
	typedef enum { srNull = 0, srGWC = 1 } ServiceType;
	typedef enum { srNoNet = 0, srG2 = 1, srMulti = 2 } NetworkType;

public:
	CDiscoveryService();

private:
	ServiceType	m_ServiceType;
	NetworkType m_NetworkType;
	QString		m_sServiceURL;
	quint8		m_nRating;
};

#endif // DISCOVERYSERVICE_H
