//
// routetable.cpp
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

#include "routetable.h"

CRouteTable::CRouteTable()
{
}
CRouteTable::~CRouteTable()
{

}

bool CRouteTable::Add(QUuid& pGUID, CG2Node* pNeighbour, CEndPoint* pEndpoint, bool bNoExpire)
{

	//qDebug() << "CRouteTable::Add " << pGUID << pNeighbour << pEndpoint << bNoExpire;


	if(!pNeighbour && !pEndpoint)
	{
		return false;
	}

	if((quint32)m_lRoutes.size() >= MaxRoutes)
	{
		ExpireOldRoutes(true);
	}

	if(m_lRoutes.contains(pGUID))
	{
		G2RouteItem& pRoute = m_lRoutes[pGUID];

		if(bNoExpire && pNeighbour)
		{
			pRoute.nExpireTime = 0;
		}
		else
		{
			pRoute.nExpireTime = time(0) + RouteExpire;
		}


		if(pNeighbour)
		{
			pRoute.pNeighbour = pNeighbour;
		}
		if(pEndpoint)
		{
			pRoute.pEndpoint = *pEndpoint;
		}

		return true;
	}
	else
	{
		G2RouteItem pRoute;

		if(bNoExpire && pNeighbour)
		{
			pRoute.nExpireTime = 0;
		}
		else
		{
			pRoute.nExpireTime = time(0) + RouteExpire;
		}

		pRoute.pGUID = pGUID;

		if(pNeighbour)
		{
			pRoute.pNeighbour = pNeighbour;
		}
		if(pEndpoint)
		{
			pRoute.pEndpoint = *pEndpoint;
		}

		m_lRoutes[pGUID] = pRoute;

		return true;
	}

	return false;
}
bool CRouteTable::Add(QUuid& pGUID, CG2Node* pNeighbour, bool bNoExpire)
{
	return Add(pGUID, pNeighbour, 0, bNoExpire);
}
bool CRouteTable::Add(QUuid& pGUID, CEndPoint& pEndpoint, bool bNoExpire)
{
	return Add(pGUID, 0, &pEndpoint, bNoExpire);
}

void CRouteTable::Remove(QUuid& pGUID)
{
	m_lRoutes.remove(pGUID);
}
void CRouteTable::Remove(CG2Node* pNeighbour)
{
	for(QHash<QUuid, G2RouteItem>::iterator itRoute = m_lRoutes.begin(); itRoute != m_lRoutes.end();)
	{
		if(itRoute.value().pNeighbour == pNeighbour)
		{
			if(!itRoute.value().pEndpoint.isNull())
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

bool CRouteTable::Find(QUuid& pGUID, CG2Node** ppNeighbour, CEndPoint* pEndpoint)
{
	if(m_lRoutes.contains(pGUID))
	{
		if(ppNeighbour)
		{
			*ppNeighbour = m_lRoutes[pGUID].pNeighbour;
		}
		if(pEndpoint)
		{
			*pEndpoint = m_lRoutes[pGUID].pEndpoint;
		}

		m_lRoutes[pGUID].nExpireTime = time(0) + RouteExpire;

		return true;
	}

	return false;
}

void CRouteTable::ExpireOldRoutes(bool bForce)
{
	quint32 tNow = time(0);

	// najpierw wygasle
	for(QHash<QUuid, G2RouteItem>::iterator itRoute = m_lRoutes.begin(); itRoute != m_lRoutes.end();)
	{
		if(itRoute.value().nExpireTime < tNow)
		{
			itRoute = m_lRoutes.erase(itRoute);
		}
		else
		{
			itRoute++;
		}
	}

	// teraz, jesli jestesmy zmuszeni do wyczyszczenia czegos
	// ale tylko gdy lista jest pelna w 75%

	if(bForce && m_lRoutes.size() >= MaxRoutes * 0.75)
	{
		// redukujemy hash'a do 3/4 jego wartosci
		while(m_lRoutes.size() >= MaxRoutes * 0.75)
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

	systemLog.postLog(LogSeverity::Debug, "----------------------------------");
	//qDebug() << "----------------------------------";
	systemLog.postLog(LogSeverity::Debug, "Dumping routing table:");
	//qDebug() << "Dumping routing table:";
	systemLog.postLog(LogSeverity::Debug, QString("Table size: ").arg(m_lRoutes.size()));
	//qDebug() << "Table size: " << m_lRoutes.size();

	for(QHash<QUuid, G2RouteItem>::iterator itRoute = m_lRoutes.begin(); itRoute != m_lRoutes.end(); itRoute++)
	{
		qint64 nExpire = itRoute.value().nExpireTime - tNow;
		if(itRoute.value().nExpireTime == 0)
		{
			nExpire = 0;
		}
		systemLog.postLog(LogSeverity::Debug, "%s %i %s TTL %i", itRoute.key().toString().toAscii().constData(), itRoute.value().pNeighbour, itRoute.value().pEndpoint.toString().toAscii().constData(), nExpire);
		//qDebug() << itRoute.key().toString().toAscii().constData() << itRoute.value().pNeighbour << itRoute.value().pEndpoint.toString().toAscii().constData() << " TTL " << nExpire;
	}

	systemLog.postLog(LogSeverity::Debug, "End of data");
	//qDebug() << "End of data";
	systemLog.postLog(LogSeverity::Debug, "Dumping routing table:");
	//qDebug() << "----------------------------------";
}
