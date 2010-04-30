#ifndef G2PACKET_H
#define G2PACKET_H

#include "types.h"
#include <QList>
#include <QSet>
#include <QMutex>
#include <QByteArray>
#include <QAtomicInt>

struct packet_incomplete{};
struct packet_error{};
struct stream_end{};

class G2PacketPool;

class G2Packet
{
    friend class G2PacketPool;
protected:
    QAtomicInt  m_nReference;

    char        m_sName[9];
    QByteArray  m_sPayload;
    bool        m_bCompound;
    bool        m_bChildrenRead;
    quint32     m_nPosition;

    QList<G2Packet*> m_lChildren;


public:
    G2Packet();

    static G2Packet* New(const char* pName);
    void ToBuffer(QByteArray* pBuffer)
    {
        ToBuffer(*pBuffer);
    }

    void ToBuffer(QByteArray& pBuffer);

    G2Packet* WriteChild(const char* pName);
    void WriteBytes(const void* pData, quint32 nLength);

    static G2Packet* FromBuffer(QByteArray* pBuffer)
    {
        return G2Packet::FromBuffer(*pBuffer);
    }
    static G2Packet* FromBuffer(QByteArray& pBuffer);

    void SkipChildren();
    G2Packet* ReadNextChild();
    bool ReadBytes(void* pDest, quint32 nLength);

    template <typename T> void ReadIntBE(T* pDest)
    {
        ReadBytes(pDest, sizeof(T));
        *pDest = qFromBigEndian(*pDest);
    }
    template <typename T> T ReadIntBE()
    {
        T nRet = 0;
        ReadIntBE(&nRet);
        return nRet;
    }

    template <typename T> void ReadIntLE(T* pDest)
    {
        ReadBytes(pDest, sizeof(T));
        *pDest = qFromLittleEndian(*pDest);
    }
    template <typename T> T ReadIntLE()
    {
        T nRet = 0;
        ReadIntLE(&nRet);
        return nRet;
    }

    void ReadHostAddress(IPv4_ENDPOINT* pDest)
    {
        ReadIntBE(&pDest->ip);
        ReadIntLE(&pDest->port);
    }
    QString ReadString();
    void WriteString(QString sToWrite, bool bTerminate = false);
    QUuid ReadGUID()
    {
        QUuid ret;
        ReadBytes(&ret.data1, sizeof(ret.data1));
        ReadBytes(&ret.data2, sizeof(ret.data2));
        ReadBytes(&ret.data3, sizeof(ret.data3));
        ReadBytes(&ret.data4[0], sizeof(ret.data4[0]) * 8);
        return ret;
    }

    template <typename T> void WriteIntBE(T* pSrc)
    {
        T nNew = qToBigEndian(*pSrc);
        WriteBytes((void*)&nNew, sizeof(T));
    }
    template <typename T> void WriteIntLE(T* pSrc)
    {
        T nNew = qToLittleEndian(*pSrc);
        WriteBytes((void*)&nNew, sizeof(T));
    }
    template <typename T> void WriteIntBE(T nSrc)
    {
        WriteIntBE(&nSrc);
    }
    template <typename T> void WriteIntLE(T nSrc)
    {
        WriteIntLE(&nSrc);
    }

    void WriteHostAddress(IPv4_ENDPOINT* pSrc)
    {
        WriteIntBE(&pSrc->ip);
        WriteIntLE(&pSrc->port);
    }
    void WriteGUID(QUuid& guid)
    {
        WriteBytes(&guid.data1, sizeof(guid.data1));
        WriteBytes(&guid.data2, sizeof(guid.data2));
        WriteBytes(&guid.data3, sizeof(guid.data3));
        WriteBytes(&guid.data4[0], sizeof(guid.data4[0]) * 8);
    }

    bool IsAddressed(QUuid& pTargetGUID)
    {
        if( !HasChildren() )
            return false;

        if ( *m_sPayload.constData() != 0x48 ) return false;
        if ( *(m_sPayload.constData() + 1) != 0x10 ) return false;
        if ( *(m_sPayload.constData() + 2) != 'T' ) return false;
        if ( *(m_sPayload.constData() + 3) != 'O' ) return false;

        G2Packet* pTO = ReadNextChild();
        if( pTO && pTO->PayloadLength() >= 16 )
        {
            pTargetGUID = pTO->ReadGUID();
            return true;
        }

        return false;

    }

    inline bool IsType(const char* szType) const
    {
        if( strcmp(m_sName, szType) == 0 )
            return true;
        return false;
    }
    inline char* GetType()
    {
        return (char*)&m_sName;
    }

    inline bool HasChildren()
    {
        return m_bCompound;
    }
    inline quint32 PayloadLength()
    {
        if( HasChildren() && !m_bChildrenRead )
            SkipChildren();

        return m_sPayload.size();
    }
    inline quint32 Position()
    {
        return m_nPosition;
    }
    inline void Position(quint32 nPos)
    {
        Q_ASSERT(nPos < (quint32)m_sPayload.size());
        m_nPosition = nPos;
    }


    void Reset()
    {
        m_nReference = 0;
        memset(&m_sName, 0, sizeof(m_sName));
        m_sPayload.clear();
        m_bCompound = false;
        m_bChildrenRead = false;
        m_nPosition = 0;
    }

    void AddRef()
    {
        m_nReference.ref();
    }

    void Release();

};


class G2PacketPool
{
protected:
    QMutex             m_mLocker;
    QList<G2Packet*>   m_lPacketsFree;
    QSet<G2Packet*>    m_lPacketsUsed;

public:
    ~G2PacketPool();
    G2Packet* New();
    void Remove(G2Packet* pPacket);
};

extern G2PacketPool G2Packets;
#endif // G2PACKET_H
