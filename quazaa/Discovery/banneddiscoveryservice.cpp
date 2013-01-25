#include "banneddiscoveryservice.h"

using namespace Discovery;

CBannedDiscoveryService::CBannedDiscoveryService(const QUrl& oURL, const CNetworkType&, quint8 nRating) :
	CDiscoveryService( oURL, CNetworkType( dpNull ), nRating )
{
	m_nServiceType = stBanned;
}

CBannedDiscoveryService::~CBannedDiscoveryService()
{
}

void CBannedDiscoveryService::doQuery() throw()
{
	Q_ASSERT( false );
}

void CBannedDiscoveryService::doUpdate() throw()
{
	Q_ASSERT( false );
}


void CBannedDiscoveryService::doCancelRequest() throw()
{
	Q_ASSERT( false );
}
