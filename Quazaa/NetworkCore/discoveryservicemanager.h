#ifndef DISCOVERYSERVICEMANAGER_H
#define DISCOVERYSERVICEMANAGER_H

#include <QObject>
#include <QReadWriteLock>

#include "discoveryservice.h"

class CDiscoveryServiceManager : public QObject
{
	Q_OBJECT
	/* ================================================================ */
	/* ========================== Attributes ========================== */
	/* ================================================================ */
public:
	typedef enum { srFail = 0, srNoHosts = 1, srGotHosts = 2 } QueryResult;
	typedef std::list< CDiscoveryService*  > CDiscoveryServicesList;

	QReadWriteLock m_pRWLock;

private:
	CDiscoveryServicesList m_lServices;

public:
	/* ================================================================ */
	/* ========================= Construction ========================= */
	/* ================================================================ */
	explicit CDiscoveryServiceManager(QObject *parent = 0);

	/* ================================================================ */
	/* ========================== Operations ========================== */
	/* ================================================================ */
	bool	start();	// connects signals etc.
	bool	stop();		// makes the Discovery Services Manager ready for destruction
	bool	load();
	bool	save();

signals:


public slots:
	void queryService( CDiscoveryService::NetworkType type );


};

extern CDiscoveryServiceManager discoveryManager;

#endif // DISCOVERYSERVICEMANAGER_H
