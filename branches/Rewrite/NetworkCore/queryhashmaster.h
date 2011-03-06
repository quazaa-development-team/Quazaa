/*
** queryhashmaster.h
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
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
