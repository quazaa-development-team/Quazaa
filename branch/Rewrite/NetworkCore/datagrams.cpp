//
// datagrams.cpp
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

#include "datagrams.h"
#include "network.h"
#include "neighbours.h"
#include "datagramfrags.h"
#include "g2node.h"
#include "g2packet.h"
#include <QTimer>
#include "hostcache.h"
#include "searchmanager.h"
#include "queryhit.h"
#include "systemlog.h"

#include "quazaaglobals.h"
#include "quazaasettings.h"

#include "thread.h"
#include "buffer.h"


CDatagrams Datagrams;
CThread DatagramsThread;

CDatagrams::CDatagrams()
{
	m_nUploadLimit = 8192; // TODO it

	m_pRecvBuffer = new CBuffer();
	m_pHostAddress = new QHostAddress();
	m_nSequence = 0;

	m_bActive = false;

	m_pSocket = 0;
	m_tSender = 0;
	m_bFirewalled = true;

}
CDatagrams::~CDatagrams()
{
	if(m_bActive)
	{
		Disconnect();
	}

	if(m_pSocket)
	{
		delete m_pSocket;
	}

	if(m_tSender)
	{
		delete m_tSender;
	}

	if(m_pRecvBuffer)
	{
		delete m_pRecvBuffer;
	}
	if(m_pHostAddress)
	{
		delete m_pHostAddress;
	}
}

void CDatagrams::SetupThread()
{
	systemLog.postLog(tr("Setup Thread"), LogSeverity::Debug);
	//qDebug() << "SetupThread";
	systemLog.postLog(tr("Thread id: %1").arg((quint32)QThread::currentThreadId()), LogSeverity::Debug);
	//qDebug() << "Thread id: " << QThread::currentThreadId();

	Q_ASSERT(m_pSocket == 0);
	Q_ASSERT(m_tSender == 0);

	m_pSocket = new QUdpSocket(this);
	m_tSender = new QTimer(this);

	CEndPoint addr = Network.GetLocalAddress();
	if(m_pSocket->bind(addr.port()))
	{
		systemLog.postLog(tr("Datagrams listening on %1 %2").arg(m_pSocket->localPort()).arg(addr.port()), LogSeverity::Debug);
		//qDebug() << "Datagrams listening on " << m_pSocket->localPort() << addr.port();
		m_nDiscarded = 0;

		for(int i = 0; i < quazaaSettings.Gnutella2.UdpBuffers; i++)
		{
			m_FreeBuffer.append(new CBuffer(1024));
		}

		for(int i = 0; i < quazaaSettings.Gnutella2.UdpInFrames; i++)
		{
			m_FreeDGIn.append(new DatagramIn);
		}

		for(int i = 0; i < quazaaSettings.Gnutella2.UdpOutFrames; i++)
		{
			m_FreeDGOut.append(new DatagramOut);
		}

		connect(this, SIGNAL(SendQueueUpdated()), this, SLOT(FlushSendCache()), Qt::QueuedConnection);
		connect(m_tSender, SIGNAL(timeout()), this, SLOT(FlushSendCache()), Qt::QueuedConnection);
		connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(OnDatagram()), Qt::QueuedConnection);

		m_tSender->setInterval(200);

		m_bActive = true;
		m_tMeterTimer.start();
	}
	else
	{
		systemLog.postLog(tr("Can't bind UDP socket! UDP communication disabled!"), LogSeverity::Debug);
		//qDebug() << "Can't bind UDP socket! UDP communication disabled!";
		Disconnect();
	}
}

void CDatagrams::Listen()
{
	QMutexLocker l(&m_pSection);

	if(m_bActive)
	{
		systemLog.postLog(tr("CDatagrams::Listen - already listening"), LogSeverity::Debug);
		//qDebug() << "CDatagrams::Listen - already listening";
		return;
	}

	//SetupThread();
	m_bFirewalled = true;
	DatagramsThread.start("Datagrams", &m_pSection, this);

}

void CDatagrams::CleanupThread()
{
	if(m_tSender)
	{
		m_tSender->stop();
		m_tSender->disconnect();
		delete m_tSender;
		m_tSender = 0;
	}

	if(m_pSocket)
	{
		m_pSocket->close();
		m_pSocket->disconnect();
		delete m_pSocket;
		m_pSocket = 0;
	}

	disconnect(SIGNAL(SendQueueUpdated()));

	while(!m_SendCache.isEmpty())
	{
		Remove(m_SendCache.first());
	}

	while(!m_RecvCacheTime.isEmpty())
	{
		Remove(m_RecvCacheTime.first());
	}

	while(!m_FreeDGIn.isEmpty())
	{
		delete m_FreeDGIn.takeFirst();
	}

	while(!m_FreeDGOut.isEmpty())
	{
		delete m_FreeDGOut.takeFirst();
	}

	while(!m_FreeBuffer.isEmpty())
	{
		delete m_FreeBuffer.takeFirst();
	}

	while(!m_lPendingQA.isEmpty())
	{
		m_lPendingQA.takeFirst().second->Release();
	}

	while(!m_lPendingQH2.isEmpty())
	{
		QueuedQueryHit pHit = m_lPendingQH2.takeFirst();
		pHit.m_pPacket->Release();
		delete pHit.m_pInfo;
	}

	while(!m_lPendingQKA.isEmpty())
	{
		m_lPendingQKA.takeFirst().second->Release();
	}
}

void CDatagrams::Disconnect()
{
	QMutexLocker l(&m_pSection);

	m_bActive = false;

	//CleanupThread();
	DatagramsThread.exit(0);
}

void CDatagrams::OnDatagram()
{
	if(!m_bActive)
	{
		return;
	}

	quint32 nCounter = 500;

	while(m_pSocket->hasPendingDatagrams() && nCounter--)
	{
		qint64 nSize = m_pSocket->pendingDatagramSize();
		m_pRecvBuffer->resize(nSize);
		qint64 nReadSize = m_pSocket->readDatagram(m_pRecvBuffer->data(), nSize, m_pHostAddress, &m_nPort);

		m_nBandwidthIn += nReadSize;

		if(nReadSize < 8)
		{
			continue;
		}
		//return;

		GND_HEADER* pHeader = (GND_HEADER*)m_pRecvBuffer->data();
		if(strncmp((char*)&pHeader->szTag, "GND", 3) == 0 && pHeader->nPart > 0 && (pHeader->nCount == 0 || pHeader->nPart <= pHeader->nCount))
		{
			if(pHeader->nCount == 0)
			{
				// ACK
				OnAcknowledgeGND();
			}
			else
			{
				// DG
				OnReceiveGND();
			}
		}
	}
}

void CDatagrams::OnReceiveGND()
{

	GND_HEADER* pHeader = (GND_HEADER*)m_pRecvBuffer->data();
	QHostAddress nIp = *m_pHostAddress;
	quint32 nSeq = ((pHeader->nSequence << 16) & 0xFFFF0000) + (m_nPort & 0x0000FFFF);

	//qDebug("Received GND from %s:%u nSequence = %u nPart = %u nCount = %u", m_pHostAddress->toString().toAscii().constData(), m_nPort, pHeader->nSequence, pHeader->nPart, pHeader->nCount);
	DatagramIn* pDG = 0;

	if(m_RecvCache.contains(nIp) && m_RecvCache[nIp].contains(nSeq))
	{
		pDG = m_RecvCache[nIp][nSeq];

		// Zeby dac szanse wiekszym pakietom ;)
		if(pDG->m_nLeft)
		{
			pDG->m_tStarted = time(0);
		}
	}
	else
	{
		QMutexLocker l(&m_pSection);

		if(!m_FreeDGIn.isEmpty())
		{
			pDG = m_FreeDGIn.takeFirst();
		}
		else
		{
			if(m_FreeDGIn.isEmpty())
			{
				RemoveOldIn(true);
				if(m_FreeDGIn.isEmpty())
				{
					systemLog.postLog(tr("UDP in frames exhausted"), LogSeverity::Debug);
					//qDebug() << "UDP in frames exhausted";
					m_nDiscarded++;
					return;
				}
			}

			pDG = m_FreeDGIn.takeFirst();
		}

		if(m_FreeBuffer.size() < pHeader->nCount)
		{
			m_nDiscarded++;
			m_FreeDGIn.append(pDG);
			RemoveOldIn(false);
			return;
		}

		pDG->Create(CEndPoint(*m_pHostAddress, m_nPort), pHeader->nFlags, pHeader->nSequence, pHeader->nCount);

		for(int i = 0; i < pHeader->nCount; i++)
		{
			Q_ASSERT(pDG->m_pBuffer[i] == 0);
			pDG->m_pBuffer[i] = m_FreeBuffer.takeFirst();
		}

		m_RecvCache[nIp][nSeq] = pDG;
		m_RecvCacheTime.prepend(pDG);
	}

	// Dopiero tutaj, na wypadek gdybysmy nie mieli wolnych datagramow
	// ACK = odebralismy datagram, a jesli odebralismy i odrzucilismy to nie wysylamy ACK-a
	if(pHeader->nFlags & 0x02)
	{
		GND_HEADER oAck;

		memcpy(&oAck, pHeader, sizeof(GND_HEADER));
		oAck.nCount = 0;
		oAck.nFlags = 0;
		//qDebug() << "Sending UDP ACK to " << m_pHostAddress->toString() << m_nPort;
		m_pSocket->writeDatagram((char*)&oAck, sizeof(GND_HEADER), *m_pHostAddress, m_nPort);
	}

	if(pDG->Add(pHeader->nPart, m_pRecvBuffer->data() + sizeof(GND_HEADER), m_pRecvBuffer->size() - sizeof(GND_HEADER)))
	{

		G2Packet* pPacket = 0;
		try
		{
			CEndPoint addr(*m_pHostAddress, m_nPort);
			pPacket = pDG->ToG2Packet();
			if(pPacket)
			{
				OnPacket(addr, pPacket);
			}

		}
		catch(...)
		{

		}
		if(pPacket)
		{
			pPacket->Release();
		}

		m_pSection.lock();
		Remove(pDG, true);
		m_pSection.unlock();
	}

}

void CDatagrams::OnAcknowledgeGND()
{
	GND_HEADER* pHeader = (GND_HEADER*)m_pRecvBuffer->data();

	//qDebug() << "UDP received GND ACK from" << m_pHostAddress->toString().toAscii().constData() << "seq" << pHeader->nSequence << "part" << pHeader->nPart;

	if(!m_SendCacheMap.contains(pHeader->nSequence))
	{
		return;
	}

	DatagramOut* pDG = m_SendCacheMap.value(pHeader->nSequence);

	if(pDG->Acknowledge(pHeader->nPart))
	{
		Remove(pDG);
	}
}

void CDatagrams::Remove(DatagramIn* pDG, bool bReclaim)
{
	for(int i = 0; i < pDG->m_nCount; i++)
	{
		if(pDG->m_pBuffer[i])
		{
			m_FreeBuffer.append(pDG->m_pBuffer[i]);
			pDG->m_pBuffer[i]->clear();
			pDG->m_pBuffer[i] = 0;
		}
	}

	if(bReclaim)
	{
		return;
	}

	if(m_RecvCache.contains(pDG->m_oAddress))
	{
		quint32 nSeq = ((pDG->m_nSequence << 16) & 0xFFFF0000) + (pDG->m_oAddress.port() & 0x0000FFFF);
		Q_ASSERT(pDG == m_RecvCache[pDG->m_oAddress][nSeq]);
		m_RecvCache[pDG->m_oAddress].remove(nSeq);
		if(m_RecvCache[pDG->m_oAddress].isEmpty())
		{
			m_RecvCache.remove(pDG->m_oAddress);
		}

		m_RecvCacheTime.removeOne(pDG);
		m_FreeDGIn.append(pDG);
	}

	/*
	bool bFound = false;

	for( QHash<quint32, QHash<quint32, DatagramIn*> >::iterator it = m_RecvCache.begin(); it != m_RecvCache.end(); it++ )
	{
		for( QHash<quint32, DatagramIn*>::iterator it2 = it.value().begin(); it2 != it.value().end(); it2++ )
		{
			if( it2.value() == pDG )
			{
				bFound = true;
				m_RecvCache[it.key()].remove(it2.key());
				m_FreeDGIn.push(pDG);
				break;
			}
		}

		if( bFound )
		{
			if( it.value().size() == 0 )
			{
				m_RecvCache.remove(it.key());
				break;
			}
		}
	}*/
}

// Usuwa jeden pakiet z cache'u odbioru
void CDatagrams::RemoveOldIn(bool bForce)
{
	//DatagramIn* pOldest = 0;
	quint32 tNow = time(0);
	bool bRemoved = false;

	while(!m_RecvCacheTime.isEmpty() && (tNow - m_RecvCacheTime.back()->m_tStarted > quazaaSettings.Gnutella2.UdpInExpire || m_RecvCacheTime.back()->m_nLeft == 0))
	{
		Remove(m_RecvCacheTime.back());
		bRemoved = true;
	}

	if(bForce && !bRemoved && !m_RecvCacheTime.isEmpty())
	{
		for(QLinkedList<DatagramIn*>::iterator itPacket = m_RecvCacheTime.begin(); itPacket != m_RecvCacheTime.end(); ++itPacket)
		{
			if((*itPacket)->m_nLeft == 0)
			{
				Remove(*itPacket);
				break;
			}
		}
	}

	/*for( QHash<quint32, QHash<quint32, DatagramIn*> >::iterator it = m_RecvCache.begin(); it != m_RecvCache.end(); it++ )
	{
		for( QHash<quint32, DatagramIn*>::iterator itPacket = it.value().begin(); itPacket != it.value().end(); itPacket++ )
		{
			if( pOldest )
			{
				if( pOldest->m_tStarted < itPacket.value()->m_tStarted && (bForce || tNow - itPacket.value()->m_tStarted > quazaaSettings.Gnutella2.UdpInExpire || itPacket.value()->m_nLeft == 0) )
				{
					pOldest = itPacket.value();
				}
			}
			else
			{
				if( bForce || tNow - itPacket.value()->m_tStarted > quazaaSettings.Gnutella2.UdpInExpire || itPacket.value()->m_nLeft == 0 )
				{
					pOldest = itPacket.value();
				}

			}
		}
	}

	if( pOldest )
	{
		Remove(pOldest);
	}*/


}

void CDatagrams::Remove(DatagramOut* pDG)
{
	m_SendCacheMap.remove(pDG->m_nSequence);
	m_SendCache.removeOne(pDG);
	m_FreeDGOut.append(pDG);

	if(pDG->m_pBuffer)
	{
		m_FreeBuffer.append(pDG->m_pBuffer);
		pDG->m_pBuffer->clear();
		pDG->m_pBuffer = 0;
	}
}

void CDatagrams::FlushSendCache()
{
	if(!m_bActive)
	{
		return;
	}

	QMutexLocker l(&m_pSection);

	quint32 tNow = time(0);

	qint32 nMsecs = 1000;
	if(!m_tStopWatch.isNull())
	{
		nMsecs = qMin(nMsecs, m_tStopWatch.elapsed());
	}

	quint32 nToWrite = (m_nUploadLimit * nMsecs) / 1000;

	QHostAddress nLastHost;

	while(nToWrite > 0 && !m_SendCache.isEmpty())
	{
		bool bSent = false;

		char* pPacket;
		quint32 nPacket;

		for(QLinkedList<DatagramOut*>::iterator itPacket = m_SendCache.begin(); itPacket != m_SendCache.end(); ++itPacket)
		{
			DatagramOut* pDG = *itPacket;

			if(pDG->m_oAddress == nLastHost)
			{
				continue;
			}

			// TODO: sprawdzenie UDP na firewallu - mog? by? 3 stany udp
			if(pDG->GetPacket(tNow, &pPacket, &nPacket, true))
			{
				//qDebug() << "UDP sending to " << pDG->m_oAddress.toString().toAscii().constData() << "seq" << pDG->m_nSequence << "nPart" << ((GND_HEADER*)&pPacket)->nPart << "count" << pDG->m_nCount;

				m_pSocket->writeDatagram(pPacket, nPacket, pDG->m_oAddress, pDG->m_oAddress.port());

				nLastHost = pDG->m_oAddress;

				if(nToWrite >= nPacket)
				{
					nToWrite -= nPacket;
				}
				else
				{
					nToWrite = 0;
				}

				m_nBandwidthOut += nPacket;

				if(!pDG->m_bAck)
				{
					Remove(pDG);
				}

				bSent = true;

				break;
			}
		}

		if(m_SendCache.isEmpty() || !bSent)
		{
			break;
		}
	}

	while(!m_SendCache.isEmpty() && tNow - m_SendCache.back()->m_tSent > quazaaSettings.Gnutella2.UdpOutExpire)
	{
		Remove(m_SendCache.back());
	}

}

void CDatagrams::SendPacket(CEndPoint& oAddr, G2Packet* pPacket, bool bAck, DatagramWatcher* pWatcher, void* pParam)
{
	if(!m_bActive)
	{
		return;
	}

	QMutexLocker l(&m_pSection);

	Q_UNUSED(pWatcher);
	Q_UNUSED(pParam);

	if(m_FreeDGOut.isEmpty())
	{
		Remove(m_SendCache.last());
		systemLog.postLog(tr("UDP out frames exhausted"), LogSeverity::Debug);
		//qDebug() << "UDP out frames exhausted";
	}

	if(m_FreeBuffer.isEmpty())
	{
		RemoveOldIn(false);

		if(m_FreeBuffer.isEmpty())
		{
			systemLog.postLog(tr("UDP out discarded, out of buffers"), LogSeverity::Debug);
			//qDebug() << "UDP out discarded, out of buffers";
			return;
		}
	}

	DatagramOut* pDG = m_FreeDGOut.takeFirst();
	pDG->Create(oAddr, pPacket, m_nSequence++, m_FreeBuffer.takeFirst(), bAck);

	m_SendCache.prepend(pDG);
	m_SendCacheMap[pDG->m_nSequence] = pDG;

	// TODO: Powiadomienia do obiektow nasluchujacych, jesli podano

	//qDebug() << "UDP queued for " << oAddr.toString().toAscii().constData() << "seq" << pDG->m_nSequence << "parts" << pDG->m_nCount;


	emit SendQueueUpdated();

}


void CDatagrams::OnPacket(CEndPoint addr, G2Packet* pPacket)
{
	try
	{
		if(pPacket->IsType("PI"))
		{
			OnPing(addr, pPacket);
		}
		else if(pPacket->IsType("PO"))
		{
			OnPong(addr, pPacket);
		}
		else if(pPacket->IsType("CRAWLR"))
		{
			OnCRAWLR(addr, pPacket);
		}
		else if(pPacket->IsType("QKA"))
		{
			OnQKA(addr, pPacket);
		}
		else if(pPacket->IsType("QA"))
		{
			OnQA(addr, pPacket);
		}
		else if(pPacket->IsType("QH2"))
		{
			OnQH2(addr, pPacket);
		}
		else
		{
			//qDebug() << "UDP RECEIVED unknown packet " << pPacket->GetType();
		}
	}
	catch(...)
	{
		systemLog.postLog(tr("malformed packet"), LogSeverity::Debug);
		//qDebug() << "malformed packet";
	}
}

void CDatagrams::OnPing(CEndPoint& addr, G2Packet* pPacket)
{
	Q_UNUSED(pPacket);

	G2Packet* pNew = G2Packet::New("PO", false);
	SendPacket(addr, pNew, false);
	pNew->Release();
}

void CDatagrams::OnPong(CEndPoint& addr, G2Packet* pPacket)
{
	if(pPacket->m_bCompound)
	{
		char szType[9];
		quint32 nLength = 0, nNext = 0;

		while(pPacket->ReadPacket(&szType[0], nLength))
		{
			nNext = pPacket->m_nPosition + nLength;

			if(strcmp("RELAY", szType) == 0)
			{
				if(!Network.IsConnectedTo(addr))
				{
					m_bFirewalled = false;
				}
			}

			pPacket->m_nPosition = nNext;
		}
	}
}
void CDatagrams::OnCRAWLR(CEndPoint& addr, G2Packet* pPacket)
{
	QMutexLocker l(&Network.m_pSection);
	QMutexLocker l2(&Neighbours.m_pSection);

	bool bRLeaf = false;
	bool bRNick = false;
	bool bRGPS = false;
	bool bRExt = false;

	if(!pPacket->m_bCompound)
	{
		return;
	}

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("RLEAF", szType) == 0)
		{
			bRLeaf = true;
		}
		else if(strcmp("RNAME", szType) == 0)
		{
			bRNick = true;
		}
		else if(strcmp("RGPS", szType) == 0)
		{
			bRGPS = true;
		}
		else if(strcmp("REXT", szType) == 0)
		{
			bRExt = true;
		}

		pPacket->m_nPosition = nNext;
	}


	G2Packet* pCA = G2Packet::New("CRAWLA", true);

	G2Packet* pTmp = G2Packet::New("SELF", true);
	if(Network.isHub())
	{
		pTmp->WritePacket("HUB", 0);
	}
	else
	{
		pTmp->WritePacket("LEAF", 0);
	}
	pTmp->WritePacket("NA", ((Network.m_oAddress.protocol() == 0) ? 6 : 18))->WriteHostAddress(&Network.m_oAddress);
	pTmp->WritePacket("CV", quazaaGlobals.UserAgentString().toUtf8().size())->WriteString(quazaaGlobals.UserAgentString(), false);
	pTmp->WritePacket("V", 4)->WriteString(quazaaGlobals.VendorCode(), false);;
	quint16 nLeaves = Neighbours.m_nLeavesConnected;
	pTmp->WritePacket("HS", 2)->WriteIntLE(nLeaves);
	if(!quazaaSettings.Profile.GnutellaScreenName.isEmpty())
	{
		pTmp->WritePacket("NAME", quazaaSettings.Profile.GnutellaScreenName.left(255).toUtf8().size())->WriteString(quazaaSettings.Profile.GnutellaScreenName.left(255));
	}

	pCA->WritePacket(pTmp);
	pTmp->Release();

	for(QList<CG2Node*>::iterator itNode = Neighbours.begin(); itNode != Neighbours.end(); ++itNode)
	{
		CG2Node* pNode = *itNode;
		if(pNode->m_nState == nsConnected)
		{
			if(pNode->m_nType == G2_HUB)
			{
				G2Packet* pNH = G2Packet::New("NH");
				pNH->WritePacket("NA", ((pNode->m_oAddress.protocol() == 0) ? 6 : 18))->WriteHostAddress(&pNode->m_oAddress);
				pNH->WritePacket("HS", 2)->WriteIntLE(pNode->m_nLeafCount);
				pCA->WritePacket(pNH);
				pNH->Release();
			}
			else if(pNode->m_nType == G2_LEAF)
			{
				G2Packet* pNL = G2Packet::New("NL");
				pNL->WritePacket("NA", ((pNode->m_oAddress.protocol() == 0) ? 6 : 18))->WriteHostAddress(&pNode->m_oAddress);
				pCA->WritePacket(pNL);
				pNL->Release();
			}
		}
	}

	SendPacket(addr, pCA, true);

	pCA->Release();
}
void CDatagrams::OnQKA(CEndPoint& addr, G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	quint32 nKey = 0;
	QHostAddress nKeyHost;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("QK", szType) == 0 && nLength >= 4)
		{
			pPacket->ReadIntLE(&nKey);
		}
		else if(strcmp("SNA", szType) == 0 && nLength >= 4)
		{
			CEndPoint ep;

			if( nLength >= 16 )
			{
				Q_IPV6ADDR ip;
				pPacket->Read(&ip, 16);
				nKeyHost.setAddress(ip);
			}
			else
			{
				quint32 nIp;
				pPacket->ReadIntBE(&nIp);
				nKeyHost.setAddress(nIp);
			}
		}
		pPacket->m_nPosition = nNext;
	}

	HostCache.m_pSection.lock();
	CHostCacheHost* pCache = HostCache.Add(addr, 0);
	if(pCache)
	{
		pCache->SetKey(nKey);
	}
	HostCache.m_pSection.unlock();


	systemLog.postLog(tr("Got a query key for %1 = 0x%2").arg(addr.toString().toAscii().constData()).arg(nKey), LogSeverity::Debug);
	//qDebug("Got a query key for %s = 0x%x", addr.toString().toAscii().constData(), nKey);

	if(Network.isHub() && !nKeyHost.isNull() /*&& nKeyHost != Network.m_oAddress.ip*/)
	{
		if( addr.protocol() == 0 )
		{
			uchar* pOut = pPacket->WriteGetPointer(11, 0);
			*pOut++ = 0x50;
			*pOut++ = 6;
			*pOut++ = 'Q';
			*pOut++ = 'N';
			*pOut++ = 'A';

			quint32 nIP = qToBigEndian(addr.toIPv4Address());
			quint16 nPort = qToLittleEndian(addr.port());
			memcpy(pOut, &nIP, 4);
			memcpy(pOut + 4, &nPort, 2);
		}
		else
		{
			// IPv6
			uchar* pOut = pPacket->WriteGetPointer(11, 0);
			*pOut++ = 0x50;
			*pOut++ = 18;
			*pOut++ = 'Q';
			*pOut++ = 'N';
			*pOut++ = 'A';

			Q_IPV6ADDR nIP = addr.toIPv6Address();
			quint16 nPort = qToLittleEndian(addr.port());
			memcpy(pOut, &nIP, 16);
			memcpy(pOut + 16, &nPort, 2);
		}

		QMutexLocker l(&m_pSection);
		bool bNeedSignal = m_lPendingQKA.isEmpty() && m_lPendingQA.isEmpty() && m_lPendingQH2.isEmpty();

		pPacket->AddRef();
		m_lPendingQKA.append(qMakePair<QHostAddress, G2Packet*>(nKeyHost, pPacket));

		while(m_lPendingQKA.size() > 1000)
		{
			m_lPendingQKA.takeFirst().second->Release();
		}

		if(bNeedSignal)
		{
			emit PacketQueuedForRouting();
		}
	}
	// TODO Forwarding jesli hub i SNA != lokalny, dodac QNA

}
void CDatagrams::OnQA(CEndPoint& addr, G2Packet* pPacket)
{
	HostCache.m_pSection.lock();

	CHostCacheHost* pHost = HostCache.Add(addr, 0);
	if(pHost)
	{
		pHost->m_tAck = 0;
	}

	HostCache.m_pSection.unlock();

	QUuid oGuid;

	// Hubs are only supposed to route UDP /QA - we'll drop it if we're in leaf mode
	if(SearchManager.OnQueryAcknowledge(pPacket, addr, oGuid) && Network.isHub())
	{
		// Add from address
		// pPacket->WriteChild("FR")->WriteHostAddress(&addr);

		//QMutexLocker l(&Network.m_pSection);

		//Network.RoutePacket(oGuid, pPacket);

		m_pSection.lock();

		bool bNeedSignal = m_lPendingQKA.isEmpty() && m_lPendingQA.isEmpty() && m_lPendingQH2.isEmpty();

		pPacket->AddRef();
		m_lPendingQA.append(qMakePair(oGuid, pPacket));

		while(m_lPendingQA.size() > 1000)
		{
			m_lPendingQA.takeFirst().second->Release();
		}

		if(bNeedSignal)
		{
			emit PacketQueuedForRouting();
		}

		m_pSection.unlock();
	}

}

void CDatagrams::OnQH2(CEndPoint& addr, G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	QueryHitInfo* pInfo = CQueryHit::ReadInfo(pPacket, &addr);

	if(pInfo)
	{
		if(SearchManager.OnQueryHit(pPacket, pInfo) && Network.isHub() && pInfo->m_nHops < 7)
		{
			m_pSection.lock();

			pPacket->m_pBuffer[pPacket->m_nLength - 17]++;

			QueuedQueryHit pQueue;
			pQueue.m_pInfo = pInfo;
			pQueue.m_pPacket = pPacket;

			bool bNeedSignal = m_lPendingQKA.isEmpty() && m_lPendingQA.isEmpty() && m_lPendingQH2.isEmpty();

			pPacket->AddRef();
			m_lPendingQH2.append(pQueue);

			while(m_lPendingQH2.size() > 10000)
			{
				QueuedQueryHit pHit = m_lPendingQH2.takeFirst();
				pHit.m_pPacket->Release();
				delete pHit.m_pInfo;
			}

			if(bNeedSignal)
			{
				emit PacketQueuedForRouting();
			}

			m_pSection.unlock();
		}
	}
}

