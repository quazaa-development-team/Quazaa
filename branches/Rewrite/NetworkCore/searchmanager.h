/*
** searchmanager.h
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

#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>
#include <QHash>
#include <QMutex>
#include "types.h"
#include "queryhit.h"

class CManagedSearch;
class G2Packet;
class CG2Node;

class CSearchManager : public QObject
{
	Q_OBJECT

public:
	QHash<QUuid, CManagedSearch*> m_lSearches;
	QMutex  m_pSection;
	quint32 m_nPruneCounter;
	quint32 m_nCookie;

public:
	CSearchManager(QObject* parent = 0);

	void OnTimer();

	void Add(CManagedSearch* pSearch);
	void Remove(CManagedSearch* pSearch);

	CManagedSearch* Find(QUuid& oGUID);

	// zwraca true jesli pakiet ma byc routowany
	bool OnQueryAcknowledge(G2Packet* pPacket, CEndPoint& addr, QUuid& oGUID);
	bool OnQueryHit(G2Packet* pPacket, QueryHitInfo* pHitInfo);

signals:

public slots:

};

extern CSearchManager SearchManager;

#endif // SEARCHMANAGER_H
