//
// SearchManager.h
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
	QHash<QUuid,CManagedSearch*> m_lSearches;
    QMutex  m_pSection;
    quint32 m_nPruneCounter;
	quint32 m_nCookie;

public:
    CSearchManager(QObject *parent = 0);

    void OnTimer();

    void Add(CManagedSearch* pSearch);
    void Remove(CManagedSearch* pSearch);

    CManagedSearch* Find(QUuid& oGUID);

    // zwraca true jesli pakiet ma byc routowany
    bool OnQueryAcknowledge(G2Packet* pPacket, IPv4_ENDPOINT& addr, QUuid& oGUID);
	void OnQueryHit(G2Packet* pPacket, CG2Node* pNode = 0, IPv4_ENDPOINT* pEndpoint = 0);

signals:

public slots:

};

extern CSearchManager SearchManager;

#endif // SEARCHMANAGER_H
