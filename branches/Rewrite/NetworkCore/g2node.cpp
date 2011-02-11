//
// g2node.cpp
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

#include "g2node.h"
#include "network.h"
#include "neighbours.h"
#include "g2packet.h"
#include "hostcache.h"
#include "parser.h"
#include "datagrams.h"
#include "searchmanager.h"
#include "queryhit.h"
#include "queryhashtable.h"
#include "queryhashmaster.h"

#include "quazaasettings.h"
#include "quazaaglobals.h"

#include <QTcpSocket>

#define _DISABLE_COMPRESSION

CG2Node::CG2Node(QObject* parent) :
	CNeighbour(parent)
{

	m_nProtocol = dpGnutella2;

	m_nType = G2_UNKNOWN;

	m_nLeafCount = m_nLeafMax = 0;
	m_bAcceptDeflate = false;
	m_tLastQuery = 0;
	m_tKeyRequest = 0;
	m_bCachedKeys = false;
	m_bG2Core = false;

	m_pLocalTable = 0;
	m_pRemoteTable = 0;
}

CG2Node::~CG2Node()
{
	while(m_lSendQueue.size())
	{
		m_lSendQueue.dequeue()->Release();
	}

	if(m_pLocalTable)
	{
		delete m_pLocalTable;
	}

	if(m_pRemoteTable)
	{
		delete m_pRemoteTable;
	}
}

void CG2Node::SendPacket(G2Packet* pPacket, bool bBuffered, bool bRelease)
{
	m_nPacketsOut++;

	if(bBuffered)
	{
		while( m_lSendQueue.size() > 128 )
		{
			m_lSendQueue.dequeue()->Release();
		}

		pPacket->AddRef();
		m_lSendQueue.enqueue(pPacket);
	}
	else
	{
		pPacket->ToBuffer(GetOutputBuffer());
	}

	if(bRelease)
	{
		pPacket->Release();
	}

	emit readyToTransfer();
}

void CG2Node::OnConnect()
{
	//QMutexLocker l(&Neighbours.m_pSection);

	systemLog.postLog(LogSeverity::Information, "Connection with %s established, handshaking...", qPrintable(m_oAddress.toString()));

	m_nState = nsHandshaking;
	emit NodeStateChanged();

	QByteArray sHs;

	sHs += "GNUTELLA CONNECT/0.6\r\n";
	sHs += "Accept: application/x-gnutella2\r\n";
	sHs += "User-Agent: " + quazaaGlobals.UserAgentString() + "\r\n";
	sHs += "Remote-IP: " + m_oAddress.toString() + "\r\n";
	sHs += "Listen-IP: " + Network.GetLocalAddress().toStringWithPort() + "\r\n";
	if(Network.isHub())
	{
		sHs += "X-Ultrapeer: True\r\n";
	}
	else
	{
		sHs += "X-Ultrapeer: False\r\n";
	}
#ifndef _DISABLE_COMPRESSION
	sHs += "Accept-Encoding: deflate\r\n";
#endif

	sHs += "\r\n";

	//qDebug() << "Handshake send:\n" << sHs;

	Write(sHs);
}
void CG2Node::OnDisconnect()
{
	//QMutexLocker l(&Neighbours.m_pSection);
	//qDebug("OnDisconnect()");
	systemLog.postLog(LogSeverity::Information, "Connection with %s was dropped: %s", qPrintable(m_oAddress.toString()), qPrintable(m_pSocket->errorString()));
	delete this;
}

void CG2Node::OnRead()
{
	//qDebug() << "CG2Node::OnRead";
	if(m_nState == nsHandshaking)
	{
		if(Peek(bytesAvailable()).indexOf("\r\n\r\n") != -1)
		{
			if(m_bInitiated)
			{
				ParseOutgoingHandshake();
			}
			else
			{
				ParseIncomingHandshake();
			}
		}
	}
	else if(m_nState == nsConnected)
	{

		G2Packet* pPacket = 0;
		try
		{
			while((pPacket = G2Packet::ReadBuffer(GetInputBuffer())))
			{
				m_tLastPacketIn = time(0);
				m_nPacketsIn++;

				OnPacket(pPacket);

				pPacket->Release();
			}
		}
		catch(...)
		{
			if(pPacket)
			{
				systemLog.postLog(LogSeverity::Debug, QString("%1\n%2").arg(pPacket->ToHex()).arg(pPacket->ToASCII()));
				//qDebug() << pPacket->ToHex() << "\n" << pPacket->ToASCII();
				pPacket->Release();
			}

			systemLog.postLog(LogSeverity::Debug, QString("Packet error - ").arg(m_oAddress.toString()));
			//qDebug() << "Packet error - " << m_oAddress.toString().toAscii();
			Close();
		}
	}

}
void CG2Node::OnError(QAbstractSocket::SocketError e)
{
	if(e != QAbstractSocket::RemoteHostClosedError)
	{
		HostCache.OnFailure(m_oAddress);
	}

	//systemLog.postLog(tr("Remote host closed connection: %1. Error: %2").arg(m_oAddress.toString()).arg(m_pSocket->errorString()), LogSeverity::Notice);
	//qDebug() << "OnError(" << e << ")" << m_oAddress.toString();
	delete this;
}

void CG2Node::OnStateChange(QAbstractSocket::SocketState s)
{
	Q_UNUSED(s);

	//QMutexLocker l(&Network.m_pSection);

	//qDebug() << "OnStateChange(" << s << ")";
}

void CG2Node::OnTimer(quint32 tNow)
{

	CNeighbour::OnTimer(tNow);

	if(m_nState == nsConnected)
	{
		if(m_nPingsWaiting == 0 && (tNow - m_tLastPacketIn >= 30 || tNow - m_tLastPingOut >= quazaaSettings.Gnutella2.PingRate))
		{
			// Jesli dostalismy ostatni pakiet co najmniej 30 sekund temu
			// lub wyslalismy ostatniego pinga co najmniej 2 minuty temu
			// no i nie oczekujemy odpowiedzi na wczesniejszego pinga
			// to wysylamy keep-alive ping, przy okazji merzac RTT
			G2Packet* pPacket = G2Packet::New("PI", false);
			SendPacket(pPacket, false, true); // niebuforowany, zeby dokladniej zmierzyc RTT
			m_nPingsWaiting++;
			m_tLastPingOut = tNow;
			m_tRTTTimer.start();
		}

		/*if( m_tKeyRequest > 0 && tNow - m_tKeyRequest > 90 )
		{
			qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "QueryKey wait timeout reached";
			m_nState = nsClosing;
			emit NodeStateChanged();
			Close();
			return;
		}*/

		if((m_nType == G2_HUB && tNow - m_tConnected > 30) &&
				((m_pLocalTable != 0 && m_pLocalTable->m_nCookie != QueryHashMaster.m_nCookie) &&
				 (tNow - m_pLocalTable->m_nCookie > 60) &&
				 (tNow - QueryHashMaster.m_nCookie > 30) ||
				 QueryHashMaster.m_nCookie - m_pLocalTable->m_nCookie > 60 ||
				 !m_pLocalTable->m_bLive)
		  )
		{
			if(m_pLocalTable->PatchTo(&QueryHashMaster, this))
			{
				systemLog.postLog(LogSeverity::Notice, tr("Sending query routing table to %1 (%2 bits, %3 entries, %4 bytes, %5% full)").arg(m_oAddress.toString().toAscii().constData()).arg(m_pLocalTable->m_nBits).arg(m_pLocalTable->m_nHash).arg(m_pLocalTable->m_nHash / 8).arg(m_pLocalTable->GetPercent()));
			}
		}

		// Anti-DDoS
		// cleaning table
		if( m_lRABan.size() >= 1000 )
		{
			systemLog.postLog(LogSeverity::Debug, QString("Clearing bans on hub %1").arg(m_oAddress.toString()));
			//qDebug() << "Clearing bans on hub " << m_oAddress.toString();
			for( QHash<quint32,quint32>::iterator itBan = m_lRABan.begin(); itBan != m_lRABan.end(); )
			{
				if( tNow - itBan.value() > 60 ) // 1 minute
				{
					itBan = m_lRABan.erase(itBan);
				}
				else
				{
					++itBan;
				}
			}
			systemLog.postLog(LogSeverity::Debug, QString("Still active bans: %1").arg(m_lRABan.size()));
			//qDebug() << "Still active bans: " << m_lRABan.size();
		}


		//FlushSendQueue(true);
	}
	else if(m_nState == nsClosing)
	{
		if(m_nType == G2_UNKNOWN && tNow - m_tConnected > 20)
		{
			Close();
		}
		else if(tNow - m_tLastPacketIn > 20)
		{
			Close();
		}
	}
}

void CG2Node::ParseIncomingHandshake()
{
	//QMutexLocker l(&Neighbours.m_pSection);

	qint32 nIndex = Peek(bytesAvailable()).indexOf("\r\n\r\n");
	QString sHs = Read(nIndex + 4);

	//qDebug() << "Handshake receive:\n" << sHs;

	if(m_sUserAgent.isEmpty())
	{
		m_sUserAgent = Parser::GetHeaderValue(sHs, "User-Agent");
	}

	if(m_sUserAgent.isEmpty())
	{
		Send_ConnectError("503 Anonymous clients are not allowed here");
		return;
	}


	if(sHs.startsWith("GNUTELLA CONNECT/0.6"))
	{
		QString sAccept = Parser::GetHeaderValue(sHs, "Accept");
		bool bAcceptG2 = sAccept.contains("application/x-gnutella2");

		if(!bAcceptG2)
		{
			Send_ConnectError("503 Required network not accepted");
			return;
		}

#ifndef _DISABLE_COMPRESSION
		m_bAcceptDeflate = false;
		QString sAcceptEnc = Parser::GetHeaderValue(sHs, "Accept-Encoding");
		if(sAcceptEnc.contains("deflate") && Network.isHub())
		{
			m_bAcceptDeflate = true;
		}
#endif

		QString sUltra = Parser::GetHeaderValue(sHs, "X-Ultrapeer").toLower();
		//QString sUltraNeeded = Parser::GetHeaderValue(sHs, "X-Ultrapeer-Needed").toLower();
		if(sUltra.isEmpty())
		{
			Send_ConnectError("503 No hub mode specified");
			return;
		}

		QString sRemoteIP = Parser::GetHeaderValue(sHs, "Remote-IP");
		if(!sRemoteIP.isEmpty())
		{
			Network.AcquireLocalAddress(sRemoteIP);
		}
		else
		{
			Send_ConnectError("503 Remote-IP header missing");
			return;
		}

		bool bUltra = (sUltra == "true");

		if(bUltra)
		{
			if(!Neighbours.NeedMore(G2_HUB))
			{
				Send_ConnectError("503 Maximum hub connections reached");
				return;
			}
			m_nType = G2_HUB;
		}
		else
		{
			if(!Neighbours.NeedMore(G2_LEAF))
			{
				Send_ConnectError("503 Maximum leaf connections reached");
				return;
			}
			m_nType = G2_LEAF;
		}

		Send_ConnectOK(false, m_bAcceptDeflate);

	}
	else if(sHs.contains(" 200 OK"))
	{
		QString sContentType = Parser::GetHeaderValue(sHs, "Content-Type");
		bool bG2Provided = sContentType.contains("application/x-gnutella2");

		if(!bG2Provided)
		{
			Send_ConnectError("503 Required network not provided");
			return;
		}

#ifndef _DISABLE_COMPRESSION
		QString sContentEnc = Parser::GetHeaderValue(sHs, "Content-Encoding");
		if(sContentEnc.contains("deflate"))
		{
			if(!EnableInputCompression())
			{
				systemLog.postLog(LogSeverity::Debug, QString("Inflate init error!"));
				//qDebug() << "Inflate init error!";
				Close();
				return;
			}
		}

		if(m_bAcceptDeflate)
		{
			if(!EnableOutputCompression())
			{
				systemLog.postLog(LogSeverity::Debug, QString("Deflate init error!"));
				//qDebug() << "Deflate init error!";
				Close();
				return;
			}
		}
#endif

		m_nState = nsConnected;
		emit NodeStateChanged();

		SendStartups();
		m_tLastPacketIn = m_tLastPacketOut = time(0);
		if(m_nType == G2_HUB)
		{
			m_pLocalTable = new CQueryHashTable();
		}


	}
	else
	{
		systemLog.postLog(LogSeverity::Debug, QString("Connection to %1 rejected: %2").arg(this->m_oAddress.toString()).arg(sHs.left(sHs.indexOf("\r\n"))));
		//qDebug() << "Connection rejected: " << sHs.left(sHs.indexOf("\r\n"));
		m_nState = nsClosing;
		emit NodeStateChanged();
		Close();
	}
}

void CG2Node::ParseOutgoingHandshake()
{
	//QMutexLocker l(&Neighbours.m_pSection);
	QString sHs = Read(Peek(bytesAvailable()).indexOf("\r\n\r\n") + 4);

	//qDebug() << "Handshake receive:\n" << sHs;

	QString sAccept = Parser::GetHeaderValue(sHs, "Accept");
	bool bAcceptG2 = sAccept.contains("application/x-gnutella2");

	if(!bAcceptG2)
	{
		Send_ConnectError("503 Required network not accepted");
		return;
	}

	QString sContentType = Parser::GetHeaderValue(sHs, "Content-Type");
	bool bG2Provided = sContentType.contains("application/x-gnutella2");

	if(!bG2Provided)
	{
		Send_ConnectError("503 Required network not provided");
		return;
	}

	QString sTry = Parser::GetHeaderValue(sHs, "X-Try-Hubs");
	if(bAcceptG2 && bG2Provided && sTry.size())
	{
		HostCache.AddXTry(sTry);
	}

	if(sHs.left(16) != "GNUTELLA/0.6 200")
	{
		systemLog.postLog(LogSeverity::Debug, QString("Connection to %1 rejected: %2").arg(this->m_oAddress.toString()).arg(sHs.left(sHs.indexOf("\r\n"))));
		//qDebug() << "Connection rejected: " << sHs.left(sHs.indexOf("\r\n"));
		Close();
		return;
	}

	m_sUserAgent = Parser::GetHeaderValue(sHs, "User-Agent");

	if(m_sUserAgent.isEmpty())
	{
		Send_ConnectError("503 Anonymous clients are not allowed here");
		return;
	}

	QString sRemoteIP = Parser::GetHeaderValue(sHs, "Remote-IP");
	if(!sRemoteIP.isEmpty())
	{
		Network.AcquireLocalAddress(sRemoteIP);
	}
	else
	{
		Send_ConnectError("503 Remote-IP header missing");
		return;
	}

	QString sUltra = Parser::GetHeaderValue(sHs, "X-Ultrapeer").toLower();
	//QString sUltraNeeded = Parser::GetHeaderValue(sHs, "X-Ultrapeer-Needed").toLower();

	bool bUltra = (sUltra == "true");
	//bool bUltraNeeded = (sUltraNeeded == "true");

#ifndef _DISABLE_COMPRESSION
	QString sContentEnc = Parser::GetHeaderValue(sHs, "Content-Encoding");
	if(sContentEnc.contains("deflate"))
	{
		if(!EnableInputCompression())
		{
			systemLog.postLog(LogSeverity::Debug, "Inflate init error!");
			//qDebug() << "Inflate init error!";
			Close();
			return;
		}
	}
#endif

	bool bAcceptDeflate = false;
#ifndef _DISABLE_COMPRESSION
	QString sAcceptEnc = Parser::GetHeaderValue(sHs, "Accept-Encoding");
	if(sAcceptEnc.contains("deflate") && Network.isHub())
	{
		bAcceptDeflate = true;
	}
#endif

	if(bUltra)
	{
		if(!Neighbours.NeedMore(G2_HUB))
		{
			Send_ConnectError("503 Maximum hub connections reached");
			return;
		}
		m_nType = G2_HUB;
	}
	else
	{
		if(!Neighbours.NeedMore(G2_LEAF))
		{
			Send_ConnectError("503 Maximum leaf connections reached");
			return;
		}
		m_nType = G2_LEAF;
	}

	Send_ConnectOK(true, bAcceptDeflate);

#ifndef _DISABLE_COMPRESSION
	if(bAcceptDeflate)
	{
		if(!EnableOutputCompression())
		{
			systemLog.postLog(LogSeverity::Debug, "Deflate init error!");
			//qDebug() << "Deflate init error!";
			Close();
			return;
		}
	}
#endif

	m_nState = nsConnected;
	emit NodeStateChanged();

	SendStartups();
	m_tLastPacketIn = m_tLastPacketOut = time(0);

	if(m_nType == G2_HUB)
	{
		m_pLocalTable = new CQueryHashTable();
	}

	systemLog.postLog(LogSeverity::Information, tr("Gnutella2 connection with %1 established.").arg(qPrintable(m_oAddress.toString())));
}
void CG2Node::Send_ConnectError(QString sReason)
{
	systemLog.postLog(LogSeverity::Information, tr("Rejecting connection with %1: %2").arg(qPrintable(m_oAddress.toString())).arg(qPrintable(sReason)));

	systemLog.postLog(LogSeverity::Debug, QString("Rejecting connection: %1").arg(sReason));
	//qDebug() << "Rejecting connection:" << sReason;

	QByteArray sHs;

	sHs += "GNUTELLA/0.6 " + sReason + "\r\n";
	sHs += "User-Agent: " + quazaaGlobals.UserAgentString() + "\r\n";
	sHs += "Accept: application/x-gnutella2\r\n";
	sHs += "Content-Type: application/x-gnutella2\r\n";
	sHs += HostCache.GetXTry();
	sHs += "\r\n";

	//qDebug() << "Handshake send:\n" << sHs;
	Write(sHs);

	Close(true);
}
void CG2Node::Send_ConnectOK(bool bReply, bool bDeflated)
{
	QByteArray sHs;

	sHs += "GNUTELLA/0.6 200 OK\r\n";
	sHs += "User-Agent: " + quazaaGlobals.UserAgentString() + "\r\n";
	if(Network.isHub())
	{
		sHs += "X-Ultrapeer: True\r\n";
	}
	else
	{
		sHs += "X-Ultrapeer: False\r\n";
	}
	sHs += "Content-Type: application/x-gnutella2\r\n";

	if(!bReply)
	{
		// 2-handshake
#ifndef _DISABLE_COMPRESSION
		if(Network.isHub() && m_nType == G2_HUB)
		{
			sHs += "Accept-Encoding: deflate\r\n";
		}

		if(bDeflated)
		{
			sHs += "Content-Encoding: deflate\r\n";
		}
#endif
		sHs += "Accept: application/x-gnutella2\r\n";
		sHs += "Remote-IP: ";
		sHs += m_oAddress.toString();
		sHs += "\r\n";
		sHs += "Listen-IP: " + Network.GetLocalAddress().toStringWithPort() + "\r\n";
	}
	else
	{
#ifndef _DISABLE_COMPRESSION
		if(bDeflated)
		{
			sHs += "Content-Encoding: deflate\r\n";
		}
#endif
	}

	sHs += "\r\n";

	//qDebug() << "Handshake send:\n" << sHs;

	Write(sHs);

}

void CG2Node::SendStartups()
{
	if(Network.IsListening())
	{
		CEndPoint addr = Network.GetLocalAddress();
		G2Packet* pPacket = G2Packet::New("PI", true);
		pPacket->WritePacket("UDP", 6);
		pPacket->WriteHostAddress(&addr);
		pPacket->WritePacket("TFW", 0);
		SendPacket(pPacket, false, true);
	}

	SendLNI();
}

void CG2Node::SendLNI()
{
	G2Packet* pLNI = G2Packet::New("LNI", true);
	if( Network.m_oAddress.protocol() == 0 )
	{
		pLNI->WritePacket("NA", 6)->WriteHostAddress(&Network.m_oAddress);
	}
	else
	{
		pLNI->WritePacket("NA", 18)->WriteHostAddress(&Network.m_oAddress);
	}
	pLNI->WritePacket("GU", 16)->WriteGUID(quazaaSettings.Profile.GUID);
	pLNI->WritePacket("V", 4)->WriteString(quazaaGlobals.VendorCode(), false);

	if(Network.isHub())
	{
		quint16 nLeavesMax = quazaaSettings.Gnutella2.NumLeafs;
		quint16 nLeaves = Neighbours.m_nLeavesConnected;
		pLNI->WritePacket("HS", 4);
		pLNI->WriteIntLE(nLeaves);
		pLNI->WriteIntLE(nLeavesMax);
		pLNI->WritePacket("QK", 0);
	}

	pLNI->WritePacket("g2core", 0);

	SendPacket(pLNI, false, true);
}


void CG2Node::OnPacket(G2Packet* pPacket)
{
	//qDebug() << "Got packet " << pPacket->GetType() << pPacket->ToHex() << pPacket->ToASCII();

	//try
	//{
	if(!Network.RoutePacket(pPacket))
	{

		if(pPacket->IsType("PI"))
		{
			OnPing(pPacket);
		}
		else if(pPacket->IsType("PO"))
		{
			OnPong(pPacket);
		}
		else if(pPacket->IsType("LNI"))
		{
			OnLNI(pPacket);
		}
		else if(pPacket->IsType("KHL"))
		{
			OnKHL(pPacket);
		}
		else if(pPacket->IsType("QHT"))
		{
			OnQHT(pPacket);
		}
		else if(pPacket->IsType("Q2"))
		{
			OnQuery(pPacket);
		}
		else if(pPacket->IsType("QKR"))
		{
			OnQKR(pPacket);
		}
		else if(pPacket->IsType("QKA"))
		{
			OnQKA(pPacket);
		}
		else if(pPacket->IsType("QA"))
		{
			OnQA(pPacket);
		}
		else if(pPacket->IsType("QH2"))
		{
			OnQH2(pPacket);
		}
		else
		{
			systemLog.postLog(LogSeverity::Debug, QString("G2 TCP recieved unknown packet %1").arg(pPacket->GetType()));
			//qDebug() << "Unknown packet " << pPacket->GetType();
		}
	}
	/*}
	catch(...)
	{
		qDebug() << "Packet error in child packets";
	}*/
}

void CG2Node::OnPing(G2Packet* pPacket)
{
	bool bUdp = false;
	bool bRelay = false;
	bool bTestFirewall = false;
	CEndPoint addr;

	if(pPacket->m_bCompound)
	{
		char szType[9];
		quint32 nLength = 0, nNext = 0;

		while(pPacket->ReadPacket(&szType[0], nLength))
		{
			nNext = pPacket->m_nPosition + nLength;

			if(strcmp("UDP", szType) == 0 && nLength >= 6)
			{
				pPacket->ReadHostAddress(&addr);
				if(!addr.isNull())
				{
					bUdp = true;
				}
			}
			else if(strcmp("RELAY", szType) == 0)
			{
				bRelay = true;
			}
			else if(strcmp("TFW", szType) == 0)
			{
				bTestFirewall = true;
			}

			pPacket->m_nPosition = nNext;
		}

	}

	if(!bUdp && !bRelay)
	{
		// direct ping
		G2Packet* pPong = G2Packet::New("PO", false);
		SendPacket(pPong, false, true);
		return;
	}

	if(bUdp && !bRelay)
	{
		// /PI/UDP

		if(Network.isHub())
		{
			QMutexLocker l(&Neighbours.m_pSection);

			uchar* pRelay = pPacket->WriteGetPointer(7, 0);
			*pRelay++ = 0x60;
			*pRelay++ = 0;
			*pRelay++ = 'R';
			*pRelay++ = 'E';
			*pRelay++ = 'L';
			*pRelay++ = 'A';
			*pRelay++ = 'Y';

			int nRelayed = 0, nCount = Neighbours.GetCount();
			QList<int> lToRelayIndex;

			for(int i = 0; i < nCount && nRelayed < quazaaSettings.Gnutella2.PingRelayLimit; ++i)
			{
				int nIndex = qrand() % nCount;
				if(!lToRelayIndex.contains(nIndex))
				{
					CNeighbour* pNode = Neighbours.GetAt(nIndex);
					if(pNode != this && pNode->m_nProtocol == dpGnutella2 && pNode->m_nState == nsConnected)
					{
						pPacket->AddRef();
						((CG2Node*)pNode)->SendPacket(pPacket, true, true);
						nRelayed++;
					}
				}
			}

			/*QList<CG2Node*> lToRelay;

			for( QList<CG2Node*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode )
			{
				CG2Node* pNode = *itNode;
				if( pNode != this && pNode->m_nState == nsConnected )
					lToRelay.append(pNode);
			}

			for( int nCount = 0; nCount < quazaaSettings.Gnutella2.PingRelayLimit && lToRelay.size(); nCount++ )
			{
				int nIndex = qrand() % lToRelay.size();
				pPacket->AddRef();
				CG2Node* pNode = lToRelay.at(nIndex);
				pNode->SendPacket(pPacket, true, true);
				lToRelay.removeAt(nIndex);
			}*/

			return;
		}
	}

	if(bUdp && bRelay)
	{
		G2Packet* pPong = G2Packet::New("PO", true);
		pPong->WritePacket("RELAY", 0);
		Datagrams.SendPacket(addr, pPong, true);
		pPong->Release();
	}
}

void CG2Node::OnPong(G2Packet* pPacket)
{
	Q_UNUSED(pPacket);

	if(m_nPingsWaiting > 0)
	{
		m_nPingsWaiting--;

		if(m_nPingsWaiting == 0)
		{
			m_tRTT = m_tRTTTimer.elapsed();
		}
	}
}

void CG2Node::OnLNI(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	bool hasNA = false;
	QUuid pGUID;
	bool hasGUID = false;

	CEndPoint hostAddr;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("NA", szType) == 0 && nLength >= 6)
		{
			if( nLength >= 18 )
			{
				pPacket->ReadHostAddress(&hostAddr, false);
			}
			else
			{
				pPacket->ReadHostAddress(&hostAddr);
			}

			if( !hostAddr.isNull() )
			{
				hasNA = true;

				if( !m_bInitiated )
					m_oAddress = hostAddr;
			}

		}
		else if(strcmp("GU", szType) == 0 && nLength >= 16)
		{
			hasGUID = true;
			pGUID = pPacket->ReadGUID();
		}
		else if(strcmp("HS", szType) == 0 && nLength >= 4)
		{
			if(m_nType == G2_HUB)
			{
				pPacket->ReadIntLE(&m_nLeafCount);
				pPacket->ReadIntLE(&m_nLeafMax);
			}
		}
		else if(strcmp("QK", szType) == 0)
		{
			m_bCachedKeys = true;
		}
		else if(strcmp("g2core", szType) == 0)
		{
			m_bG2Core = true;
		}

		pPacket->m_nPosition = nNext;
	}

	if(hasNA && hasGUID)
	{
		QMutexLocker l(&Network.m_pSection);
		Network.m_oRoutingTable.Add(pGUID, this, true);
	}

}
void CG2Node::OnKHL(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	quint32 tNow = time(0);
	quint32 nTimestamp = tNow;
	qint64 nDiff = 0;

	char szType[9], szInner[9];
	quint32 nLength = 0, nInnerLength = 0;
	bool bCompound = false;
	quint32 nNext = 0, nInnerNext = 0;

	while(pPacket->ReadPacket(&szType[0], nLength, &bCompound))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("NH", szType) == 0 && bCompound)
		{
			QUuid pGUID;

			while(pPacket->m_nPosition < nNext && pPacket->ReadPacket(&szInner[0], nInnerLength))
			{
				nInnerNext = pPacket->m_nPosition + nInnerLength;

				if(strcmp("GU", szInner) && nInnerLength >= 16)
				{
					pGUID = pPacket->ReadGUID();
				}

				pPacket->m_nPosition = nInnerNext;
			}

			if(!pGUID.isNull() && nLength >= 6)
			{
				CEndPoint pAddr;

				if( nLength >= 18 )
				{
					pPacket->ReadHostAddress(&pAddr, false);
				}
				else
				{
					pPacket->ReadHostAddress(&pAddr);
				}

				QMutexLocker l(&Network.m_pSection);

				Network.m_oRoutingTable.Add(pGUID, this, &pAddr, false);
			}
		}
		else if(strcmp("CH", szType) == 0)
		{
			if(bCompound)
			{
				pPacket->SkipCompound(nLength);
			}

			if(nLength >= 10)
			{
				CEndPoint ep;
				quint32 nTs = 0;

				if( nLength >= 22 )
				{
					pPacket->ReadHostAddress(&ep, false);
				}
				else
				{
					pPacket->ReadHostAddress(&ep);
				}

				pPacket->ReadIntLE(&nTs);

				HostCache.m_pSection.lock();
				HostCache.Add(ep, tNow + nDiff);
				HostCache.m_pSection.unlock();
			}
		}
		else if(strcmp("TS", szType) == 0)
		{
			if(bCompound)
			{
				pPacket->SkipCompound(nLength);
			}

			if(nLength >= 4)
			{
				pPacket->ReadIntLE(&nTimestamp);
				nDiff = tNow - nTimestamp;
			}
		}
		pPacket->m_nPosition = nNext;
	}
}

void CG2Node::OnQHT(G2Packet* pPacket)
{
	if(m_pRemoteTable == 0)
	{
		if(!Network.isHub())
		{
			systemLog.postLog(LogSeverity::Debug, QString("Recieved unexpected Query Routing Table, ignoring"));
			//qDebug() << "Received unexpected Query Routing Table, ignoring";
			return;
		}

		m_pRemoteTable = new CQueryHashTable();
	}

	bool bLive = m_pRemoteTable->m_bLive;

	if(!m_pRemoteTable->OnPacket(pPacket))
	{
		systemLog.postLog(LogSeverity::Error, tr("Neighbour %1 sent bad query hash table update. Closing connection.").arg(m_oAddress.toString().toAscii().constData()));
		Close();
		return;
	}

	if(m_pRemoteTable->m_bLive && !bLive)
	{
		systemLog.postLog(LogSeverity::Notice, tr("Neighbour %1 updated its query hash table. %2 bits %3% full.").arg(m_oAddress.toString().toUtf8().constData()).arg(m_pRemoteTable->m_nBits).arg(m_pRemoteTable->GetPercent()));
	}

	if(m_nType == G2_LEAF && m_pRemoteTable && m_pRemoteTable->GetPercent() > 90)
	{
		systemLog.postLog(LogSeverity::Error, tr("Dropping neighbour %1 - hash table fill percentage too high.").arg(m_oAddress.toString().toAscii().constData()));
		Close();
		return;
	}

	if(m_nType == G2_LEAF && m_pRemoteTable->m_pGroup == 0)
	{
		QueryHashMaster.Add(m_pRemoteTable);
	}
}

void CG2Node::OnQKR(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound || m_nType != G2_LEAF)
	{
		return;
	}

	char szType[9];
	quint32 nLength = 0, nNext = 0;
	bool bCacheOK = true;
	CEndPoint addr;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("QNA", szType) == 0 && nLength >= 6)
		{
			if( nLength >= 18 )
			{
				pPacket->ReadHostAddress(&addr, false);
			}
			else
			{
				pPacket->ReadHostAddress(&addr);
			}
		}
		else if(strcmp("REF", szType) == 0)
		{
			bCacheOK = false;
		}

		pPacket->m_nPosition = nNext;
	}

	if(addr.isNull())   // TODO: sprawdzene czy adres jest za fw
	{
		return;
	}

	QMutexLocker l(&HostCache.m_pSection);

	CHostCacheHost* pHost = bCacheOK ? HostCache.Find(addr) : 0;

	if(pHost != 0 && pHost->m_nQueryKey != 0 && pHost->m_nKeyHost == Network.m_oAddress && time(0) - pHost->m_nKeyTime < quazaaSettings.Gnutella2.QueryKeyTime)
	{
		G2Packet* pQKA = G2Packet::New("QKA", true);
		if( addr.protocol() == 0 )
		{
			pQKA->WritePacket("QNA", 6)->WriteHostAddress(&addr);
		}
		else
		{
			pQKA->WritePacket("QNA", 18)->WriteHostAddress(&addr);
		}
		pQKA->WritePacket("QK", 4)->WriteIntBE(pHost->m_nQueryKey);
		pQKA->WritePacket("CACHED", 0);
		SendPacket(pQKA, true, true);
	}
	else
	{
		G2Packet* pQKR = G2Packet::New("QKR", true);
		if( addr.protocol() == 0 )
		{
			pQKR->WritePacket("SNA", 6)->WriteHostAddress(&m_oAddress);
		}
		else
		{
			pQKR->WritePacket("SNA", 18)->WriteHostAddress(&m_oAddress);
		}
		Datagrams.SendPacket(addr, pQKR, false);
		pQKR->Release();
	}
}

void CG2Node::OnQKA(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	m_tKeyRequest = 0;

	quint32 nKey;
	CEndPoint addr;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("QK", szType) == 0 && nLength >= 4)
		{
			pPacket->ReadIntLE(&nKey);
		}
		else if(strcmp("QNA", szType) == 0)
		{
			if(nLength >= 6)
			{
				if( nLength >= 18 )
				{
					// IPv6 with port
					pPacket->ReadHostAddress(&addr, false);
				}
				else if( nLength >= 16 )
				{
					// IPv6 without port
					Q_IPV6ADDR ip6;
					pPacket->Read(&ip6, 16);
					addr.setAddress(ip6);
				}
				else
				{
					// IPv4 with port
					pPacket->ReadHostAddress(&addr);
				}
			}
			else if(nLength >= 4)
			{
				// IPv4 without port
				quint32 ip4;
				pPacket->ReadIntBE(&ip4);
				addr.setAddress(ip4);
				addr.setPort(6346);
			}
		}

		pPacket->m_nPosition = nNext;
	}

	HostCache.m_pSection.lock();
	CHostCacheHost* pCache = HostCache.Add(addr, 0);
	if(pCache)
	{
		pCache->SetKey(nKey, &m_oAddress);

		systemLog.postLog(LogSeverity::Debug, QString("Got a query key from %1 via %2 = 0x%3").arg(addr.toString().toAscii().constData()).arg(m_oAddress.toString().toAscii().constData()).arg(QString().number(nKey, 16)));
		//qDebug("Got a query key from %s via %s = 0x%x", addr.toString().toAscii().constData(), m_oAddress.toString().toAscii().constData(), nKey);
	}
	HostCache.m_pSection.unlock();
}
void CG2Node::OnQA(G2Packet* pPacket)
{
	QUuid oGUID;
	SearchManager.OnQueryAcknowledge(pPacket, m_oAddress, oGUID);

	// TCP /QA - no need for routing, it's either for us or to be dropped
}

void CG2Node::OnQH2(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	QueryHitInfo* pInfo = CQueryHit::ReadInfo(pPacket, &m_oAddress);

	if(SearchManager.OnQueryHit(pPacket, pInfo))
	{
		Network.m_pSection.lock();

		if(Network.isHub() && pInfo->m_nHops < 7)
		{
			Network.m_oRoutingTable.Add(pInfo->m_oNodeGUID, this, false);
			pPacket->m_pBuffer[pPacket->m_nLength - 17]++;
			Network.RoutePacket(pInfo->m_oGUID, pPacket);
		}

		Network.m_pSection.unlock();

		delete pInfo;
	}

}
void CG2Node::OnQuery(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	// temporary code
	// just read guid for now to have it in routing table

	QUuid oGUID;
	CEndPoint oReturnAddr;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if( strcmp(&szType[0], "UDP") == 0 && nLength >= 6 )
		{
			pPacket->ReadHostAddress(&oReturnAddr, !(nLength >= 18));
		}

		pPacket->m_nPosition = nNext;
	}

	/*
	 * Anti-DDoS
	 * Code below is to minimize the impact caused by forwarded searches.
	 * We restrict number of queries with a given return address sent by this neighbouring hub.
	 * We allow unlimited queried for this connected hub (when its address is specified in query).
	 * Our hub can still be searched via other neighbouring hubs.
	 * This is to protect inter-hub query forwarding where nothing is checked.
	 * The main problem is still there, though.
	 */

	/*if( Network.isHub() && m_nType == G2_HUB ) // must be both hubs
	{
		QHash<quint32,quint32>::const_iterator itEntry = m_lRABan.find(oReturnAddr.ip);
		if( itEntry != m_lRABan.end() && time(0) - itEntry.value() < 60 && itEntry.key() != oReturnAddr.ip )
		{
			qDebug() << "Dropping query for return address " << oReturnAddr.toString() << "on hub" << m_oAddress.toString();
			return; // drop excess packets
		}
		m_lRABan.insert(oReturnAddr.ip, time(0));
		qDebug() << "Banning return address" << oReturnAddr.toString() << "on hub " << m_oAddress.toString() << " num banned: " << m_lRABan.size();
	}*/


	if(pPacket->GetRemaining() >= 16)
	{
		oGUID = pPacket->ReadGUID();
		Network.m_oRoutingTable.Add(oGUID, this, false);

		if(m_nType == G2_LEAF)   // temporary
		{
			G2Packet* pQA = G2Packet::New("QA", true);
			quint32 tNow = time(0);
			pQA->WritePacket("TS", 4)->WriteIntLE(tNow);
			pQA->WritePacket("D", (Network.m_oAddress.protocol() == 0 ? 8 : 20))->WriteHostAddress(&Network.m_oAddress);
			quint16 nLeaves = Neighbours.m_nLeavesConnected;
			pQA->WriteIntLE<quint16>(nLeaves);

			quint32 nCount = 0;

			for(uint i = 0; i < HostCache.size() && nCount < 10u; i++, nCount++)
			{
				G2Packet* pS = pQA->WritePacket("S", (HostCache.m_lHosts[i]->m_oAddress.protocol() == 0 ? 10 : 22));
				pS->WriteHostAddress(&HostCache.m_lHosts[i]->m_oAddress);
				pS->WriteIntLE(&HostCache.m_lHosts[i]->m_tTimestamp);
			}

			pQA->WriteByte(0);
			pQA->WriteGUID(oGUID);
			//qDebug() << "Sending query ACK " << pQA->ToHex() << pQA->ToASCII();
			SendPacket(pQA, true, true);
		}
	}
}

qint64 CG2Node::writeToNetwork(qint64 nBytes)
{
	qint64 nTotalSent = 0;

	// send everything left in out buffer
	while( nTotalSent < nBytes && !GetOutputBuffer()->isEmpty() )
	{
		qint64 nSent = CNeighbour::writeToNetwork(nBytes - nTotalSent);
		if( nSent < 0 )
			return nTotalSent;
		nTotalSent += nSent;
	}

	// now if rate controller allows us to send anything more
	// process send queue

	while( nTotalSent < nBytes && !m_lSendQueue.isEmpty() && GetOutputBuffer()->isEmpty() )
	{
		G2Packet* pPacket = m_lSendQueue.dequeue();
		pPacket->ToBuffer(GetOutputBuffer());
		pPacket->Release();

		qint64 nSent = CNeighbour::writeToNetwork(nBytes - nTotalSent);
		if( nSent < 0 )
			return nTotalSent;
		nTotalSent += nSent;
	}

	return nTotalSent;
}
