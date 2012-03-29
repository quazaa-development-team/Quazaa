#include "discoveryservicemanager.h"

CDiscoveryServiceManager::CDiscoveryServiceManager(QObject *parent) :
	QObject(parent)
{
}

bool CDiscoveryServiceManager::start()
{
	return true;
}

bool CDiscoveryServiceManager::stop()
{
	return true;
}

bool CDiscoveryServiceManager::load()
{
	return true;
}

bool CDiscoveryServiceManager::save()
{
	return true;
}

void CDiscoveryServiceManager::queryService( CDiscoveryService::NetworkType /*type*/ )
{

}
