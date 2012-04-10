#ifndef GWC_H
#define GWC_H

#include "discoveryservice.h"

namespace Discovery
{

class CGWC : public CDiscoveryService
{
public:
	CGWC(const QUrl&, const Type, const CNetworkType&,
		 const quint8, const QUuid& = QUuid()) {}

	~CGWC() {}

	QString type();

private:
	void doQuery();
	void doUpdate();
};

}

#endif // GWC_H
