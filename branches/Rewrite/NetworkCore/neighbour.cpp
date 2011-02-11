#include "neighbour.h"
#include "neighbours.h"
#include "quazaasettings.h"
#include "hostcache.h"

CNeighbour::CNeighbour(QObject *parent) :
	CCompressedConnection(parent)
{
	m_nProtocol = dpNull;

	m_nState = nsClosed;
	m_tLastPacketIn = m_tLastPacketOut = 0;
	m_tLastPingOut = 0;
	m_nPingsWaiting = 0;
	m_tRTT = 0;
	m_nPacketsIn = m_nPacketsOut = 0;

}

CNeighbour::~CNeighbour()
{
	Neighbours.m_pSection.lock();
	Neighbours.RemoveNode(this);
	Neighbours.m_pSection.unlock();
}

void CNeighbour::OnTimer(quint32 tNow)
{
	if(m_nState < nsConnected)
	{
		if(tNow - m_tConnected > quazaaSettings.Connection.TimeoutConnect)
		{
			if(m_bInitiated)
			{
				HostCache.OnFailure(m_oAddress);
			}
			m_nState = nsClosing;
			Close();
			return;
		}
	}
	else if(m_nState == nsConnected)
	{
		if(tNow - m_tLastPacketIn > quazaaSettings.Connection.TimeoutTraffic)
		{
			systemLog.postLog(LogSeverity::Error, tr("Closing connection to %1 due to lack of traffic.").arg(m_oAddress.toString()));
			systemLog.postLog(LogSeverity::Debug, QString("Conn %1, Packet %2, bytes avail %3, net bytes avail %4, ping %5").arg(tNow - m_tConnected).arg(tNow - m_tLastPacketIn).arg(bytesAvailable()).arg(networkBytesAvailable()).arg(tNow - m_tLastPingOut));
			systemLog.postLog(LogSeverity::Debug, QString("Closing connection with %1 minute dead").arg(m_oAddress.toString()));
			//qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "minute dead";
			Close();
			return;
		}

		/*if(m_nPingsWaiting > 0 && tNow - m_tLastPingOut > quazaaSettings.Gnutella2.PingTimeout && tNow - m_tLastPacketIn > quazaaSettings.Connection.TimeoutTraffic)
		{
			systemLog.postLog(LogSeverity::Debug, QString("Closing connection with %1 ping timed out").arg(m_oAddress.toString()));
			//qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "ping timed out";
			Close();
			return;
		}*/
	}
}

void CNeighbour::Close(bool bDelayed)
{
	m_nState = nsClosing;
	CCompressedConnection::Close(bDelayed);
}
