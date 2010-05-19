#ifndef DATAGRAMS_H
#define DATAGRAMS_H

#include "types.h"
#include <QMutex>
#include <QUdpSocket>
#include <QHash>
#include <QStack>
#include <QTimer>
#include <QTime>

class G2Packet;

class DatagramWatcher
{
public:
    virtual      ~DatagramWatcher();
    virtual void OnSuccess(void* pParam) = 0;
    virtual void OnFailure(void* pParam) = 0;
};

class DatagramOut;
class DatagramIn;
class QByteArray;
class QHostAddress;


class CDatagrams : public QObject
{
    Q_OBJECT

public:
    QMutex      m_pSection;
protected:
    quint32 m_nUploadLimit;

    QUdpSocket* m_pSocket;

    bool m_bFirewalled;

    QTimer*     m_tSender;
    QTime       m_tStopWatch;

    QHash<quint16, DatagramOut*>     m_SendCacheMap;    // zeby szybko odszukac pakiety po sekwencji
    QStack<DatagramOut*>             m_SendCache;       // kolejka LIFO
    QStack<DatagramOut*>             m_FreeDGOut;
    quint16                          m_nSequence;

    QHash<quint32,
        QHash<quint32, DatagramIn*>
        >                   m_RecvCache;
    QStack<DatagramIn*>     m_FreeDGIn;

    QStack<QByteArray*>     m_FreeBuffer;

    QByteArray*     m_pRecvBuffer;
    QHostAddress*   m_pHostAddress;
    quint16         m_nPort;

    bool            m_bActive;

    quint32         m_nBandwidthIn;
    quint32         m_nAvgBandwidthIn;
    quint32         m_nBandwidthOut;
    quint32         m_nAvgBandwidthOut;
    QTime           m_tMeterTimer;

    quint32 m_nDiscarded;

public:
    CDatagrams();
    ~CDatagrams();

    void Listen();
    void Disconnect();

    void SendPacket(IPv4_ENDPOINT& oAddr, G2Packet* pPacket, bool bAck = false, DatagramWatcher* pWatcher = 0, void* pParam = 0);

    void RemoveOldIn(bool bForce = false);
    void Remove(DatagramIn* pDG, bool bReclaim = false);
    void Remove(DatagramOut* pDG);
    void OnReceiveGND();
    void OnAcknowledgeGND();

    void OnPacket(IPv4_ENDPOINT addr, G2Packet* pPacket);
    // pierdołki
    void OnPing(IPv4_ENDPOINT& addr, G2Packet* pPacket);
    void OnPong(IPv4_ENDPOINT& addr, G2Packet* pPacket);
    void OnCRAWLR(IPv4_ENDPOINT& addr, G2Packet* pPacket);
    // szukanie
    void OnQKA(IPv4_ENDPOINT& addr, G2Packet* pPacket);
    void OnQA(IPv4_ENDPOINT& addr, G2Packet* pPacket);
    void OnQH2(IPv4_ENDPOINT& addr, G2Packet* pPacket);

    inline void UpdateStats()
    {
        if( m_tMeterTimer.elapsed() < 1000 )
            return;

        m_tMeterTimer.start();

        m_nAvgBandwidthIn = (m_nAvgBandwidthIn + m_nBandwidthIn) / 2;
        m_nAvgBandwidthOut = (m_nAvgBandwidthOut + m_nBandwidthOut) / 2;
        m_nBandwidthIn = m_nBandwidthOut = 0;
    }
    inline quint32 DownloadSpeed()
    {
        UpdateStats();
        return m_nAvgBandwidthIn;
    }
    inline quint32 UploadSpeed()
    {
        UpdateStats();
        return m_nAvgBandwidthOut;
    }

    inline bool IsFirewalled()
    {
        return m_bFirewalled;
    }
    inline bool isListening()
    {
        return (m_bActive && m_pSocket && m_pSocket->isValid());
    }

public slots:
    void OnDatagram();
    void FlushSendCache();

    void SetupThread();
    void CleanupThread();

signals:
    void SendQueueUpdated();

	friend class CNetwork;
};

#pragma pack(push, 1)
typedef struct
{
   char     szTag[3];
   quint8   nFlags;
   quint16  nSequence;
   quint8   nPart;
   quint8   nCount;
} GND_HEADER;

#pragma pack(pop)

extern CDatagrams Datagrams;

#endif // DATAGRAMS_H
