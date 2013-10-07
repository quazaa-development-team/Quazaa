/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
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

#include "g2packet.h"
#include "systemlog.h"
#include "buffer.h"

#include "debug_new.h"

G2PacketPool G2Packets;

G2Packet::G2Packet()
{
	m_pNext			= 0;
	m_nReference	= 0;

	m_nPosition		= 0;

	m_pBuffer = 0;
	m_nBuffer = 0;
	m_nLength = 0;

	memset(&m_sType[0], 0, sizeof(m_sType));
	m_bCompound = false;
}

G2Packet::~G2Packet()
{
	if(m_nReference != 0)
	{
		systemLog.postLog(LogSeverity::Debug, Components::G2, QString("%1 not released").arg((char*)&m_sType[0]));
	}
	Q_ASSERT(m_nReference == 0);

	if(m_pBuffer)
	{
		free(m_pBuffer);
	}
}

void G2Packet::Reset()
{
	Q_ASSERT(m_nReference == 0);

	m_pNext			= 0;
	m_nLength		= 0;
	m_nPosition		= 0;

	memset(&m_sType[0], 0, sizeof(m_sType));
	m_bCompound = false;
}

void G2Packet::Seek(quint32 nPosition, int nRelative)
{
	if(nRelative == seekStart)
	{
		m_nPosition = qMax(0u, qMin<quint32>(m_nLength, nPosition));
	}
	else
	{
		m_nPosition = qMax(0u, qMin<quint32>(m_nLength, m_nLength - nPosition));
	}
}

uchar* G2Packet::WriteGetPointer(quint32 nLength, quint32 nOffset)
{
	if(nOffset == 0xFFFFFFFF)
	{
		nOffset = m_nLength;
	}

	if(!Ensure(nLength))
	{
		return 0;
	}

	if(nOffset != m_nLength)
	{
		memmove(m_pBuffer + nOffset + nLength, m_pBuffer + nOffset, m_nLength - nOffset);
	}

	m_nLength += nLength;

	return m_pBuffer + nOffset;
}

char* G2Packet::GetType() const
{
	return (char*)&m_sType;
}
void G2Packet::Delete()
{
	G2Packets.Delete(this);
}
G2Packet* G2Packet::New(const char* pszType, bool bCompound)
{
	G2Packet* pPacket = G2Packets.New();

	if(pszType != 0)
	{
		size_t nLength = strlen(pszType);
		strncpy(pPacket->m_sType, pszType, nLength);
		pPacket->m_sType[nLength] = 0;
	}

	pPacket->m_bCompound = bCompound;

	return pPacket;
}
G2Packet* G2Packet::New(char* pSource)
{
	G2Packet* pPacket = New();

	char nInput		= *pSource++;

	char nLenLen	= (nInput & 0xC0) >> 6;
	char nTypeLen	= (nInput & 0x38) >> 3;
	char nFlags	= (nInput & 0x07);

	pPacket->m_bCompound	= (nFlags & G2_FLAG_COMPOUND) ? true : false;
	bool	bBigEndian	= (nFlags & G2_FLAG_BIG_ENDIAN) ? true : false;

	quint32 nLength = 0;

	if(bBigEndian)
	{
		throw packet_error();
	}
	else
	{
		char* pLenOut = (char*)&nLength;
		while(nLenLen--)
		{
			*pLenOut++ = *pSource++;
		}
	}

	nTypeLen++;
	char* pszType = pPacket->m_sType;
	for(; nTypeLen-- ;)
	{
		*pszType++ = *pSource++;
	}
	*pszType++ = 0;

	pPacket->Write(pSource, nLength);

	return pPacket;
}

G2PacketPool::G2PacketPool()
{
	m_pFree = 0;
	m_nFree = 0;
}

G2PacketPool::~G2PacketPool()
{
	Clear();
}

G2Packet* G2Packet::WritePacket(G2Packet* pPacket)
{
	if(pPacket == 0)
	{
		return 0;
	}
	WritePacket(pPacket->m_sType, pPacket->m_nLength, pPacket->m_bCompound);
	Write(pPacket->m_pBuffer, pPacket->m_nLength);
	return this;
}

G2Packet* G2Packet::WritePacket(const char* pszType, quint32 nLength, bool bCompound)
{
	Q_ASSERT(strlen(pszType) > 0);
	Q_ASSERT(nLength <= 0xFFFFFF);

	char nTypeLen	= (char)(strlen(pszType) - 1) & 0x07;
	char nLenLen	= 0;

	if(nLength)
	{
		nLenLen++;
		if(nLength > 0xFF)
		{
			nLenLen++;
			if(nLength > 0xFFFF)
			{
				nLenLen++;
			}
		}
	}

	char nFlags = (nLenLen << 6) + (nTypeLen << 3);

	if(bCompound)
	{
		nFlags |= G2_FLAG_COMPOUND;
	}

	Write(&nFlags, 1);

	Write(&nLength, nLenLen);

	Write(const_cast<char*>(pszType), nTypeLen + 1);

	m_bCompound = true;

	return this;
}

bool G2Packet::ReadPacket(char* pszType, quint32& nLength, bool* pbCompound)
{
	if(GetRemaining() == 0)
	{
		return false;
	}

	char nInput = ReadByte();
	if(nInput == 0)
	{
		return false;
	}

	char nLenLen	= (nInput & 0xC0) >> 6;
	char nTypeLen	= (nInput & 0x38) >> 3;
	char nFlags		= (nInput & 0x07);

	if(GetRemaining() < nTypeLen + nLenLen + 1)
	{
		throw packet_error();
	}

	nLength = 0;
	Read(&nLength, nLenLen);

	if(GetRemaining() < (int)(nLength + nTypeLen + 1))
	{
		throw packet_error();
	}

	Read(pszType, nTypeLen + 1);
	pszType[ nTypeLen + 1 ] = 0;

	if(pbCompound)
	{
		*pbCompound = (nFlags & G2_FLAG_COMPOUND) == G2_FLAG_COMPOUND;
	}
	else
	{
		if(nFlags & G2_FLAG_COMPOUND)
		{
			SkipCompound(nLength);
		}
	}

	return true;
}

bool G2Packet::SkipCompound()
{
	if(m_bCompound)
	{
		quint32 nLength = m_nLength;
		if(! SkipCompound(nLength))
		{
			return false;
		}
	}

	return true;
}

bool G2Packet::SkipCompound(quint32& nLength, quint32 nRemaining)
{
	quint32 nStart	= m_nPosition;
	quint32 nEnd	= m_nPosition + nLength;

	while(m_nPosition < nEnd)
	{
		char nInput = ReadByte();
		if(nInput == 0)
		{
			break;
		}

		char nLenLen	= (nInput & 0xC0) >> 6;
		char nTypeLen	= (nInput & 0x38) >> 3;
		char nFlags	= (nInput & 0x07);
		Q_UNUSED(nFlags);

		if(m_nPosition + nTypeLen + nLenLen + 1 > nEnd)
		{
			throw packet_error();
		}

		quint32 nPacket = 0;

		Read(&nPacket, nLenLen);

		if(m_nPosition + nTypeLen + 1 + nPacket > nEnd)
		{
			throw packet_error();
		}

		m_nPosition += nPacket + nTypeLen + 1;
	}

	nEnd = m_nPosition - nStart;
	if(nEnd > nLength)
	{
		throw packet_error();
	}
	nLength -= nEnd;

	return nRemaining ? nLength >= nRemaining : true;
}

void G2Packet::ToBuffer(CBuffer* pBuffer) const
{
	Q_ASSERT(strlen(m_sType) > 0);

	char nLenLen	= 0;
	if(m_nLength)
	{
		nLenLen++;
		if(m_nLength > 0xFF)
		{
			nLenLen++;
			if(m_nLength > 0xFFFF)
			{
				nLenLen++;
			}
		}
	}

	char nTypeLen	= (char)(strlen(m_sType) - 1) & 0x07;

	char nFlags = (nLenLen << 6) + (nTypeLen << 3);

	if(m_bCompound)
	{
		nFlags |= G2_FLAG_COMPOUND;
	}

	pBuffer->append((char*)&nFlags, 1);

	quint32 nLength = m_nLength;
	pBuffer->append((char*)&nLength, nLenLen);

	pBuffer->append((char*)&m_sType[0], nTypeLen + 1);

	pBuffer->append((char*)m_pBuffer, m_nLength);
}

//////////////////////////////////////////////////////////////////////
// G2Packet buffer stream read

G2Packet* G2Packet::ReadBuffer(CBuffer* pBuffer)
{
	if(pBuffer == 0)
	{
		return 0;
	}

	if(pBuffer->size() < 2)
	{
		return 0;
	}

	char nInput = *pBuffer->data();

	if(nInput == 0)
	{
		pBuffer->remove(0, 1);
		return 0;
	}

	char nLenLen	= (nInput & 0xC0) >> 6;
	char nTypeLen	= (nInput & 0x38) >> 3;
	char nFlags		= (nInput & 0x07);

	if((quint32)pBuffer->size() < (quint32)nLenLen + nTypeLen + 2u)
	{
		return 0;
	}

	quint32 nLength = 0;

	if(nFlags & G2_FLAG_BIG_ENDIAN)
	{
		throw packet_error();
	}
	else
	{
		char* pLenIn	= pBuffer->data() + 1;
		char* pLenOut	= (char*)&nLength;
		for(char nLenCnt = nLenLen ; nLenCnt-- ;)
		{
			*pLenOut++ = *pLenIn++;
		}
	}

	if((quint32)pBuffer->size() < (quint32)nLength + nLenLen + nTypeLen + 2)
	{
		return 0;
	}

	G2Packet* pPacket = G2Packet::New(pBuffer->data());
	pBuffer->remove(0, nLength + nLenLen + nTypeLen + 2u);

	return pPacket;
}

QString G2Packet::ReadString(quint32 nMaximum)
{
	nMaximum = qMin<quint32>(nMaximum, m_nLength - m_nPosition);
	if(!nMaximum)
	{
		return QString();
	}

	const uchar* pInput = m_pBuffer + m_nPosition;
	uchar* pScan = const_cast<uchar*>(pInput);

	quint32 nLength = 0;

	for(; nLength < nMaximum; nLength++)
	{
		m_nPosition++;
		if(! *pScan)
		{
			break;
		}
		pScan++;
	}

	return QString::fromUtf8((char*)pInput, nLength);
}
void G2Packet::WriteString(QString sToWrite, bool bTerminate)
{
	QByteArray baUTF8 = sToWrite.toUtf8();

	Ensure(baUTF8.size() + 1);

	memcpy(m_pBuffer + m_nLength, baUTF8.constData(), baUTF8.size());

	m_nLength += baUTF8.size();

	if(bTerminate)
	{
		m_pBuffer[m_nLength] = 0;
		++m_nLength;
	}
}

QString G2Packet::ToHex() const
{
	const char* pszHex = "0123456789ABCDEF";
	QByteArray strDump;

	strDump.resize(m_nLength * 3);
	char* pszDump = strDump.data();

	for(quint32 i = 0 ; i < m_nLength ; i++)
	{
		int nChar = m_pBuffer[i];
		if(i)
		{
			*pszDump++ = ' ';
		}
		*pszDump++ = pszHex[ nChar >> 4 ];
		*pszDump++ = pszHex[ nChar & 0x0F ];
	}

	*pszDump = 0;

	return strDump;
}

QString G2Packet::ToASCII() const
{
	QByteArray strDump;

	strDump.resize(m_nLength + 1);
	char* pszDump = strDump.data();

	for(uint i = 0 ; i < m_nLength ; i++)
	{
		int nChar = m_pBuffer[i];
		*pszDump++ = (nChar >= 32 ? nChar : '.');
	}

	*pszDump = 0;

	return strDump;
}

bool G2Packet::GetTo(QUuid& pGUID)
{
	if(m_bCompound == false)
	{
		return false;
	}
	if(GetRemaining() < 4 + 16)
	{
		return false;
	}

	uchar* pTest = m_pBuffer + m_nPosition;

	if(pTest[0] != 0x48)
	{
		return false;
	}
	if(pTest[1] != 0x10)
	{
		return false;
	}
	if(pTest[2] != 'T')
	{
		return false;
	}
	if(pTest[3] != 'O')
	{
		return false;
	}

	m_nPosition = 4;
	pGUID = ReadGUID();
	m_nPosition = 0;

	return true;
}

void G2PacketPool::Clear()
{
	for(int nIndex = m_pPools.size() - 1 ; nIndex >= 0 ; nIndex--)
	{
		G2Packet* pPool = (G2Packet*)m_pPools[ nIndex ];
		delete [] pPool;
	}

	m_pPools.clear();;
	m_pFree = 0;
	m_nFree = 0;
}

//////////////////////////////////////////////////////////////////////
// G2PacketPool new pool setup

void G2PacketPool::NewPool()
{
	G2Packet* pPool = 0;
	int nPitch = 0, nSize = 256;

	nPitch	= sizeof(G2Packet);
	pPool	= new G2Packet[ nSize ];

	m_pPools.append(pPool);

	char* pchars = (char*)pPool;

	while(nSize-- > 0)
	{
		pPool = (G2Packet*)pchars;
		pchars += nPitch;

		pPool->m_pNext = m_pFree;
		m_pFree = pPool;
		m_nFree++;
	}
}

G2Packet * G2Packet::AddOrReplaceChild(const char* pFind, G2Packet *pReplacement, bool bRelease, bool bPreserveExtensions)
{
	m_nPosition = 0; // start at zero

	if( !m_bCompound )
	{
		// not a compound packet, just prepend this child
		return PrependPacket(pReplacement, bRelease);
	}

	G2Packet* pNew = G2Packet::New(m_sType, m_bCompound);

	char szType[9];
	quint32 nLength = 0, nNext = 0;
	bool bCompound = false, bFound = false;

	while(ReadPacket(&szType[0], nLength, &bCompound))
	{
		nNext = m_nPosition + nLength;

		if(strcmp(pFind, szType) == 0)
		{
			bFound = true;
			if(!bPreserveExtensions || !bCompound)
			{
				pNew->WritePacket(pReplacement);
			}
			else
			{
				// if this is a compound packet, we need to copy child packets

				quint32 nCompoundLength = nLength;
				uchar* pStart = m_pBuffer + m_nPosition;
				SkipCompound(nLength);
				nCompoundLength -= nLength;

				G2Packet* pReplace = G2Packet::New(pReplacement->m_sType, true);
				pReplace->Write(pStart, nCompoundLength);
				if( !pReplacement->m_bCompound && pReplacement->m_nLength > 0 )
					pReplace->WriteByte(0);
				pReplace->Write(pReplacement->m_pBuffer, pReplacement->m_nLength);
				pNew->WritePacket(pReplace);
				pReplace->Release();
			}
		}
		else
		{
			pNew->WritePacket(szType, nLength, bCompound);
			pNew->Write(m_pBuffer + m_nPosition, nLength);
		}

		m_nPosition = nNext;
	}

	if( !bFound )
	{
		pNew->Release();
		return PrependPacket(pReplacement, bRelease);
	}

	if( GetRemaining() )
	{
		pNew->WriteByte(0);
		pNew->Write(m_pBuffer + m_nPosition, m_nLength - m_nPosition);
	}

	uchar*  pBuff = m_pBuffer;
	quint32 nBuff = m_nBuffer;
	m_pBuffer = pNew->m_pBuffer;
	m_nBuffer = pNew->m_nBuffer;
	pNew->m_pBuffer = pBuff;
	pNew->m_nBuffer = nBuff;
	m_nLength = pNew->m_nLength;
	m_nPosition = 0;
	pNew->Release();
	if( bRelease )
		pReplacement->Release();

	return this;
}

G2Packet * G2Packet::PrependPacket(G2Packet *pPacket, bool bRelease)
{
	if(pPacket == 0)
	{
		return 0;
	}

	quint32 nLength = pPacket->m_nLength;

	Q_ASSERT(strlen((char*)&pPacket->m_sType) > 0);
	Q_ASSERT(nLength <= 0xFFFFFF);

	char nTypeLen	= (char)(strlen((char*)&pPacket->m_sType) - 1) & 0x07;
	char nLenLen	= 0;

	if(nLength)
	{
		nLenLen++;
		if(nLength > 0xFF)
		{
			nLenLen++;
			if(nLength > 0xFFFF)
			{
				nLenLen++;
			}
		}
	}

	char nFlags = (nLenLen << 6) + (nTypeLen << 3);

	if(pPacket->m_bCompound)
	{
		nFlags |= G2_FLAG_COMPOUND;
	}

	quint32 nExpand = 2 + nLenLen + nTypeLen + pPacket->m_nLength + (m_bCompound ? 0 : 1);
	uchar* pWrite = WriteGetPointer(nExpand, 0);
	*pWrite = nFlags;
	pWrite++;
	memcpy(pWrite, &nLength, nLenLen);
	pWrite += nLenLen;
	memcpy(pWrite, &pPacket->m_sType, nTypeLen + 1);
	pWrite += nTypeLen + 1;
	memcpy(pWrite, pPacket->m_pBuffer, pPacket->m_nLength);
	pWrite += pPacket->m_nLength;
	if(!m_bCompound)
		*pWrite = 0;

	m_bCompound = true;

	if( bRelease )
		pPacket->Release();

	return this;
}

QString G2Packet::Dump() const
{
	QString sHex = ToHex();
	QString sAscii = ToASCII();
	QString sRet;

	int nOffset = 0;

	sRet = QString("Type: %1, length: %2, compound: %3\nOffset      Hex                             ASCII\n------------------------------------------------------\n").arg(m_sType).arg(m_nLength).arg(m_bCompound);

	for( ; nOffset < sAscii.length(); nOffset += 10 )
	{
		QString sLine("0x%1  %2  %3\n");

		sRet += sLine.arg(nOffset, 8, 16, QLatin1Char('0')).arg(sHex.mid(nOffset * 3, 10 * 3), -30, QLatin1Char(' ')).arg(sAscii.mid(nOffset, 10));
	}

	return sRet;
}

