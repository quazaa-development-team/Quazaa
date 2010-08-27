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
