#include "queryhashtable.h"
#include <QString>
#include "network.h"
#include "g2node.h"
#include "g2packet.h"

QueryHashTable::QueryHashTable()
{
    m_nHashBits = 20;
    m_nTableSize = ((1u << m_nHashBits) + 31) / 8;
    m_pTable = new char[m_nTableSize];
    Reset();
}
QueryHashTable::~QueryHashTable()
{
    if( m_pTable ) delete[] m_pTable;
}

void QueryHashTable::Add(const char* pSz, const quint32 nLength)
{
    quint32 nHash = HashWord(pSz, nLength, m_nHashBits);
    quint32 nByte = (nHash >> 3);
    quint32 nBit = (nHash & 7);

    m_pTable[ nByte ] &= ~( 1 << nBit );
}

quint32 QueryHashTable::HashWord(const char* pSz, quint32 nLength, qint32 nBits)
{
    quint32 nNumber = 0;
    int nByte = 0;
    for ( ; nLength > 0 ; nLength--, pSz++ )
    {
        int nValue = tolower( *pSz ) & 0xFF;
        nValue = nValue << ( nByte * 8 );
        nByte = ( nByte + 1 ) & 3;
        nNumber = nNumber ^ nValue;
    }
    return HashNumber( nNumber, nBits );
}

quint32 QueryHashTable::HashNumber(quint32 nNumber, qint32 nBits)
{
    quint64 nProduct = (quint64)nNumber * (quint64)0x4F1BBCDC;
    quint64 nHash = ( nProduct << 32 ) >> ( 32 + ( 32 - nBits ) );
    return (quint32)nHash;
}

void QueryHashTable::Reset()
{
    memset(m_pTable, 0xFF, m_nTableSize);
}
void QueryHashTable::AddWord(QByteArray sWord)
{
    Add(sWord.data(), sWord.size());
}

void QueryHashTable::AddPhrase(QString sPhrase)
{
	QStringList keywords;

	if( QueryHashTable::MakeKeywords(sPhrase, keywords) )
	{
		foreach( QString kw, keywords )
		{
			AddWord(kw.toUtf8());
		}
	}
}

void QueryHashTable::PatchTo(CG2Node* pNode)
{
    if( Network.isHub() )
    {
        // tu robienie QHT do wymiany z innymi hubami (jesli hub)
    }

    /*if( pNode->m_pLocalTable == 0 )
    {
        pNode->m_pLocalTable = new QueryHashTable();
    }*/


}
int QueryHashTable::MakeKeywords(QString sPhrase, QStringList &outList)
{
	qDebug() << "Making keywords from:" << sPhrase;

	// remove all garbage from phrase
	sPhrase = sPhrase.replace(QRegExp("\\W"), " ").simplified().toLower();

	// and split it into words
	QStringList lOut;
	lOut = sPhrase.split(" ", QString::SkipEmptyParts);

	// now filter out too short words and only numeric
	QRegExp rx("^\\d+$");
	foreach( QString sWord, lOut )
	{
		// not specs compliant, Shareaza does this too
		if( sWord.length() < 4 )
			continue;

		if( rx.indexIn(sWord) != -1 )
			continue;

		outList.append(sWord);

		if( sWord.length() > 5 )
		{
			outList.append(sWord.left(sWord.length() - 1));
			outList.append(sWord.left(sWord.length() - 2));
		}
	}

	foreach( QString sDebug, outList )
	{
		qDebug() << "Added keyword:" << sDebug;
	}

	return outList.size();
}
