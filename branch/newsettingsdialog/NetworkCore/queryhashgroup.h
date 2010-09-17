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

#ifndef QUERYHASHGROUP_H
#define QUERYHASHGROUP_H

#include <QtGlobal>
#include <QList>

class CQueryHashTable;

class CQueryHashGroup
{
public:
	CQueryHashGroup(quint32 nHash = 0);
	virtual ~CQueryHashGroup();

public:
	uchar*		m_pHash;
	quint32		m_nHash;
	quint32		m_nCount;
protected:
	QList< CQueryHashTable* > m_pTables;

public:
	void	Add(CQueryHashTable* pTable);
	void	Remove(CQueryHashTable* pTable);
protected:
	void	Operate(CQueryHashTable* pTable, bool nAdd);

public:
	int		GetCount()
	{
		return m_pTables.size();
	}
};
#endif // QUERYHASHGROUP_H
