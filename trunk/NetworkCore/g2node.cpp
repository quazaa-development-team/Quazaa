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

//#define _DISABLE_COMPRESSION

CG2Node::CG2Node(QObject *parent) :
	CCompressedConnection(parent)
{
	m_nState = nsClosed;
	m_nType = G2_UNKNOWN;
	m_tLastPacketIn = m_tLastPacketOut = 0;
	m_tLastPingOut = 0;
	m_nPingsWaiting = 0;
	m_tRTT = 0;
	m_nPacketsIn = m_nPacketsOut = 0;
	m_nLeafCount = m_nLeafMax = 0;
	m_bAcceptDeflate = false;
	m_tLastQuery = 0;
	m_tKeyRequest= 0;
	m_bCachedKeys = false;

	m_pLocalTable = 0;
	m_pRemoteTable = 0;
}

CG2Node::~CG2Node()
{
	while( m_lSendQueue.size() )
		m_lSendQueue.dequeue()->Release();

	if( m_pLocalTable )
		delete m_pLocalTable;

	if( m_pRemoteTable )
		delete m_pRemoteTable;

	Network.RemoveNode(this);
}

void CG2Node::SendPacket(G2Packet* pPacket, bool bBuffered, bool bRelease)
{
	m_nPacketsOut++;

	if( bBuffered )
	{
		pPacket->AddRef(); // FlushSendQueue will release

		if( m_lSendQueue.size() < 128 )
			m_lSendQueue.enqueue(pPacket);
		else
			pPacket->Release();
	}
	else
	{
		//m_lSendQueue.prepend(pPacket);
		pPacket->ToBuffer(GetOutputBuffer());
	}

	if( bRelease )
		pPacket->Release();

	FlushSendQueue(!bBuffered);
}
void CG2Node::FlushSendQueue(bool bFullFlush)
{
	QByteArray* pOutput = GetOutputBuffer();

	while( bytesToWrite() == 0 && m_lSendQueue.size() )
	{
		while( pOutput->size() < 4096 && m_lSendQueue.size() )
		{
			G2Packet* pPacket = m_lSendQueue.dequeue();
			pPacket->ToBuffer(pOutput);
			pPacket->Release();
		}
		emit readyToTransfer();
	}


	if( bFullFlush )
	{
		if( m_bCompressedOutput && pOutput->size() )
			m_bOutputPending = true;
	}

	if( pOutput->size() )
		emit readyToTransfer();
}

void CG2Node::SetupSlots()
{
	connect(this, SIGNAL(connected()), this, SLOT(OnConnect()), Qt::QueuedConnection);
	connect(this, SIGNAL(disconnected()), this, SLOT(OnDisconnect()), Qt::QueuedConnection);
	connect(this, SIGNAL(readyRead()), this, SLOT(OnRead()), Qt::QueuedConnection);
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError(QAbstractSocket::SocketError)), Qt::QueuedConnection);
	connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(OnStateChange(QAbstractSocket::SocketState)), Qt::QueuedConnection);
}

void CG2Node::OnConnect()
{
	QMutexLocker l(&Network.m_pSection);

	systemLog.postLog(LogSeverity::Information, "Connection with %s established, handshaking...", qPrintable(m_oAddress.toString()));

	m_nState = nsHandshaking;
	emit NodeStateChanged();

	QByteArray sHs;

	sHs += "GNUTELLA CONNECT/0.6\r\n";
	sHs += "Accept: application/x-gnutella2\r\n";
	sHs += "User-Agent: " + quazaaGlobals.UserAgentString() + "\r\n";
	sHs += "Remote-IP: " + m_oAddress.toStringNoPort() + "\r\n";
	sHs += "Listen-IP: " + Network.GetLocalAddress().toString() + "\r\n";
	if( Network.isHub() )
		sHs += "X-Ultrapeer: True\r\n";
	else
		sHs += "X-Ultrapeer: False\r\n";
#ifndef _DISABLE_COMPRESSION
	sHs += "Accept-Encoding: deflate\r\n";
#endif

	sHs += "\r\n";

	//qDebug() << "Handshake send:\n" << sHs;

	Write(sHs);
}
void CG2Node::OnDisconnect()
{
	QMutexLocker l(&Network.m_pSection);
	//qDebug("OnDisconnect()");
	systemLog.postLog(LogSeverity::Information, "Connection with %s was dropped: %s", qPrintable(m_oAddress.toString()), qPrintable(m_pSocket->errorString()));
	deleteLater();
}

void CG2Node::OnRead()
{
	if( !Network.m_pSection.tryLock(50) )
	{
		emit readyRead(); // it is a queued connection, lets requeue the missed signal
		return;
	}

	//qDebug() << "CG2Node::OnRead";
	if( m_nState == nsHandshaking )
	{
		if( Peek(bytesAvailable()).indexOf("\r\n\r\n") != -1 )
		{
			if( m_bInitiated )
			{
				ParseOutgoingHandshake();
			}
			else
			{
				ParseIncomingHandshake();
			}
		}
	}
	else if ( m_nState == nsConnected )
	{

		G2Packet* pPacket = 0;
		try
		{
			while( (pPacket = G2Packet::ReadBuffer(GetInputBuffer())) )
			{
				m_tLastPacketIn = time(0);
				m_nPacketsIn++;

				OnPacket(pPacket);

				pPacket->Release();
			}
		}
		catch(...)
		{
			if( pPacket )
			{
				qDebug() << pPacket->ToHex() << "\n" << pPacket->ToASCII();
				pPacket->Release();
			}

			qDebug() << "Packet error - " << m_oAddress.toString().toAscii();
			m_nState = nsClosing;
			emit NodeStateChanged();
			deleteLater();
		}
	}

	Network.m_pSection.unlock();
}
void CG2Node::OnError(QAbstractSocket::SocketError e)
{
	QMutexLocker l(&Network.m_pSection);

	if( e != QAbstractSocket::RemoteHostClosedError )
	{
		HostCache.OnFailure(m_oAddress);
	}

	//systemLog.postLog(tr("Remote host closed connection: ") + m_oAddress.toString() + tr(". Error: ") + m_pSocket->errorString(), LogSeverity::Notice);
	//qDebug() << "OnError(" << e << ")" << m_oAddress.toString();
	deleteLater();
}

void CG2Node::OnStateChange(QAbstractSocket::SocketState s)
{
	Q_UNUSED(s);

	//QMutexLocker l(&Network.m_pSection);

	//qDebug() << "OnStateChange(" << s << ")";
}

void CG2Node::OnTimer(quint32 tNow)
{

	if( m_nState < nsConnected )
	{
		if( tNow - m_tConnected > quazaaSettings.Connection.TimeoutConnect )
		{
			if( m_bInitiated )
				HostCache.OnFailure(m_oAddress);
			m_nState = nsClosing;
			Close();
			return;
		}
	}
	else if( m_nState == nsConnected )
	{
		if( tNow - m_tLastPacketIn > quazaaSettings.Connection.TimeoutTraffic )
		{
			qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "minute dead";
			m_nState = nsClosing;
			emit NodeStateChanged();
			Close();
			return;
		}

		if( m_nPingsWaiting == 0 && (tNow - m_tLastPacketIn >= 30 || tNow - m_tLastPingOut >= quazaaSettings.Gnutella2.PingRate) )
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

		if( m_nPingsWaiting > 0 && tNow - m_tLastPingOut > quazaaSettings.Gnutella2.PingTimeout && tNow - m_tLastPacketIn > quazaaSettings.Connection.TimeoutTraffic )
		{
			qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "ping timed out";
			m_nState = nsClosing;
			emit NodeStateChanged();
			Close();
			return;
		}

		/*if( m_tKeyRequest > 0 && tNow - m_tKeyRequest > 90 )
		{
			qDebug() << "Closing connection with " << m_oAddress.toString().toAscii() << "QueryKey wait timeout reached";
			m_nState = nsClosing;
			emit NodeStateChanged();
			Close();
			return;
		}*/

		if( (m_nType == G2_HUB && tNow - m_tConnected > 60) &&
			((m_pLocalTable != 0 && m_pLocalTable->m_nCookie != QueryHashMaster.m_nCookie) &&
			(tNow - m_pLocalTable->m_nCookie > 60) &&
			(tNow - QueryHashMaster.m_nCookie > 30) ||
			QueryHashMaster.m_nCookie - m_pLocalTable->m_nCookie > 60 ||
			!m_pLocalTable->m_bLive)
			)
		{
			if( m_pLocalTable->PatchTo(&QueryHashMaster, this) )
			{
				systemLog.postLog(LogSeverity::Information, "Sending query routing table to %s (%d bits, %d entries, %d bytes, %d%% full)",  m_oAddress.toStringNoPort().toAscii().constData(), m_pLocalTable->m_nBits, m_pLocalTable->m_nHash, m_pLocalTable->m_nHash / 8, m_pLocalTable->GetPercent());
			}
		}


		FlushSendQueue(true);
	}
	else if( m_nState == nsClosing )
	{
		if( m_nType == G2_UNKNOWN && tNow - m_tConnected > 20 )
		{
			Close();
		}
		else if( tNow - m_tLastPacketIn > 20)
		{
			Close();
		}
	}
}

void CG2Node::ParseIncomingHandshake()
{
	qint32 nIndex = Peek(bytesAvailable()).indexOf("\r\n\r\n");
	QString sHs = Read(nIndex + 4);

	//qDebug() << "Handshake receive:\n" << sHs;

	if( m_sUserAgent.isEmpty() )
		m_sUserAgent = Parser::GetHeaderValue(sHs, "User-Agent");

	if( m_sUserAgent.isEmpty() )
	{
		Send_ConnectError("503 Anonymous clients are not allowed here");
		return;
	}


	if( sHs.startsWith("GNUTELLA CONNECT/0.6") )
	{
		QString sAccept = Parser::GetHeaderValue(sHs, "Accept");
		bool bAcceptG2 = sAccept.contains("application/x-gnutella2");

		if( !bAcceptG2 )
		{
			Send_ConnectError("503 Required network not accepted");
			return;
		}

#ifndef _DISABLE_COMPRESSION
		m_bAcceptDeflate = false;
		QString sAcceptEnc = Parser::GetHeaderValue(sHs, "Accept-Encoding");
		if( sAcceptEnc.contains("deflate") && Network.isHub() )
		{
			m_bAcceptDeflate = true;
		}
#endif

		QString sUltra = Parser::GetHeaderValue(sHs, "X-Ultrapeer").toLower();
		//QString sUltraNeeded = Parser::GetHeaderValue(sHs, "X-Ultrapeer-Needed").toLower();
		if( sUltra.isEmpty() )
		{
			Send_ConnectError("503 No hub mode specified");
			return;
		}

		QString sRemoteIP = Parser::GetHeaderValue(sHs, "Remote-IP");
		if( !sRemoteIP.isEmpty() )
			Network.AcquireLocalAddress(sRemoteIP);
		else
		{
			Send_ConnectError("503 Remote-IP header missing");
			return;
		}

		bool bUltra = (sUltra == "true");

		if( bUltra )
		{
			if( !Network.NeedMore(G2_HUB) )
			{
				Send_ConnectError("503 Maximum hub connections reached");
				return;
			}
			m_nType = G2_HUB;
		}
		else
		{
			if( !Network.NeedMore(G2_LEAF) )
			{
				Send_ConnectError("503 Maximum leaf connections reached");
				return;
			}
			m_nType = G2_LEAF;
		}

		Send_ConnectOK(false, m_bAcceptDeflate);

	}
	else if( sHs.contains(" 200 OK") )
	{
		QString sContentType = Parser::GetHeaderValue(sHs, "Content-Type");
		bool bG2Provided = sContentType.contains("application/x-gnutella2");

		if( !bG2Provided )
		{
			Send_ConnectError("503 Required network not provided");
			return;
		}

#ifndef _DISABLE_COMPRESSION
		QString sContentEnc = Parser::GetHeaderValue(sHs, "Content-Encoding");
		if( sContentEnc.contains("deflate") )
		{
			if( !EnableInputCompression() )
			{
				qDebug() << "Inflate init error!";
				abort();
				deleteLater();
				return;
			}
		}

		if( m_bAcceptDeflate )
		{
			if( !EnableOutputCompression() )
			{
				qDebug() << "Deflate init error!";
				abort();
				deleteLater();
				return;
			}
		}
#endif

		m_nState = nsConnected;
		emit NodeStateChanged();

		SendStartups();
		m_tLastPacketIn = m_tLastPacketOut = time(0);
		if( m_nType == G2_HUB )
		{
			m_pLocalTable = new CQueryHashTable();
		}


	}
	else
	{
		qDebug() << "Connection rejected: " << sHs.left(sHs.indexOf("\r\n"));
		m_nState = nsClosing;
		emit NodeStateChanged();
		Close();
	}
}

void CG2Node::ParseOutgoingHandshake()
{
	QString sHs = Read(Peek(bytesAvailable()).indexOf("\r\n\r\n") + 4);

	//qDebug() << "Handshake receive:\n" << sHs;

	QString sAccept = Parser::GetHeaderValue(sHs, "Accept");
	bool bAcceptG2 = sAccept.contains("application/x-gnutella2");

	if( !bAcceptG2 )
	{
		Send_ConnectError("503 Required network not accepted");
		return;
	}

	QString sContentType = Parser::GetHeaderValue(sHs, "Content-Type");
	bool bG2Provided = sContentType.contains("application/x-gnutella2");

	if( !bG2Provided )
	{
		Send_ConnectError("503 Required network not provided");
		return;
	}

	QString sTry = Parser::GetHeaderValue(sHs, "X-Try-Hubs");
	if( bAcceptG2 && bG2Provided && sTry.size() )
	{
		HostCache.AddXTry(sTry);
	}

	if( sHs.left(16) != "GNUTELLA/0.6 200" )
	{
		qDebug() << "Connection rejected: " << sHs.left(sHs.indexOf("\r\n"));
		Close();
		return;
	}

	m_sUserAgent = Parser::GetHeaderValue(sHs, "User-Agent");

	if( m_sUserAgent.isEmpty() )
	{
		Send_ConnectError("503 Anonymous clients are not allowed here");
		return;
	}

	QString sRemoteIP = Parser::GetHeaderValue(sHs, "Remote-IP");
	if( !sRemoteIP.isEmpty() )
		Network.AcquireLocalAddress(sRemoteIP);
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
	if( sContentEnc.contains("deflate") )
	{
		if( !EnableInputCompression() )
		{
			qDebug() << "Inflate init error!";
			Close();
			deleteLater();
			return;
		}
	}
#endif

	bool bAcceptDeflate = false;
#ifndef _DISABLE_COMPRESSION
	QString sAcceptEnc = Parser::GetHeaderValue(sHs, "Accept-Encoding");
	if( sAcceptEnc.contains("deflate") && Network.isHub() )
	{
		bAcceptDeflate = true;
	}
#endif

	if( bUltra )
	{
		if( !Network.NeedMore(G2_HUB) )
		{
			Send_ConnectError("503 Maximum hub connections reached");
			return;
		}
		m_nType = G2_HUB;
	}
	else
	{
		if( !Network.NeedMore(G2_LEAF) )
		{
			Send_ConnectError("503 Maximum leaf connections reached");
			return;
		}
		m_nType = G2_LEAF;
	}

	Send_ConnectOK(true, bAcceptDeflate);

#ifndef _DISABLE_COMPRESSION
	if( bAcceptDeflate )
	{
		if( !EnableOutputCompression() )
		{
			qDebug() << "Deflate init error!";
			Close();
			deleteLater();
			return;
		}
	}
#endif

	m_nState = nsConnected;
	emit NodeStateChanged();

	SendStartups();
	m_tLastPacketIn = m_tLastPacketOut = time(0);

	if( m_nType == G2_HUB )
	{
		m_pLocalTable = new CQueryHashTable();
	}

	systemLog.postLog(LogSeverity::Information, "Gnutella2 connection with %s established.", qPrintable(m_oAddress.toString()));
}
void CG2Node::Send_ConnectError(QString sReason)
{
	systemLog.postLog(LogSeverity::Information, "Rejecting connection with %s: %s", qPrintable(m_oAddress.toString()), qPrintable(sReason));

	qDebug() << "Rejecting connection:" << sReason;

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
	if( Network.isHub() )
		sHs += "X-Ultrapeer: True\r\n";
	else
		sHs += "X-Ultrapeer: False\r\n";
	sHs += "Content-Type: application/x-gnutella2\r\n";

	if( !bReply )
	{
		// 2-handshake
#ifndef _DISABLE_COMPRESSION
		if( Network.isHub() && m_nType == G2_HUB )
			sHs += "Accept-Encoding: deflate\r\n";

		if( bDeflated )
		{
			sHs += "Content-Encoding: deflate\r\n";
		}
#endif
		sHs += "Accept: application/x-gnutella2\r\n";
		sHs += "Remote-IP: ";
		sHs += m_oAddress.toStringNoPort();
		sHs += "\r\n";
		sHs += "Listen-IP: " + Network.GetLocalAddress().toString() + "\r\n";
	}
	else
	{
#ifndef _DISABLE_COMPRESSION
		if( bDeflated )
			sHs += "Content-Encoding: deflate\r\n";
#endif
	}

	sHs += "\r\n";

	//qDebug() << "Handshake send:\n" << sHs;

	Write(sHs);

}

void CG2Node::SendStartups()
{
	if( Network.IsListening() )
	{
		IPv4_ENDPOINT addr = Network.GetLocalAddress();
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
	pLNI->WritePacket("NA", 6)->WriteHostAddress(&Network.m_oAddress);
	pLNI->WritePacket("GU", 16)->WriteGUID(quazaaSettings.Profile.GUID);
	pLNI->WritePacket("V", 4)->WriteString(quazaaGlobals.VendorCode(), false);

	if( Network.isHub() )
	{
		quint16 nLeafs = quazaaSettings.Gnutella2.NumLeafs;
		pLNI->WritePacket("HS", 4);
		pLNI->WriteIntLE(Network.m_nLeavesConnected);
		pLNI->WriteIntLE(nLeafs);
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
		if( !Network.RoutePacket(pPacket) )
		{

			if( pPacket->IsType("PI") )
			{
				OnPing(pPacket);
			}
			else if( pPacket->IsType("PO") )
			{
				OnPong(pPacket);
			}
			else if( pPacket->IsType("LNI") )
			{
				OnLNI(pPacket);
			}
			else if( pPacket->IsType("KHL") )
			{
				OnKHL(pPacket);
			}
			else if( pPacket->IsType("QHT") )
			{
				//OnQHT(pPacket);
			}
			else if( pPacket->IsType("Q2") )
			{
				OnQuery(pPacket);
			}
			else if( pPacket->IsType("QKR") )
			{
				OnQKR(pPacket);
			}
			else if( pPacket->IsType("QKA") )
			{
				OnQKA(pPacket);
			}
			else if( pPacket->IsType("QA") )
			{
				OnQA(pPacket);
			}
			else if( pPacket->IsType("QH2") )
			{
				OnQH2(pPacket);
			}
			else
			{
				qDebug() << "Unknown packet " << pPacket->GetType();
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
	IPv4_ENDPOINT addr;

	if( pPacket->m_bCompound )
	{
		char szType[9];
		quint32 nLength = 0, nNext = 0;

		while( pPacket->ReadPacket(&szType[0], nLength) )
		{
			nNext = pPacket->m_nPosition + nLength;

			if( strcmp("UDP", szType) == 0 && nLength >= 6)
			{
				pPacket->ReadHostAddress(&addr);
				if( addr.ip != 0 && addr.port != 0)
					bUdp = true;
			}
			else if( strcmp("RELAY", szType) == 0 )
			{
				bRelay = true;
			}
			else if( strcmp("TFW", szType) == 0 )
			{
				bTestFirewall = true;
			}

			pPacket->m_nPosition = nNext;
		}

	}

	if( !bUdp && !bRelay )
	{
		// direct ping
		G2Packet* pPong = G2Packet::New("PO", false);
		SendPacket(pPong, false, true);
		return;
	}

	if( bUdp && !bRelay )
	{
		// /PI/UDP

		if( Network.isHub() )
		{
			char* pRelay = pPacket->WriteGetPointer(7, 0);
			*pRelay++ = 0x60;
			*pRelay++ = 0;
			*pRelay++ = 'R'; *pRelay++ = 'E'; *pRelay++ = 'L';
			*pRelay++ = 'A'; *pRelay++ = 'Y';


			QList<CG2Node*> lToRelay;

			for( int i = 0; i < Network.m_lNodes.size(); i++ )
			{
				if( Network.m_lNodes.at(i)->m_nState == nsConnected )
					lToRelay.append(Network.m_lNodes[i]);
			}

			for( int nCount = 0; nCount < quazaaSettings.Gnutella2.PingRelayLimit && lToRelay.size(); nCount++ )
			{
				int nIndex = qrand() % lToRelay.size();
				pPacket->AddRef();
				CG2Node* pNode = lToRelay.at(nIndex);
				pNode->SendPacket(pPacket, true, true);
				lToRelay.removeAt(nIndex);
			}

			return;
		}
	}

	if( bUdp && bRelay )
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

   if( m_nPingsWaiting > 0 )
   {
	   m_nPingsWaiting--;

	   if( m_nPingsWaiting == 0 )
	   {
			m_tRTT = m_tRTTTimer.elapsed();
	   }
   }
}

void CG2Node::OnLNI(G2Packet* pPacket)
{
	if( !pPacket->m_bCompound )
		return;

	bool hasNA = false;
	QUuid pGUID;
	bool hasGUID = false;

	IPv4_ENDPOINT hostAddr;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while( pPacket->ReadPacket(&szType[0], nLength) )
	{
		nNext = pPacket->m_nPosition + nLength;

		if( strcmp("NA", szType) == 0 && nLength >= 6 )
		{
			pPacket->ReadHostAddress(&hostAddr);
			if( hostAddr.ip != 0 && hostAddr.port != 0 )
			{
				hasNA = true;
				if( !m_bInitiated )
					m_oAddress.ip = hostAddr.ip;
				m_oAddress.port = hostAddr.port;
			}

		}
		else if( strcmp("GU", szType) == 0 && nLength >= 16 )
		{
			hasGUID = true;
			pGUID = pPacket->ReadGUID();
		}
		else if( strcmp("HS", szType) == 0 && nLength >= 4 )
		{
			if( m_nType == G2_HUB )
			{
				pPacket->ReadIntLE(&m_nLeafCount);
				pPacket->ReadIntLE(&m_nLeafMax);
			}
		}
		else if( strcmp("QK", szType) == 0 )
		{
			m_bCachedKeys = true;
		}
		else if( strcmp("g2core", szType) == 0 )
		{
			m_bG2Core = true;
		}

		pPacket->m_nPosition = nNext;
	}

	if( hasNA && hasGUID )
	{
		Network.m_oRoutingTable.Add(pGUID, this, true);
	}

}
void CG2Node::OnKHL(G2Packet* pPacket)
{
	if( !pPacket->m_bCompound )
		return;

	quint32 tNow = time(0);
	quint32 nTimestamp = tNow;
	qint64 nDiff = 0;

	char szType[9], szInner[9];
	quint32 nLength = 0, nInnerLength = 0;
	bool bCompound = false;
	quint32 nNext = 0, nInnerNext = 0;

	while( pPacket->ReadPacket(&szType[0], nLength, &bCompound) )
	{
		nNext = pPacket->m_nPosition + nLength;

		if( strcmp("NH", szType) == 0 && bCompound )
		{
			QUuid pGUID;

			while( pPacket->m_nPosition < nNext && pPacket->ReadPacket(&szInner[0], nInnerLength) )
			{
				nInnerNext = pPacket->m_nPosition + nInnerLength;

				if( strcmp("GU", szInner) && nInnerLength >= 16 )
				{
					pGUID = pPacket->ReadGUID();
				}

				pPacket->m_nPosition = nInnerNext;
			}

			if( !pGUID.isNull() && nLength >= 6 )
			{
				IPv4_ENDPOINT pAddr;
				pPacket->ReadHostAddress(&pAddr);

				Network.m_oRoutingTable.Add(pGUID, this, &pAddr, false);
			}
		}
		else if( strcmp("CH", szType) == 0 )
		{
			if( bCompound )
				pPacket->SkipCompound(nLength);

			if( nLength >= 10 )
			{
				IPv4_ENDPOINT ip4;
				quint32 nTs = 0;
				pPacket->ReadHostAddress(&ip4);
				pPacket->ReadIntLE(&nTs);

				HostCache.Add(ip4, tNow + nDiff);
			}
		}
		else if( strcmp("TS", szType) == 0 )
		{
			if( bCompound )
				pPacket->SkipCompound(nLength);

			if( nLength >= 4 )
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
	Q_UNUSED(pPacket);

	if( !Network.isHub() )
	{
		qDebug() << "Received unexpected Query Routing Table, ignoring";
		return;
	}
}

void CG2Node::OnQKR(G2Packet *pPacket)
{
	if( !pPacket->m_bCompound || m_nType != G2_LEAF )
		return;

	char szType[9];
	quint32 nLength = 0, nNext = 0;
	bool bCacheOK = true;
	IPv4_ENDPOINT addr;

	while( pPacket->ReadPacket(&szType[0], nLength) )
	{
		nNext = pPacket->m_nPosition + nLength;

		if( strcmp("QNA", szType) == 0 && nLength >= 6 )
		{
			pPacket->ReadHostAddress(&addr);
		}
		else if( strcmp("REF", szType) == 0 )
		{
			bCacheOK = false;
		}

		pPacket->m_nPosition = nNext;
	}

	if( addr.ip == 0 || addr.port == 0 ) // TODO: sprawdzene czy adres jest za fw
		return;

	CHostCacheHost* pHost = bCacheOK ? HostCache.Find(addr) : 0;

	if( pHost != 0 && pHost->m_nQueryKey != 0 && pHost->m_nKeyHost == Network.m_oAddress.ip && time(0) - pHost->m_nKeyTime < quazaaSettings.Gnutella2.QueryKeyTime )
	{
		G2Packet* pQKA = G2Packet::New("QKA", true);
		pQKA->WritePacket("QNA", 6)->WriteHostAddress(&addr);
		pQKA->WritePacket("QK", 4)->WriteIntBE(pHost->m_nQueryKey);
		pQKA->WritePacket("CACHED", 0);
		SendPacket(pQKA, true, true);
	}
	else
	{
		G2Packet* pQKR = G2Packet::New("QKR", true);
		pQKR->WritePacket("SNA", 6)->WriteHostAddress(&m_oAddress);
		Datagrams.SendPacket(addr, pQKR, false);
		pQKR->Release();
	}
}

void CG2Node::OnQKA(G2Packet *pPacket)
{
	if( !pPacket->m_bCompound )
		return;

	m_tKeyRequest = 0;

	quint32 nKey;
	IPv4_ENDPOINT addr;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while( pPacket->ReadPacket(&szType[0], nLength) )
	{
		nNext = pPacket->m_nPosition + nLength;

		if( strcmp("QK", szType) == 0 && nLength >= 4 )
		{
			pPacket->ReadIntLE(&nKey);
		}
		else if( strcmp("QNA", szType) == 0 )
		{
			if( nLength >= 6 )
			{
				pPacket->ReadHostAddress(&addr);
			}
			else if( nLength >= 4 )
			{
				pPacket->ReadIntBE(&addr.ip);
				addr.port = 6346;
			}
		}

		pPacket->m_nPosition = nNext;
	}

	CHostCacheHost* pCache = HostCache.Add(addr, 0);
	if( pCache )
	{
		pCache->SetKey(nKey, &m_oAddress);

		qDebug("Got a query key from %s via %s = 0x%x", addr.toString().toAscii().constData(), m_oAddress.toString().toAscii().constData(), nKey);
	}
}
void CG2Node::OnQA(G2Packet *pPacket)
{
   QUuid oGUID;
   SearchManager.OnQueryAcknowledge(pPacket, m_oAddress, oGUID);

   // TCP /QA - no need for routing, it's either for us or to be dropped
}

void CG2Node::OnQH2(G2Packet *pPacket)
{
	if( !pPacket->m_bCompound )
		return;

	SearchManager.OnQueryHit(pPacket, this);
}
void CG2Node::OnQuery(G2Packet *pPacket)
{
	if( !pPacket->m_bCompound )
		return;

	// just read guid for now to have it in routing table

	QUuid oGUID;
	if( pPacket->m_bCompound )
		pPacket->SkipCompound();

	if( pPacket->GetRemaining() >= 16 )
	{
		oGUID = pPacket->ReadGUID();
		Network.m_oRoutingTable.Add(oGUID, this, false);

		if( m_nType == G2_LEAF ) // temporary
		{
			G2Packet* pQA = G2Packet::New("QA", true);
			quint32 tNow = time(0);
			pQA->WritePacket("TS", 4)->WriteIntLE(tNow);
			pQA->WritePacket("D", 8)->WriteHostAddress(&Network.m_oAddress);
			pQA->WriteIntLE(Network.m_nLeavesConnected);

			quint32 nCount = 0;

			for( uint i = 0; i < HostCache.size() && nCount < 100u; i++, nCount++ )
			{
				pQA->WritePacket("S", 6)->WriteHostAddress(&HostCache.m_lHosts[i]->m_oAddress);
			}

			pQA->WriteByte(0);
			pQA->WriteGUID(oGUID);
			//qDebug() << "Sending query ACK " << pQA->ToHex() << pQA->ToASCII();
			SendPacket(pQA, true, true);
		}
	}
}
