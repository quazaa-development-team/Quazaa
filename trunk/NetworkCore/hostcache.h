#ifndef HOSTCACHE_H
#define HOSTCACHE_H

#include "types.h"
#include <QList>

const quint32 ReconnectTime = 3600;

class CHostCacheHost
{
public:
    IPv4_ENDPOINT   m_oAddress;     // Adres huba
    quint32         m_tTimestamp;   // Kiedy ostatnio widziany

    quint32         m_nQueryKey;    // QK
    quint32         m_nKeyHost;     // host dla ktorego jest QK
    quint32         m_nKeyTime;     // kiedy odebrano OK?

    quint32         m_tAck;         // czas ostatniej operacji wymagajacej potwierdzenia

    quint32         m_tLastQuery;   // kiedy poslano ostatnie zapytanie?
    quint32         m_tRetryAfter;  // kiedy mozna ponowic?
    quint32         m_tLastConnect; // kiedy ostatnio sie polaczylismy?

public:
    CHostCacheHost()
    {
        m_tTimestamp = 0;

        m_nQueryKey = 0;
        m_nKeyHost = 0;
        m_nKeyTime = 0;

        m_tRetryAfter = 0;
        m_tAck = 0;

        m_tLastQuery = 0;
        m_tLastConnect = 0;
    }

    bool CanQuery(quint32 tNow = 0)
    {
        if( tNow == 0 )
            tNow = time(0);

        if( m_tAck != 0 )
            return false;

        if( tNow - m_tTimestamp > 3600 )
            return false;

        if( m_tRetryAfter != 0 && tNow < m_tRetryAfter )
            return false;

        if( m_tLastQuery == 0 )
            return true;

        return (tNow - m_tLastQuery) >= 120;
    }

    void SetKey(quint32 nKey, IPv4_ENDPOINT* pHost = 0);
};

class CHostCache
{

//protected:
public:
    QList<CHostCacheHost*>  m_lHosts;

public:
    CHostCache();
    ~CHostCache();

    CHostCacheHost* Add(IPv4_ENDPOINT host, quint32 ts);
    void AddXTry(QString & sHeader);
    QString GetXTry();
    void Update(IPv4_ENDPOINT oHost);
    void Update(CHostCacheHost* pHost);
    void Remove(CHostCacheHost* pRemove);
    CHostCacheHost* Find(IPv4_ENDPOINT oHost);
    void OnFailure(IPv4_ENDPOINT addr);
    CHostCacheHost* Get();
	CHostCacheHost* GetConnectable(quint32 tNow = 0, QString sCountry = QString("ZZ"));

    void Save();

    void PruneOldHosts();
    void PruneByQueryAck();

    inline quint32 size()
    {
        return m_lHosts.size();
    }
    inline bool isEmpty()
    {
        return (size() == 0);
    }
};

extern CHostCache HostCache;

#endif // HOSTCACHE_H
