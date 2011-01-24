#ifndef NEIGHBOUR_H
#define NEIGHBOUR_H

#include "compressedconnection.h"
#include "types.h"

class CNeighbour : public CCompressedConnection
{
	Q_OBJECT

public:
	DiscoveryProtocol	m_nProtocol;

	quint32         m_tLastPacketIn;
	quint32         m_tLastPacketOut;
	quint32         m_nPacketsIn;
	quint32         m_nPacketsOut;
	QString         m_sUserAgent;

	quint32         m_tLastPingOut;
	quint32         m_nPingsWaiting;
	QElapsedTimer   m_tRTTTimer;
	qint64          m_tRTT;
	quint32         m_tLastQuery;

	NodeState       m_nState;


public:
	CNeighbour(QObject *parent = 0);
	virtual ~CNeighbour();

	virtual void ConnectTo(CEndPoint oAddress)
	{
		systemLog.postLog(LogSeverity::Information, "Initiating neighbour connection to %s...", qPrintable(oAddress.toString()));
		m_nState = nsConnecting;
		CNetworkConnection::ConnectTo(oAddress);
		SetupSlots();
	}
	void AttachTo(CNetworkConnection* pOther)
	{
		m_nState = nsHandshaking;
		SetupSlots();
		CCompressedConnection::AttachTo(pOther);
	}

	virtual void OnTimer(quint32 tNow);
protected:
	void SetupSlots();

signals:

public slots:
	virtual void OnConnect() = 0;
	virtual void OnDisconnect() = 0;
	virtual void OnRead() = 0;
	virtual void OnError(QAbstractSocket::SocketError e) = 0;
	virtual void OnStateChange(QAbstractSocket::SocketState s) = 0;

};

#endif // NEIGHBOUR_H
