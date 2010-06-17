#include "hostcache.h"
#include <QFile>
#include <QDataStream>
#include <QStringList>
#include <QDateTime>
#include "network.h"
#include <time.h>
#include "geoiplist.h"

const quint32 MaxCacheHosts = 1000;

CHostCache HostCache;

void CHostCacheHost::SetKey(quint32 nKey, IPv4_ENDPOINT *pHost)
{
    m_tAck = 0;
    m_nQueryKey = nKey;
    m_nKeyTime = time(0);
    m_nKeyHost = pHost ? pHost->ip : Network.GetLocalAddress().ip;
}

CHostCache::CHostCache()
{
    QFile f("hostcache.dat");

    if( f.exists() && f.open(QFile::ReadOnly) )
    {
        QDataStream s(&f);
        quint16 nVersion;
        s >> nVersion;

        IPv4_ENDPOINT addr;
        quint32 ts;

        while( !f.atEnd() )
        {
            s >> addr.ip >> addr.port >> ts;
            Add(addr, ts);
        }
        f.close();

    }

    IPv4_ENDPOINT e("127.0.0.1:6346");
    Add(e, 0);
    IPv4_ENDPOINT cc("127.0.0.5:6348");
    Add(cc, 0);

    PruneOldHosts();
}
CHostCache::~CHostCache()
{
    Save();

    while( m_lHosts.size() > 0 )
        Remove(m_lHosts.first());
}

CHostCacheHost* CHostCache::Add(IPv4_ENDPOINT host, quint32 ts)
{
    //qDebug() << "CHostCache::Add " << host.toString() << ts;

    if( host.ip == 0 || host.port == 0 )
        return 0;

    if( m_lHosts.size() > 1.5 * MaxCacheHosts )
    {
        while( (quint32)m_lHosts.size() > MaxCacheHosts / 2 )
            delete m_lHosts.takeLast();
    }

    if( ts == 0 )
        ts = time(0) - 60;

    CHostCacheHost* pNew = Find(host);

    if( pNew && pNew->m_tTimestamp < ts )
    {
        Update(pNew);
        return pNew;
    }

    pNew = new CHostCacheHost();
    pNew->m_oAddress = host;
    pNew->m_tTimestamp = ts;

    bool bAdded = false;

    for( qint32 i = 0; i < m_lHosts.size(); i++ )
    {
        if( m_lHosts.at(i)->m_tTimestamp < ts )
        {
            m_lHosts.insert(i, pNew);
            bAdded = true;
            break;
        }
    }

    if( !bAdded )
        m_lHosts.append(pNew);

    return pNew;

}

void CHostCache::AddXTry(QString &sHeader)
{
    // X-Try-Hubs: 86.141.203.14:6346 2010-02-23T16:17Z,91.78.12.117:1164 2010-02-23T16:17Z,89.74.83.103:7972 2010-02-23T16:17Z,93.89.196.113:5649 2010-02-23T16:17Z,24.193.237.252:6346 2010-02-23T16:17Z,24.226.149.80:6346 2010-02-23T16:17Z,89.142.217.180:9633 2010-02-23T16:17Z,83.219.112.111:6346 2010-02-23T16:17Z,201.17.187.205:6346 2010-02-23T16:17Z,213.29.19.41:6346 2010-02-23T16:17Z,78.231.224.180:6346 2010-02-23T16:17Z,213.143.88.92:6346 2010-02-23T16:17Z,77.209.25.104:1515 2010-02-23T16:17Z,86.220.168.24:59153 2010-02-23T16:17Z,88.183.80.110:6346 2010-02-23T16:17Z

    QStringList l = sHeader.split(",");

    if( l.isEmpty() )
        return;

    for( qint32 i = 0; i < l.size(); i++ )
    {
        QStringList le = l.at(i).split(" ");
        if( le.size() != 2 )
            continue;

        IPv4_ENDPOINT addr(le.at(0));
        if( addr.ip == 0 || addr.port == 0 )
            continue;

        Add(addr, time(0));
    }
}
QString CHostCache::GetXTry()
{
    const quint32 nMax = 10;

    QString sRet;
    quint32 nCount = 0;

    if( m_lHosts.size() == 0 )
        return QString();

    while( nCount < nMax )
    {
        if( (quint32)m_lHosts.size() > nCount )
        {
            CHostCacheHost* pHost = m_lHosts.at(nCount);
            sRet.append(pHost->m_oAddress.toString() + " ");

            sRet.append(QDateTime::fromTime_t(pHost->m_tTimestamp).toString("yyyy-MM-ddThh:mm:ssZ"));
            sRet.append(",");

        }
        else
        {
            break;
        }
        nCount++;
    }

    if( sRet.isEmpty() )
        return QString();

    return "X-Try-Hubs: " + sRet.left(sRet.size() - 1) + "\r\n";
}

void CHostCache::Update(IPv4_ENDPOINT oHost)
{
    CHostCacheHost* pHost = Find(oHost);

    if( pHost )
        Update(pHost);
}

void CHostCache::Update(CHostCacheHost *pHost)
{
    int nIndex = m_lHosts.indexOf(pHost);

    if( nIndex == -1)
        return;

    m_lHosts.removeAt(nIndex);
    pHost->m_tTimestamp = time(0);
    m_lHosts.prepend(pHost);
}

void CHostCache::Remove(CHostCacheHost* pRemove)
{
    for( qint32 i = 0; i < m_lHosts.size(); i++ )
    {
        if( m_lHosts.at(i) == pRemove )
        {
            delete pRemove;
            m_lHosts.removeAt(i);
        }
    }
}

CHostCacheHost* CHostCache::Find(IPv4_ENDPOINT oHost)
{
    CHostCacheHost* pFind = 0;

    for( int i = 0; i < m_lHosts.size(); i++ )
    {
        if( m_lHosts.at(i)->m_oAddress == oHost )
        {
            pFind = m_lHosts[i];
            break;
        }
    }

    return pFind;
}

void CHostCache::OnFailure(IPv4_ENDPOINT addr)
{
    foreach( CHostCacheHost* pHost, m_lHosts )
    {
        if( pHost->m_oAddress == addr )
        {
            Remove(pHost);
            break;
        }
    }
}

void CHostCache::Save()
{
    QFile f("hostcache.dat");

    if( f.open(QFile::WriteOnly) )
    {
        QDataStream s(&f);
        quint16 nVersion = 1;
        s << nVersion;

        if( !m_lHosts.isEmpty() )
        {
            foreach( CHostCacheHost* pHost, m_lHosts )
            {
                s << pHost->m_oAddress.ip << pHost->m_oAddress.port << pHost->m_tTimestamp;
            }
        }

        f.close();
    }
}

CHostCacheHost* CHostCache::Get()
{
    CHostCacheHost* pHost = 0;

    if( m_lHosts.size() == 0 )
        return pHost;

    pHost = m_lHosts.first();
    m_lHosts.removeFirst();

    return pHost;
}
CHostCacheHost* CHostCache::GetConnectable(quint32 tNow, QString sCountry)
{
	bool bCountry = (sCountry != "ZZ");

    if( tNow == 0 )
        tNow = time(0);

    if( m_lHosts.isEmpty() )
        return 0;

    foreach( CHostCacheHost* pHost, m_lHosts )
    {
		if( bCountry )
			if( sCountry != GeoIP.findCountryCode(pHost->m_oAddress) )
				continue;
        if( tNow - pHost->m_tLastConnect > ReconnectTime )
            return pHost;
    }

    return 0;
}

void CHostCache::PruneOldHosts()
{
    quint32 tNow = time(0);

    for( int i = 0; i < m_lHosts.size(); )
    {
        if( tNow - m_lHosts[i]->m_tTimestamp > 86400 )
        {
            m_lHosts.removeAt(i);
        }
        else
        {
            i++;
        }
    }
}

void CHostCache::PruneByQueryAck()
{
    quint32 tNow = time(0);

    for( int i = 0; i < m_lHosts.size(); )
    {
        if( m_lHosts[i]->m_tAck && tNow - m_lHosts[i]->m_tAck > 600 )
        {
            m_lHosts.removeAt(i);
        }
        else
        {
            i++;
        }
    }
}
