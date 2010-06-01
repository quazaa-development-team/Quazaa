#ifndef QUERYHIT_H
#define QUERYHIT_H

#include "types.h"
#include <QList>
#include <QSharedPointer>

#include "Hashes/sha1.h"

class G2Packet;
class CQuery;

struct QueryHitInfo
{
    IPv4_ENDPOINT   m_oNodeAddress;
    QUuid           m_oGUID;
    QUuid           m_oNodeGUID;
    QList<IPv4_ENDPOINT>    m_lNeighbouringHubs;
    quint8          m_nHops;
	QString			m_sVendor;

	QueryHitInfo()
	{
		m_sVendor = "";
	}

    ~QueryHitInfo()
    {
        qDebug("hit info deleted");
    }
};

class CQueryHit
{
public:
    CQueryHit*      m_pNext;

    QSharedPointer<QueryHitInfo>   m_pHitInfo;

    CSHA1           m_oSha1;
    QString         m_sDescriptiveName;
    QString         m_sURL;
    QString         m_sMetadata;
    quint64         m_nObjectSize;
    quint32         m_nCachedSources;
    bool            m_bIsPartial;
    quint64         m_nPartialBytesAvailable;
    QString         m_sPreviewURL;

    bool            m_bIsP2PIMCapable;

public:
    CQueryHit();
    ~CQueryHit();

	static CQueryHit* ReadPacket(G2Packet* pPacket, QSharedPointer<QueryHitInfo> pHitInfo);

    void Delete();
    void ResolveURLs();
    bool IsValid(CQuery* pQuery = 0);
};

typedef class QSharedPointer<CQueryHit> QueryHitSharedPtr;

#endif // QUERYHIT_H
