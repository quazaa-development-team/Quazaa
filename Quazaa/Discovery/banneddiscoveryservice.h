#ifndef BANNEDDISCOVERYSERVICE_H
#define BANNEDDISCOVERYSERVICE_H

#include "discoveryservice.h"

namespace Discovery
{

class CBannedDiscoveryService : public CDiscoveryService
{
	Q_OBJECT

	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
public:
	CBannedDiscoveryService(const QUrl& oURL, const CNetworkType& oNType, quint8 nRating);

	~CBannedDiscoveryService();

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	inline QString type() const { return QString( "Banned" ); }

private:
	void doQuery()  throw();
	void doUpdate() throw();
	void doCancelRequest() throw();
};

}

#endif // BANNEDDISCOVERYSERVICE_H
