#include "gwc.h"

using namespace Discovery;

CGWC::CGWC(const QUrl&, const ServiceType, const CNetworkType&,
		   const quint8, const QUuid&)
{
	m_nServiceType = stGWC;
}

QString CGWC::type()
{
	return QString( "GWC" );
}

void CGWC::doQuery()
{
	// TODO: Implement.
}

void CGWC::doUpdate()
{
	// TODO: Implement.
}
