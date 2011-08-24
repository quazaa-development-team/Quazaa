/*
** queryhashtable.h
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

#ifndef QUERYHASHTABLE_H
#define QUERYHASHTABLE_H

#include "types.h"
#include <QObject>
class QString;
class QByteArray;
class CG2Node;
class G2Packet;
class QByteArray;
class CBuffer;
class CQueryHashGroup;
class CQuery;

typedef QSharedPointer<CQuery> CQueryPtr;

class CQueryHashTable : public QObject
{
	Q_OBJECT
public:
	CQueryHashTable();
	virtual ~CQueryHashTable();

public:
	bool				m_bLive;
	quint32				m_nCookie;
	uchar*				m_pHash;
	quint32				m_nHash;
	quint32				m_nBits;
	quint32				m_nInfinity;
	quint32				m_nCount;
	CBuffer*			m_pBuffer;
	CQueryHashGroup* 	m_pGroup;

public:
	static quint32 HashWord(const char* pSz, const quint32 nLength, qint32 nBits);
	static quint32 HashNumber(quint32 nNumber, qint32 nBits);

public:
	static int MakeKeywords(QString sPhrase, QStringList& outList);

public:
	void	Create();
	void	Clear();
	bool	Merge(const CQueryHashTable* pSource);
	bool	Merge(const CQueryHashGroup* pSource);
	bool	PatchTo(const CQueryHashTable* pTarget, CG2Node* pNeighbour);
	bool	OnPacket(G2Packet* pPacket);
	void	AddString(const QString& strString);
	void	AddExactString(const QString& strString);
	void	AddWord(const QByteArray& sWord);
	bool	CheckString(const QString& strString) const;
	bool	CheckHash(const quint32 nHash) const;
	bool	CheckQuery(CQueryPtr pQuery);
	int		GetPercent() const;
protected:
	bool	OnReset(G2Packet* pPacket);
	bool	OnPatch(G2Packet* pPacket);
	void	Add(const char* pszString, size_t nLength);
	void	AddExact(const char* pszString, size_t nLength);
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
