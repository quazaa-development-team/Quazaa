//
// managedsearch.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef MANAGEDSEARCH_H
#define MANAGEDSEARCH_H

#include <QObject>
#include <QHash>

#include "queryhit.h"

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

	QHash<quint32, quint32>		m_lSearchedNodes;

	CQueryHit*					m_pCachedHit;
	quint32						m_nCachedHits;

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
	void OnQueryHit(CQueryHit* pHits);
	void SendHits();

signals:
    void OnHit(QueryHitSharedPtr);
	void StatsUpdated();
	void StateChanged();

public slots:

	friend class CSearchManager;
};

#endif // MANAGEDSEARCH_H
