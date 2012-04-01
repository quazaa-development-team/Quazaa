#ifndef GWC_H
#define GWC_H

#include "discoveryservice.h"

class CGWC : public CDiscoveryService
{
public:
	CGWC(const QString&, const ServiceType, const CNetworkType&,
		 const quint8, const QUuid& = QUuid()) {}

	~CGWC() {}

private:
	void doQuery();
	void doUpdate();
};

#endif // GWC_H
