#include "queryhashtable.h"
#include <QString>
#include "network.h"
#include "g2node.h"
#include "g2packet.h"
#include "ZLibUtils.h"
#include <QByteArray>

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
	Q_ASSERT(pNode->m_nType == G2_HUB);

    if( Network.isHub() )
    {
        // tu robienie QHT do wymiany z innymi hubami (jesli hub)
    }

	bool bReset = false;

	if( pNode->m_pLocalTable == 0 )
    {
        pNode->m_pLocalTable = new QueryHashTable();
		bReset = true;
	}

	if( bReset )
	{
		G2_QHT_RESET reset;
		memset(&reset, 0, sizeof(G2_QHT_RESET));

		reset.nCmd = 0;
		reset.nInfinity = 1;
		reset.nTableSize = TableSizeBits();

		G2Packet* pReset = G2Packet::New("QHT");
		pReset->Write((void*)&reset, sizeof(G2_QHT_RESET));
		pNode->m_lSendQueue.enqueue(pReset);

		qDebug() << "Sending QHT reset to " << pNode->m_oAddress.toString();
	}

	quint32* pThis = reinterpret_cast<quint32*>(m_pTable);
	quint32* pThat = reinterpret_cast<quint32*>(pNode->m_pLocalTable->m_pTable);

	int nUints = TableSize() / 4;

	for( int i = 0; i < nUints; i++ )
	{
		quint32 nTemp = *pThis;
		*pThat ^= *pThis;
		//*pThat = nTemp;
		pThis++;
		pThat++;
	}

	quint32 nSize = TableSize();
	char* pTable = pNode->m_pLocalTable->m_pTable;
	bool bCompressed = false;

	QByteArray ar;
	if( !pNode->m_bCompressedOutput )
	{
		// compress the table if connection is not compressed

		bCompressed = true;
		ar.append(pTable, pNode->m_pLocalTable->TableSize());

		ZLibUtils::Compress(ar);

		pTable = ar.data();
		nSize = ar.size();
	}


	quint32 nFrags = 1;
	const quint32 nFragSize = 4096;
	quint32 nToWrite = TableSize();

	while( nSize > nFrags * nFragSize )
		nFrags++;

	quint32 nOffset = 0;
	for( char nFrag = 1; nFrag <= nFrags; nFrag++ )
	{
		G2_QHT_PATCH p;
		p.nCmd = 1;
		p.nBits = 1;
		p.nCompression = (bCompressed ? 1 : 0);
		p.nFragCount = nFrags;
		p.nFragNum = nFrag;

		G2Packet* pPatch = G2Packet::New("QHT");
		pPatch->Write((void*)&p, sizeof(p));

		quint32 nFs = qMin(nToWrite, nFragSize);

		pPatch->Write((void*)(pTable + nOffset), nFs);

		nOffset += nFs;
		nToWrite -= nFs;
		pNode->m_lSendQueue.enqueue(pPatch);
	}

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
