//
// g2packet.h
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

#ifndef G2PACKET_H
#define G2PACKET_H

#include "types.h"
#include <QtGlobal>
#include <QMutex>
#include <QList>

struct packet_error {};
struct packet_read_past_end {};

class CBuffer;

class G2Packet
{
	// Construction
protected:
	G2Packet();
	~G2Packet();

public:
	static G2Packet* New(const char* pszType = 0, bool bCompound = false);
	static G2Packet* New(char* pSource);


	// Attributes
public:
	G2Packet* 	m_pNext;
	quint32		m_nReference;
public:
	uchar*		m_pBuffer;
	quint32		m_nBuffer;
	quint32		m_nLength;
	quint32		m_nPosition;
	char		m_sType[9];
	bool		m_bCompound;

	enum { seekStart, seekEnd };

	// Operations
public:
	void	Reset();
	void	Seek(quint32 nPosition, int nRelative = seekStart);
	uchar* 	WriteGetPointer(quint32 nLength, quint32 nOffset = 0xFFFFFFFF);
public:
	char* 	GetType() const;

public:
	G2Packet* 	WritePacket(G2Packet* pPacket);
	G2Packet* 	WritePacket(const char* pszType, quint32 nLength, bool bCompound = false);
	bool	ReadPacket(char* pszType, quint32& nLength, bool* pbCompound = 0);
	bool	SkipCompound();
	bool	SkipCompound(quint32& nLength, quint32 nRemaining = 0);
	bool	GetTo(QUuid& pGUID);


public:
	static	G2Packet* ReadBuffer(CBuffer* pBuffer);
	void	ToBuffer(CBuffer* pBuffer) const;

	// Inline Packet Operations
public:

	inline bool IsType(const char* sType)
	{
		return strcmp(sType, m_sType) == 0;
	}

	inline int GetRemaining()
	{
		return m_nLength - m_nPosition;
	}

	inline bool Ensure(quint32 nBytes)
	{
		if(m_nLength + nBytes > m_nBuffer)
		{
			m_nBuffer += qMax(nBytes, 128u);
			m_pBuffer = (uchar*)qRealloc(m_pBuffer, m_nBuffer);

			if(!m_pBuffer)
			{
				return false;
			}
		}

		return true;
	}

	inline void Read(void* pData, int nLength)
	{
		if(m_nPosition + nLength > m_nLength)
		{
			throw packet_read_past_end();
		}
		memcpy(pData, m_pBuffer + m_nPosition, nLength);
		m_nPosition += nLength;
	}

	inline void Write(void* pData, int nLength)
	{
		Ensure(nLength);

		memcpy(m_pBuffer + m_nLength, pData, nLength);
		m_nLength += nLength;
	}

	template <typename T>
	inline T ReadIntBE()
	{
		if(m_nLength - m_nPosition < sizeof(T))
		{
			throw packet_read_past_end();
		}

		T nRet = qFromBigEndian(*(T*)(m_pBuffer + m_nPosition));
		m_nPosition += sizeof(T);
		return nRet;
	}
	template <typename T>
	inline T ReadIntLE()
	{
		if(m_nLength - m_nPosition < sizeof(T))
		{
			throw packet_read_past_end();
		}

		T nRet = qFromLittleEndian(*(T*)(m_pBuffer + m_nPosition));
		m_nPosition += sizeof(T);
		return nRet;
	}
	template <typename T>
	inline void ReadIntBE(T* pDest)
	{
		*pDest = ReadIntBE<T>();
	}
	template <typename T>
	inline void ReadIntLE(T* pDest)
	{
		*pDest = ReadIntLE<T>();
	}
	template <typename T>
	inline void WriteIntBE(T* nValue)
	{
		T nNew = qToBigEndian(*nValue);
		Write((void*)&nNew, sizeof(T));
	}
	template <typename T>
	inline void WriteIntLE(T* nValue)
	{
		T nNew = qToLittleEndian(*nValue);
		Write((void*)&nNew, sizeof(T));
	}
	template <typename T>
	inline void WriteIntBE(T nValue)
	{
		WriteIntBE(&nValue);
	}
	template <typename T>
	inline void WriteIntLE(T nValue)
	{
		WriteIntLE(&nValue);
	}
	inline uchar ReadByte()
	{
		uchar nRet;
		Read(&nRet, 1);
		return nRet;
	}
	inline void WriteByte(uchar nByte)
	{
		Write(&nByte, 1);
	}

	void ReadHostAddress(CEndPoint* pDest, bool bIP4 = true)
	{
		if( bIP4 )
		{
			quint32 nIP;
			quint16 nPort;
			ReadIntBE(&nIP);
			ReadIntLE(&nPort);
			pDest->setAddress(nIP);
			pDest->setPort(nPort);
		}
		else
		{
			Q_IPV6ADDR ip6;
			quint16 nPort;
			Read(&ip6, 16);
			ReadIntLE(&nPort);
			pDest->setAddress(ip6);
			pDest->setPort(nPort);
		}
	}

	QUuid ReadGUID()
	{
		QUuid ret;
		ReadIntBE(&ret.data1);
		ReadIntBE(&ret.data2);
		ReadIntBE(&ret.data3);
		Read(&ret.data4[0], 8);

		return ret;
	}
	void WriteHostAddress(CEndPoint* pSrc)
	{
		if( pSrc->protocol() == 0 ) // IPv4
		{
			Ensure(6);
			WriteIntBE(pSrc->toIPv4Address());
			WriteIntLE(pSrc->port());
		}
		else
		{
			Ensure(18);
			Q_IPV6ADDR ip6 = pSrc->toIPv6Address();
			Write(&ip6, 16);
			WriteIntLE(pSrc->port());
		}
	}
	void WriteGUID(QUuid& guid)
	{
		Ensure(16);
		WriteIntBE(guid.data1);
		WriteIntBE(guid.data2);
		WriteIntBE(guid.data3);
		Write(&guid.data4[0], 8);
	}

	QString ReadString(quint32 nMaximum = 0xFFFFFFFF);
	void WriteString(QString sToWrite, bool bTerminate = false);
	// Inline Allocation
public:

	inline void AddRef()
	{
		m_nReference++;
	}

	inline void Release()
	{
		if(this != NULL && ! --m_nReference)
		{
			Delete();
		}
	}

	inline void ReleaseChain()
	{
		if(this == NULL)
		{
			return;
		}

		for(G2Packet* pPacket = this ; pPacket ;)
		{
			G2Packet* pNext = pPacket->m_pNext;
			pPacket->Release();
			pPacket = pNext;
		}
	}

	void Delete();



	QString ToHex() const;
	QString ToASCII() const;

protected:
	friend class G2PacketPool;
};


#define G2_FLAG_COMPOUND	0x04
#define G2_FLAG_BIG_ENDIAN	0x02


class G2PacketPool
{
	// Construction
public:
	G2PacketPool();
	~G2PacketPool();

	// Attributes
protected:
	G2Packet* 	m_pFree;
	quint32		m_nFree;
protected:
	QMutex				m_pSection;
	QList<G2Packet*>	m_pPools;

	// Operations
protected:
	void	Clear();
	void	NewPool();

	// Inlines
public:
	inline G2Packet* New()
	{
		m_pSection.lock();

		if(m_nFree == 0)
		{
			NewPool();
		}
		Q_ASSERT(m_nFree > 0);

		G2Packet* pPacket = m_pFree;
		m_pFree = m_pFree->m_pNext;
		m_nFree --;

		m_pSection.unlock();

		pPacket->Reset();
		pPacket->AddRef();

		return pPacket;
	}

	inline void Delete(G2Packet* pPacket)
	{
		Q_ASSERT(pPacket != NULL);
		Q_ASSERT(pPacket->m_nReference == 0);

		m_pSection.lock();

		pPacket->m_pNext = m_pFree;
		m_pFree = pPacket;
		m_nFree ++;

		m_pSection.unlock();
	}

};

extern G2PacketPool G2Packets;

#endif // G2PACKET_H
