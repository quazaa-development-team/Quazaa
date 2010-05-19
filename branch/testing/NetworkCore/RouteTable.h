#ifndef ROUTETABLE_H
#define ROUTETABLE_H

#include "types.h"
#include <QHash>

class CG2Node;

struct G2RouteItem
{
    QUuid           pGUID;
    CG2Node*        pNeighbour;
    IPv4_ENDPOINT   pEndpoint;
    quint32         nExpireTime;

    G2RouteItem()
    {
        pNeighbour = 0;
        nExpireTime = 0;
    }

};

class CRouteTable
{
protected:
    QHash<QUuid, G2RouteItem>  m_lRoutes;
public:
    CRouteTable();
    ~CRouteTable();

    bool Add(QUuid& pGUID, CG2Node* pNeighbour = 0, IPv4_ENDPOINT* pEndpoint = 0, bool bNoExpire = false);
    bool Add(QUuid& pGUID, CG2Node* pNeighbour, bool bNoExpire = false);
    bool Add(QUuid& pGUID, IPv4_ENDPOINT& pEndpoint, bool bNoExpire = false);

    void Remove(QUuid& pGUID);
    void Remove(CG2Node* pNeighbour);

    bool Find(QUuid& pGUID, CG2Node** ppNeighbour = 0, IPv4_ENDPOINT* pEndpoint = 0);

    void ExpireOldRoutes(bool bForce = false);
    void Clear();

    void Dump();
};

const quint32 MaxRoutes = 20000;
const quint32 RouteExpire = 600;
#endif // ROUTETABLE_H
