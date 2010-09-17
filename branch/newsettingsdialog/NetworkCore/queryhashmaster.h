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

#ifndef QUERYHASHMASTER_H
#define QUERYHASHMASTER_H

#include "queryhashtable.h"

class CQueryHashGroup;

class CQueryHashMaster : public CQueryHashTable
{
	Q_OBJECT
public:
	CQueryHashMaster();
	virtual ~CQueryHashMaster();

protected:
	QList< CQueryHashGroup* > m_pGroups;
	int			m_nPerGroup;
	bool		m_bValid;

public:
	void		Create();
	void		Add(CQueryHashTable* pTable);
	void		Remove(CQueryHashTable* pTable);
public slots:
	void		Build();

public:

	inline int GetCount() const
	{
		return m_pGroups.size();
	}

	inline void Invalidate()
	{
		m_bValid = false;
	}
	inline bool IsValid()
	{
		return m_bValid;
	}
};

extern CQueryHashMaster QueryHashMaster;

#endif // QUERYHASHMASTER_H
