#ifndef GWC_H
#define GWC_H

#include "discoveryservice.h"

namespace Discovery
{

// http://www.gnucleus.com/gwebcache/newgwc.html

class CGWC : public CDiscoveryService
{
	Q_OBJECT

	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */
private:
	QNetworkAccessManager* m_pRequest;

	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
public:
	CGWC(const QUrl& oURL, const CNetworkType& oNType, quint8 nRating);

	~CGWC();

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	inline QString type() const { return QString( "GWC" ); }

private:
	void doQuery()  throw();
	void doUpdate() throw();
	void doCancelRequest() throw();

private slots:
	void queryRequestCompleted(QNetworkReply* pReply);
	void updateRequestCompleted(QNetworkReply* pReply);
};

}

#endif // GWC_H
