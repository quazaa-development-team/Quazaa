#include "downloadsource.h"
#include "queryhit.h"

CDownloadSource::CDownloadSource(CQueryHit* pHit)
{
	m_oAddress = pHit->m_pHitInfo->m_oNodeAddress;
	m_bPush = false; // todo
	m_oPushProxies << pHit->m_pHitInfo->m_lNeighbouringHubs;
	m_nProtocol = tpHTTP;
	m_nNetwork = dpGnutella2;
	m_oGUID = pHit->m_pHitInfo->m_oNodeGUID;
	m_lHashes << pHit->m_lHashes;
	m_tNextAccess = time(0);
	m_nFailures = 0;
}
