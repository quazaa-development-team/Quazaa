/*
** managedsearch.h
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

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

	QHash<QHostAddress, quint32>		m_lSearchedNodes;

	CQueryHit*					m_pCachedHit;
	quint32						m_nCachedHits;

	quint32	m_nCookie;

public:
	CManagedSearch(CQuery* pQuery, QObject* parent = 0);
	~CManagedSearch();

	void Start();
	void Stop();
	void Pause();

	void Execute(quint32 tNow, quint32* pnMaxPackets);
	void SearchNeighbours(quint32 tNow);
	void SearchG2(quint32 tNow, quint32* pnMaxPackets);

	void OnHostAcknowledge(QHostAddress nHost, quint32 tNow);
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
