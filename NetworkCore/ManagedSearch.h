#ifndef MANAGEDSEARCH_H
#define MANAGEDSEARCH_H

#include <QObject>
#include <QHash>

#include "QueryHit.h"

class CQuery;
//class CQueryHit;

class CManagedSearch : public QObject
{
    Q_OBJECT
public:
    quint32 m_tStarted;
    bool    m_bActive;
    bool    m_bPaused;
    CQuery* m_pQuery;

    bool    m_bCanRequestKey;

    quint32 m_nHubs;
    quint32 m_nLeaves;
    quint32 m_nHits;
    quint32 m_nQueryCount;

    QUuid   m_oGUID;

    QHash<quint32, quint32> m_lSearchedNodes;

	quint32	m_nCookie;

public:
    CManagedSearch(CQuery* pQuery, QObject *parent = 0);
    ~CManagedSearch();

    void Start();
    void Stop();
    void Pause();

    void Execute(quint32 tNow, quint32* pnMaxPackets);
    void SearchNeighbours(quint32 tNow);
    void SearchG2(quint32 tNow, quint32* pnMaxPackets);

    void OnHostAcknowledge(quint32 nHost, quint32 tNow);
    void OnQueryHit(QueryHitSharedPtr pHits);

signals:
    void OnHit(QueryHitSharedPtr);
	void StatsUpdated();

public slots:

	friend class CSearchManager;
};

#endif // MANAGEDSEARCH_H
