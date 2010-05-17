#ifndef G2PACKET_H
#define G2PACKET_H

#include "types.h"
#include <QByteArray>
#include <QtGlobal>
#include <QMutex>
#include <QList>

struct packet_error{};
struct packet_read_past_end{};



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
	G2Packet*	m_pNext;
	quint32		m_nReference;
public:
	QByteArray	m_oBuffer;
	quint32		m_nPosition;
	char		m_sType[9];
	bool		m_bCompound;

	enum { seekStart, seekEnd };

// Operations
public:
	void	Reset();
	void	Seek(quint32 nPosition, int nRelative = seekStart);
	char*	WriteGetPointer(quint32 nLength, quint32 nOffset = 0xFFFFFFFF);
public:
	char*	GetType() const;

public:
	G2Packet*	WritePacket(G2Packet* pPacket);
	G2Packet*	WritePacket(const char* pszType, quint32 nLength, bool bCompound = false);
	bool	ReadPacket(char* pszType, quint32& nLength, bool* pbCompound = 0);
	bool	SkipCompound();
	bool	SkipCompound(quint32& nLength, quint32 nRemaining = 0);

public:
	static	G2Packet* ReadBuffer(QByteArray* pBuffer);
	void	ToBuffer(QByteArray* pBuffer) const;

// Inline Packet Operations
public:

	inline bool IsType(const char* sType)
	{
		return strcmp(sType, m_sType) == 0;
	}

	inline int GetRemaining()
	{
		return m_oBuffer.size() - m_nPosition;
	}

	inline void Read(void* pData, int nLength)
	{
		if ( m_nPosition + nLength > m_oBuffer.size() ) throw packet_read_past_end();
		memcpy(pData, m_oBuffer.constData() + m_nPosition, nLength);
		m_nPosition += nLength;
	}

	inline void Write(void* pData, int nLength)
	{
		if( m_oBuffer.size() + nLength < m_oBuffer.capacity() )
		{
			m_oBuffer.reserve(qMax(m_oBuffer.capacity() + nLength, m_oBuffer.capacity() + 128));
		}

		m_oBuffer.append((char*)pData, nLength);
	}

	template <typename T>
	inline T ReadIntBE()
	{
		if( m_oBuffer.size() - m_nPosition < sizeof(T) )
			throw packet_read_past_end();

		T nRet = qFromBigEndian(*(T*)(m_oBuffer.constData() + m_nPosition));
		m_nPosition += sizeof(T);
		return nRet;
	}
	template <typename T>
	inline T ReadIntLE()
	{
		if( m_oBuffer.size() - m_nPosition < sizeof(T) )
			throw packet_read_past_end();

		T nRet = qFromLittleEndian(*(T*)(m_oBuffer.constData() + m_nPosition));
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
	inline void WriteIntBE(T& nValue)
	{
		WriteIntBE(&nValue);
	}
	template <typename T>
	inline void WriteIntLE(T& nValue)
	{
		WriteIntLE(&nValue);
	}
	inline char ReadByte()
	{
		char nRet;
		Read(&nRet, 1);
		return nRet;
	}
	inline void WriteByte(char nByte)
	{
		Write(&nByte, 1);
	}

	void ReadHostAddress(IPv4_ENDPOINT* pDest)
	{
		ReadIntBE(&pDest->ip);
		ReadIntLE(&pDest->port);
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
	void WriteHostAddress(IPv4_ENDPOINT* pSrc)
	{
		WriteIntBE(&pSrc->ip);
		WriteIntLE(&pSrc->port);
	}
	void WriteGUID(QUuid& guid)
	{
		WriteIntBE(guid.data1);
		WriteIntBE(guid.data2);
		WriteIntBE(guid.data3);
		Write(&guid.data4[0], 8);
	}

	QString ReadString();
	void WriteString(QString sToWrite, bool bTerminate = false);
// Inline Allocation
public:

	inline void AddRef()
	{
		m_nReference++;
	}

	inline void Release()
	{
		if ( this != NULL && ! --m_nReference ) Delete();
	}

	inline void ReleaseChain()
	{
		if ( this == NULL ) return;

		for ( G2Packet* pPacket = this ; pPacket ; )
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
	G2Packet*	m_pFree;
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

		if ( m_nFree == 0 ) NewPool();
		Q_ASSERT( m_nFree > 0 );

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
		Q_ASSERT( pPacket != NULL );
		Q_ASSERT( pPacket->m_nReference == 0 );

		m_pSection.lock();

		pPacket->m_pNext = m_pFree;
		m_pFree = pPacket;
		m_nFree ++;

		m_pSection.unlock();
	}

};

extern G2PacketPool G2Packets;

#endif // G2PACKET_H
