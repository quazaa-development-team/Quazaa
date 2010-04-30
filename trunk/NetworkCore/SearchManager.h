#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>
#include <QList>
#include <QMutex>
#include "types.h"
#include "QueryHit.h"

class CManagedSearch;
class G2Packet;

class CSearchManager : public QObject
{
    Q_OBJECT

public:
    QList<CManagedSearch*> m_lSearches;
    QMutex  m_pSection;
    quint32 m_nPruneCounter;

public:
    CSearchManager(QObject *parent = 0);

    void OnTimer();

    void Add(CManagedSearch* pSearch);
    void Remove(CManagedSearch* pSearch);

    CManagedSearch* Find(QUuid& oGUID);

    // zwraca true jesli pakiet ma byc routowany
    bool OnQueryAcknowledge(G2Packet* pPacket, IPv4_ENDPOINT& addr, QUuid& oGUID);
    bool OnQueryHit(QueryHitSharedPtr pHits);

signals:
    void StatsUpdated(CManagedSearch*);

public slots:

};

extern CSearchManager SearchManager;

#endif // SEARCHMANAGER_H
