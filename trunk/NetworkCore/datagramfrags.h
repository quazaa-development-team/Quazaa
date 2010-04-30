#ifndef DATAGRAMFRAGS_H
#define DATAGRAMFRAGS_H

#include "types.h"

class QByteArray;
class G2Packet;

class DatagramIn
{
protected:
    IPv4_ENDPOINT   m_oAddress;

    quint16 m_nSequence;
    quint8  m_nCount;
    quint8  m_nLeft;
    bool    m_bCompressed;
    quint32 m_tStarted;
    quint32 m_nBuffer;
    bool*   m_bLocked;

    QByteArray** m_pBuffer;
public:
    DatagramIn();
    ~DatagramIn();

    void Create(IPv4_ENDPOINT pHost, quint8 nFlags, quint16 nSequence, quint8 nCount);
    bool Add(quint8 nPart, const void* pData, qint32 nLength);
    G2Packet* ToG2Packet();


    friend class CDatagrams;
};

class DatagramWatcher;

class DatagramOut
{
protected:
    IPv4_ENDPOINT   m_oAddress;

    quint16     m_nSequence;
    bool        m_bCompressed;
    quint32     m_nPacket;
    quint8      m_nCount;
    quint8      m_nAcked;
    quint32*    m_pLocked;
    quint8      m_nLocked;
    quint32     m_tSent;
    bool        m_bAck;

    DatagramWatcher*    m_pWatcher;
    void*               m_pParam;
    QByteArray* m_pBuffer;

public:
    DatagramOut();
    ~DatagramOut();

    void Create(IPv4_ENDPOINT oAddr, G2Packet* pPacket, quint16 nSequence, QByteArray* pBuffer, bool bAck = false);
    bool GetPacket(quint32 tNow, char** ppPacket, quint32* pnPacket, bool bResend = false);
    bool Acknowledge(quint8 nPart);

    friend class CDatagrams;

};

#endif // DATAGRAMFRAGS_H
