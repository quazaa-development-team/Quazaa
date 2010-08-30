//
// queryhashtable.h
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

#ifndef QUERYHASHTABLE_H
#define QUERYHASHTABLE_H

#include "types.h"
class QString;
class QByteArray;
class CG2Node;

class QueryHashTable
{
protected:
    char*   m_pTable;
    quint32 m_nHashBits;
    quint32 m_nTableSize;

protected:
    void    Add(const char* pSz, const quint32 nLength);
    quint32 HashWord(const char* pSz, const quint32 nLength, qint32 nBits);
    quint32 HashNumber(quint32 nNumber, qint32 nBits);

public:
    QueryHashTable();
    ~QueryHashTable();

    void Reset();
    void AddWord(QByteArray sWord);
	void AddPhrase(QString sPhrase);

    void PatchTo(CG2Node* pNode);

	static int MakeKeywords(QString sPhrase, QStringList& outList);

public:
    inline quint32 HashBits() const
    {
        return m_nHashBits;
    }
    inline quint32 TableSizeBits() const
    {
        return (1u << m_nHashBits);
    }

    inline quint32 TableSize() const
    {
        return TableSizeBits() / 8;
    }
    inline char* TablePointer() const
    {
        return m_pTable;
    }
};

#pragma pack(push,1)

struct G2_QHT_RESET
{
    char    nCmd;       // 0 - reset
    quint32 nTableSize; // 2 ^ size_in_bits
    char    nInfinity;  // 1
};
struct G2_QHT_PATCH
{
    char    nCmd;           // 1 - patch
    char    nFragNum;       //
    char    nFragCount;
    char    nCompression;   // 0 - bez, 1 defl
    char    nBits;          // 1
    //(data)
};

#pragma pack(pop)

#endif // QUERYHASHTABLE_H
