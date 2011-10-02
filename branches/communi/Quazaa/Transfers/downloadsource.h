#ifndef DOWNLOADSOURCE_H
#define DOWNLOADSOURCE_H

#include "types.h"
#include "Hashes/hash.h"
class CQueryHit;

class CDownloadSource
{
public:
	CEndPoint			m_oAddress;		// source address
	bool				m_bPush;		// is it a push source?
	QList<CEndPoint>	m_oPushProxies;	// push proxy, in case we can't connect directly
	TransferProtocol	m_nProtocol;	// protocol
	DiscoveryProtocol	m_nNetwork;		// network
	QUuid				m_oGUID;		// source GUID (needed for pushing)
	QList<CHash>		m_lHashes;		// list of hashes
	time_t				m_tNextAccess;	// seconds since 1970
	quint32				m_nFailures;	// number of failures
	QString				m_sURL;			// URL
public:
	CDownloadSource(CQueryHit* pHit);

public:
	inline bool CanAccess();
};

bool CDownloadSource::CanAccess()
{
	return time(0) > m_tNextAccess;
}

#endif // DOWNLOADSOURCE_H
