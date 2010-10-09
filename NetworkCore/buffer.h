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

#ifndef BUFFER_H
#define BUFFER_H

#include "types.h"
class QByteArray;

class CBuffer
{
protected:
	char*	m_pBuffer;	// allocated block
	quint32	m_nLength;	// length of data it stores
	quint32	m_nBuffer;	// length of this block
	quint32 m_nMinimum;

// inlines
public:
	inline char* data()
	{
		return m_pBuffer;
	}
	inline quint32 size() const
	{
		return m_nLength;
	}
	inline quint32 capacity() const
	{
		return m_nBuffer;
	}
	inline bool isEmpty() const
	{
		return (m_nLength == 0);
	}

	inline void setMinimumSize(quint32 nSize)
	{
		m_nMinimum = nSize;
	}

	inline CBuffer& clear()
	{
		m_nLength = 0;
		return *this;
	}

public:
	CBuffer(quint32 nMinimum = 1024u);
	~CBuffer();

	CBuffer& append(const void* pData, const quint32 nLength);
	CBuffer& append(const char* pStr);
	CBuffer& append(QByteArray& baData);
	CBuffer& append(CBuffer& pOther);

	CBuffer& prepend(const void* pData, const quint32 nLength);
	CBuffer& prepend(const char* pStr);

	CBuffer& insert(const quint32 i, const void* pData, const quint32 nLength);
	CBuffer& insert(const quint32 i, const char* pStr);

	CBuffer& remove(const quint32 nPos, const quint32 nLength);
	CBuffer& remove(const quint32 nLength);

	void	 ensure(const quint32 nLength);

	void	 resize(const quint32 nLength);

};

#endif // BUFFER_H
