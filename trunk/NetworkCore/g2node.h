#ifndef G2NODE_H
#define G2NODE_H

#include "CompressedConnection.h"
#include <QTime>
#include <QQueue>

class G2Packet;

enum G2NodeState { nsClosed, nsConnecting, nsHandshaking, nsConnected, nsClosing, nsError };

class CG2Node : public CCompressedConnection
{
    Q_OBJECT

public:
    quint32         m_tLastPacketIn;
    quint32         m_tLastPacketOut;
    quint32         m_nPacketsIn;
    quint32         m_nPacketsOut;
    QString         m_sUserAgent;
    bool            m_bG2Core;
	bool			m_bCachedKeys;
    G2NodeType      m_nType;
    G2NodeState     m_nState;
    quint32         m_tLastPingOut;
    quint32         m_nPingsWaiting;
    QTime           m_tRTTTimer;
    quint32         m_tRTT;
    quint32         m_tLastQuery;

    quint16         m_nLeafCount;
    quint16         m_nLeafMax;

    bool            m_bAcceptDeflate;

    quint32         m_tKeyRequest;

    QQueue<G2Packet*>   m_lSendQueue;

public:
    CG2Node(QObject *parent = 0);
    ~CG2Node();

    void connectToHost(IPv4_ENDPOINT oAddress);
    void AttachTo(QTcpSocket* pOther)
    {
        m_nState = nsHandshaking;
        SetupSlots();
        CCompressedConnection::AttachTo(pOther);
    }

	void SendPacket(G2Packet* pPacket, bool bBuffered = false, bool bRelease = false);
    void FlushSendQueue(bool bFullFlush = false);

protected:
    void SetupSlots();

    void ParseOutgoingHandshake();
    void ParseIncomingHandshake();

    void Send_ConnectError(QString sReason);
    void Send_ConnectOK(bool bReply, bool bDeflated = false);
    void SendStartups();

public:
    void OnTimer(quint32 tNow);
signals:
    void NodeStateChanged();
public slots:
    void OnConnect();
    void OnDisconnect();
    void OnRead();
    void OnError(QAbstractSocket::SocketError e);
    void OnStateChange(QAbstractSocket::SocketState s);

public:
    void SendLNI();
protected:
    void OnPacket(G2Packet* pPacket);
    void OnPing(G2Packet* pPacket);
    void OnPong(G2Packet* pPacket);
    void OnLNI(G2Packet* pPacket);
    void OnKHL(G2Packet* pPacket);
    void OnQHT(G2Packet* pPacket);
	void OnQKR(G2Packet* pPacket);
    void OnQKA(G2Packet* pPacket);
    void OnQA(G2Packet* pPacket);
    void OnQH2(G2Packet* pPacket);
	void OnQuery(G2Packet* pPacket);


    friend class CNetwork;
};

#endif // G2NODE_H
