#include "RouteTable.h"

CRouteTable::CRouteTable()
{
}
CRouteTable::~CRouteTable()
{

}

bool CRouteTable::Add(QUuid &pGUID, CG2Node *pNeighbour, IPv4_ENDPOINT *pEndpoint, bool bNoExpire)
{

	//qDebug() << "CRouteTable::Add " << pGUID << pNeighbour << pEndpoint << bNoExpire;


    if( !pNeighbour && !pEndpoint )
        return false;

    if( (quint32)m_lRoutes.size() >= MaxRoutes )
        ExpireOldRoutes(true);

    if( m_lRoutes.contains(pGUID) )
    {
        G2RouteItem &pRoute = m_lRoutes[pGUID];

        if( bNoExpire && pNeighbour )
        {
            pRoute.nExpireTime = 0;
        }
        else
        {
            pRoute.nExpireTime = time(0) + RouteExpire;
        }


        if( pNeighbour )
            pRoute.pNeighbour = pNeighbour;
        if( pEndpoint )
            pRoute.pEndpoint = *pEndpoint;

        return true;
    }
    else
    {
        G2RouteItem pRoute;

        if( bNoExpire && pNeighbour )
        {
            pRoute.nExpireTime = 0;
        }
        else
        {
            pRoute.nExpireTime = time(0) + RouteExpire;
        }

        pRoute.pGUID = pGUID;

        if( pNeighbour )
            pRoute.pNeighbour = pNeighbour;
        if( pEndpoint )
            pRoute.pEndpoint = *pEndpoint;

        m_lRoutes[pGUID] = pRoute;

        return true;
    }

    return false;
}
bool CRouteTable::Add(QUuid &pGUID, CG2Node *pNeighbour, bool bNoExpire)
{
    return Add(pGUID, pNeighbour, 0, bNoExpire);
}
bool CRouteTable::Add(QUuid &pGUID, IPv4_ENDPOINT &pEndpoint, bool bNoExpire)
{
    return Add(pGUID, 0, &pEndpoint, bNoExpire);
}

void CRouteTable::Remove(QUuid &pGUID)
{
    m_lRoutes.remove(pGUID);
}
void CRouteTable::Remove(CG2Node *pNeighbour)
{
    for( QHash<QUuid, G2RouteItem>::iterator itRoute = m_lRoutes.begin(); itRoute != m_lRoutes.end(); )
    {
        if( itRoute.value().pNeighbour == pNeighbour )
        {
            if( itRoute.value().pEndpoint.ip == 0 )
            {
                itRoute = m_lRoutes.erase(itRoute);
            }
            else
            {
                itRoute.value().pNeighbour = 0;
                itRoute.value().nExpireTime = time(0) + RouteExpire;
                itRoute++;
            }
        }
        else
        {
            itRoute++;
        }
    }
}

bool CRouteTable::Find(QUuid &pGUID, CG2Node** ppNeighbour, IPv4_ENDPOINT *pEndpoint)
{
    if( m_lRoutes.contains(pGUID) )
    {
        if( ppNeighbour )
            *ppNeighbour = m_lRoutes[pGUID].pNeighbour;
        if( pEndpoint )
            *pEndpoint = m_lRoutes[pGUID].pEndpoint;

        m_lRoutes[pGUID].nExpireTime = time(0) + RouteExpire;

        return true;
    }

    return false;
}

void CRouteTable::ExpireOldRoutes(bool bForce)
{
    quint32 tNow = time(0);

    // najpierw wygasle
    for( QHash<QUuid, G2RouteItem>::iterator itRoute = m_lRoutes.begin(); itRoute != m_lRoutes.end(); )
    {
        if( itRoute.value().nExpireTime < tNow )
            itRoute = m_lRoutes.erase(itRoute);
        else
            itRoute++;
    }

    // teraz, jesli jestesmy zmuszeni do wyczyszczenia czegos
    // ale tylko gdy lista jest pelna w 75%

    if( bForce && m_lRoutes.size() >= MaxRoutes * 0.75 )
    {
        // redukujemy hash'a do 3/4 jego wartosci
        while( m_lRoutes.size() >= MaxRoutes * 0.75 )
        {
			m_lRoutes.erase(m_lRoutes.begin());
        }
    }
}

void CRouteTable::Clear()
{
    m_lRoutes.clear();
}

void CRouteTable::Dump()
{

    quint32 tNow = time(0);

    qDebug() << "----------------------------------";
    qDebug() << "Dumping routing table:";
    qDebug() << "Table size: " << m_lRoutes.size();

    for(QHash<QUuid,G2RouteItem>::iterator itRoute = m_lRoutes.begin(); itRoute != m_lRoutes.end(); itRoute++ )
    {
        qint64 nExpire = itRoute.value().nExpireTime - tNow;
        if( itRoute.value().nExpireTime == 0 )
            nExpire = 0;
        qDebug() << itRoute.key().toString().toAscii().constData() << itRoute.value().pNeighbour << itRoute.value().pEndpoint.toString().toAscii().constData() << " TTL " << nExpire;
    }

    qDebug() << "End of data";
    qDebug() << "----------------------------------";
}
