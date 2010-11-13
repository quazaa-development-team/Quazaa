//
// routetable.h
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
