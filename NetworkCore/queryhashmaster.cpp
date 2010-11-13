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

#include "queryhashmaster.h"
#include "queryhashgroup.h"
#include "ShareManager.h"
#include <QDateTime>

CQueryHashMaster QueryHashMaster;


CQueryHashMaster::CQueryHashMaster()
{
	m_nPerGroup = 0;
}

CQueryHashMaster::~CQueryHashMaster()
{
	Q_ASSERT(GetCount() == 0);
}

void CQueryHashMaster::Create()
{
	CQueryHashTable::Create();

	m_nPerGroup			= 100;
	m_bValid			= false;
	m_bLive				= false;
	m_nCookie			= 0;
}

void CQueryHashMaster::Add(CQueryHashTable* pTable)
{
	Q_ASSERT(m_nPerGroup > 0);
	Q_ASSERT(pTable != 0);
	Q_ASSERT(pTable->m_nHash > 0);
	Q_ASSERT(pTable->m_pGroup == 0);

	for(QList<CQueryHashGroup*>::iterator itGroup = m_pGroups.begin(); itGroup != m_pGroups.end(); itGroup++)
	{
		CQueryHashGroup* pGroup = *itGroup;

		if(pGroup->m_nHash == pTable->m_nHash &&
		        pGroup->GetCount() < m_nPerGroup)
		{
			pGroup->Add(pTable);
			m_bValid = false;
			return;
		}
	}

	CQueryHashGroup* pGroup = new CQueryHashGroup(pTable->m_nHash);
	m_pGroups.append(pGroup);
	pGroup->Add(pTable);
	m_bValid = false;
}

void CQueryHashMaster::Remove(CQueryHashTable* pTable)
{
	Q_ASSERT(pTable != 0);
	if(pTable->m_pGroup == 0)
	{
		return;
	}

	CQueryHashGroup* pGroup = pTable->m_pGroup;
	pGroup->Remove(pTable);

	if(pGroup->GetCount() == 0)
	{
		int pos = m_pGroups.indexOf(pGroup);
		Q_ASSERT(pos >= 0);
		m_pGroups.removeAt(pos);
		delete pGroup;
	}

	m_bValid = false;
}

void CQueryHashMaster::Build()
{
	quint32 tNow = time(0);

	if(m_bValid)
	{
		if(tNow - m_nCookie < 600)
		{
			return;
		}
	}
	else
	{
		if(tNow - m_nCookie < 20)
		{
			return;
		}
	}

	QMutexLocker l(&ShareManager.m_oSection);

	const CQueryHashTable* pLocalTable = ShareManager.GetHashTable();

	if(!pLocalTable)
	{
		return;
	}

	Clear();
	Merge(pLocalTable);

	for(QList<CQueryHashGroup*>::iterator itGroup = m_pGroups.begin(); itGroup != m_pGroups.end(); itGroup++)
	{
		CQueryHashGroup* pGroup = *itGroup;
		Merge(pGroup);
	}


	m_bValid	= true;
	m_bLive		= true;
	m_nCookie	= tNow;
}
