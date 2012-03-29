#include "discoveryservice.h"

CDiscoveryService::CDiscoveryService()
{
}

void CDiscoveryService::save(const CDiscoveryService* const pService, QDataStream &oStream)
{
	oStream << (quint8)(pService->m_nServiceType);
	oStream << (quint8)(pService->m_nNetworkType);
	oStream << (quint8)(pService->m_nRating);
	oStream << pService->m_sServiceURL;
}

void CDiscoveryService::load(CDiscoveryService*& pService, QDataStream &oStream, int)
{
	if ( pService )
	{
		delete pService;
		pService = NULL;
	}

	quint8		nServiceType;
	quint8		nNetworkType;
	quint8		nRating;
	QString		sServiceURL;

	oStream >> nServiceType;
	oStream >> nNetworkType;
	oStream >> nRating;
	oStream >> sServiceURL;

	switch ( nServiceType )
	{
	default:
		;
	}


	pService->m_nServiceType = (ServiceType)nServiceType;
	pService->m_nNetworkType = (NetworkType)nNetworkType;
	pService->m_nRating      = nRating;
	pService->m_sServiceURL  = sServiceURL;
}
