/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "g2node.h"
#include "network.h"
#include "neighbours.h"
#include "g2packet.h"
#include "parser.h"
#include "datagrams.h"
#include "searchmanager.h"
#include "Hashes/hash.h"
#include "query.h"
#include "queryhit.h"
#include "queryhashtable.h"
#include "queryhashmaster.h"
#include "hubhorizon.h"
#include "securitymanager.h"

#include "g2hostcache.h"

#include "quazaasettings.h"
#include "quazaaglobals.h"

#include "debug_new.h"

//#define _DISABLE_COMPRESSION

CG2Node::CG2Node(QObject* parent) :
	CNeighbour( parent ),
	m_bG2Core( false ),
	m_bCachedKeys( false ),
	m_nType( G2_UNKNOWN ),
	m_nLeafCount( 0 ),
	m_nLeafMax( 0 ),
	m_bAcceptDeflate( false ),
	m_tKeyRequest( 0 ),
	m_nHAWWait( 0 ),
	m_pRemoteTable( NULL ),
	m_pLocalTable( NULL ),
	m_pHubGroup( new CHubHorizonGroup ),
	m_bHandshaking( false )
{
	m_nProtocol = DiscoveryProtocol::G2;
}

CG2Node::~CG2Node()
{
	networkG2.m_oRoutingTable.remove(this);

	while(m_lSendQueue.size())
	{
		m_lSendQueue.dequeue()->release();
	}

	if(m_pLocalTable)
	{
		delete m_pLocalTable;
	}

	if(m_pRemoteTable)
	{
		delete m_pRemoteTable;
	}

	delete m_pHubGroup;
}

void CG2Node::sendPacket(G2Packet* pPacket, bool bBuffered, bool bRelease)
{
	ASSUME_LOCK(Neighbours.m_pSection);

	m_nPacketsOut++;

	if(bBuffered)
	{
		while(m_lSendQueue.size() > 128)
		{
			m_lSendQueue.dequeue()->release();
		}

		pPacket->addRef();
		m_lSendQueue.enqueue(pPacket);
	}
	else
	{
		pPacket->toBuffer(getOutputBuffer());
	}

	if(bRelease)
	{
		pPacket->release();
	}

	emit readyToTransfer();
}

void CG2Node::sendLNI()
{
	G2Packet* pLNI = G2Packet::newPacket("LNI", true);
	if(networkG2.m_oAddress.protocol() == 0)
	{
		pLNI->writePacket("NA", 6)->writeHostAddress(networkG2.m_oAddress);
	}
	else
	{
		pLNI->writePacket("NA", 18)->writeHostAddress(networkG2.m_oAddress);
	}
	pLNI->writePacket("GU", 16)->writeGUID(quazaaSettings.Profile.GUID);
	pLNI->writePacket("V", 4)->writeString(CQuazaaGlobals::VENDOR_CODE(), false);

	if(Neighbours.isG2Hub())
	{
		quint16 nLeavesMax = quazaaSettings.Gnutella2.NumLeafs;
		quint16 nLeaves = Neighbours.m_nLeavesConnectedG2;
		pLNI->writePacket("HS", 4);
		pLNI->writeIntLE<quint16>(nLeaves);
		pLNI->writeIntLE<quint16>(nLeavesMax);
		pLNI->writePacket("QK", 0);
	}

	pLNI->writePacket("g2core", 0);

	sendPacket(pLNI, false, true);
}

void CG2Node::sendHAW()
{
	G2Packet* pPacket = G2Packet::newPacket("HAW");

	pPacket->writePacket("NA", networkG2.m_oAddress.protocol() == QAbstractSocket::IPv4Protocol ? 6 : 18);
	pPacket->writeHostAddress(networkG2.m_oAddress);

	pPacket->writePacket("V", 4);
	pPacket->writeString(CQuazaaGlobals::VENDOR_CODE());

	pPacket->writePacket("HS", 2);
	pPacket->writeIntLE<quint16>(Neighbours.m_nLeavesConnectedG2);

	pPacket->writeByte(0); // end of child packets
	pPacket->writeByte(100);
	pPacket->writeByte(0);
	QUuid oGUID = QUuid::createUuid();
	pPacket->writeGUID(oGUID);

	sendPacket(pPacket, false, true);
}

void CG2Node::onTimer(quint32 tNow)
{

	CNeighbour::onTimer(tNow);

	if(m_nState == nsConnected)
	{
		if(m_nPingsWaiting == 0 && (tNow - m_tLastPacketIn >= 30 || tNow - m_tLastPingOut >= quazaaSettings.Gnutella2.PingRate))
		{
			// If we got the last packet at least 30 seconds ago
			// or our last ping at least 2 minutes ago
			// and do not expect a response to an earlier ping
			// then we send keep-alive ping, on the occasion of the RTT measurement
			G2Packet* pPacket = G2Packet::newPacket("PI", false);
			sendPacket(pPacket, false, true); // Unbuffered, we can accurately measure the RTT
			m_nPingsWaiting++;
			m_tLastPingOut = tNow;
			m_tRTTTimer.start();
		}

		/*if( m_tKeyRequest > 0 && tNow - m_tKeyRequest > 90 )
		{
			qDebug() << "Closing connection with " << m_oAddress.toString().toLocal8Bit() << "QueryKey wait timeout reached";
			m_nState = nsClosing;
			emit NodeStateChanged();
			Close();
			return;
		}*/

		if ( ( m_nType == G2_HUB && tNow - m_tConnected > 30 ) &&
				(( m_pLocalTable != 0 && m_pLocalTable->m_nCookie != QueryHashMaster.m_nCookie &&
				   tNow - m_pLocalTable->m_nCookie > 60 ) ||
				 ( QueryHashMaster.m_nCookie - m_pLocalTable->m_nCookie > 60 ||
				   !m_pLocalTable->m_bLive ))
		   )
		{
			if(m_pLocalTable->patchTo(&QueryHashMaster, this))
			{
				systemLog.postLog(LogSeverity::Notice, Components::G2, tr("Sending query routing table to %1 (%2 bits, %3 entries, %4 bytes, %5% full)").arg(m_oAddress.toString().toLocal8Bit().constData()).arg(m_pLocalTable->m_nBits).arg(m_pLocalTable->m_nHash).arg(m_pLocalTable->m_nHash / 8).arg(m_pLocalTable->getPercent()));
			}
		}

		if(m_nType == G2_HUB && Neighbours.isG2Hub())
		{
			if(m_nHAWWait == 0)
			{
				sendHAW();
				m_nHAWWait = quazaaSettings.Gnutella2.HAWPeriod;
			}
			else
			{
				--m_nHAWWait;
			}
		}

		// If hub has been connected for more than 5 minutes and it still has less than 2 leaves,
		// something is very wrong. Ban it for a short while.
		if ( m_nType == G2_HUB && (tNow - m_tConnected > 300) && m_nLeafCount < 2 )
		{
			close();
			securityManager.ban( m_oAddress, Security::RuleTime::ThirtyMinutes, true,
								 QString( "Hub not accepting leaves (%1)"
										  ).arg( m_oAddress.toString() ) );
			return;
		}

		// Anti-DDoS
		// cleaning table
		if(m_lRABan.size() >= 1000)
		{
			systemLog.postLog(LogSeverity::Debug, Components::G2, QString("Clearing bans on hub %1").arg(m_oAddress.toString()));
			//qDebug() << "Clearing bans on hub " << m_oAddress.toString();
			for(QHash<quint32, quint32>::iterator itBan = m_lRABan.begin(); itBan != m_lRABan.end();)
			{
				if(tNow - itBan.value() > 60)   // 1 minute
				{
					itBan = m_lRABan.erase(itBan);
				}
				else
				{
					++itBan;
				}
			}
			systemLog.postLog(LogSeverity::Debug, Components::G2, QString("Still active bans: %1").arg(m_lRABan.size()));
			//qDebug() << "Still active bans: " << m_lRABan.size();
		}

	}
}

void CG2Node::initiateHandshake()
{
	m_nState = nsHandshaking;
	emit nodeStateChanged();

	QByteArray sHandshake;

	sHandshake += "GNUTELLA CONNECT/0.6\r\n";
	sHandshake += "Listen-IP: " + networkG2.getLocalAddress().toStringWithPort() + "\r\n";
	sHandshake += "Remote-IP: " + m_oAddress.toString() + "\r\n";
	sHandshake += "User-Agent: " + CQuazaaGlobals::USER_AGENT_STRING() + "\r\n";
	sHandshake += "Accept: application/x-gnutella2\r\n";

	// TODO: Remove X-Ultrapeer at a later time.
	if ( Neighbours.isG2Hub() )
	{
		sHandshake += "X-Hub: True\r\n";
		sHandshake += "X-Ultrapeer: True\r\n";
	}
	else
	{
		sHandshake += "X-Hub: False\r\n";
		sHandshake += "X-Ultrapeer: False\r\n";
	}

#ifndef _DISABLE_COMPRESSION
	sHandshake += "Accept-Encoding: deflate\r\n";
#endif

	sHandshake += "\r\n";

	//qDebug() << "Handshake send:\n" << sHs;

	m_sHandshake += "Handshake out:\n" + sHandshake;

	write( sHandshake );
}

void CG2Node::parseIncomingHandshake()
{
	//QMutexLocker l(&Neighbours.m_pSection);

	const qint32 nIndex = peek( bytesAvailable() ).indexOf( "\r\n\r\n" );
	const QString sHs   = read( nIndex + 4 );

	//qDebug() << "Handshake receive:\n" << sHs;

	m_sHandshake += "Handshake in:\n" + sHs;

	if ( !sHs.startsWith( "GNUTELLA CONNECT/0.6" ) )
	{
		systemLog.postLog( LogSeverity::Debug, Components::G2,
						   QString( "Connection to %1 rejected: %2"
									).arg( this->m_oAddress.toString(),
										   sHs.left( sHs.indexOf( "\r\n" ) ) ) );

		//qDebug() << "Connection rejected: " << sHs.left(sHs.indexOf("\r\n"));

		m_nState = nsClosing;
		emit nodeStateChanged();
		close();
	}

	// read user agent and check it against security manager
	if ( readUserAgentSecurity( sHs ) )
	{
		return;
	}

	{   // read and verify accept type
		const QString sAccept = Parser::getHeaderValue( sHs, "Accept" );
		const bool bAcceptG2 = sAccept.contains( "application/x-gnutella2" );

		if ( !bAcceptG2 )
		{
			// We don't send X-Try to nodes not trying to connect to G2.
			send_ConnectError( "503 Required network not accepted", false );
			return;
		}
	}

#ifndef _DISABLE_COMPRESSION
	{   // determine whether the other party accepts deflated data
		const QString sAcceptEnc = Parser::getHeaderValue(sHs, "Accept-Encoding");
		m_bAcceptDeflate = !sAcceptEnc.compare( "deflate" ) && Neighbours.isG2Hub();
	}
#endif

	{   // read and use remote IP information
		const QString sRemoteIP = Parser::getHeaderValue( sHs, "Remote-IP" );
		if ( !networkG2.acquireLocalAddress( sRemoteIP ) )
		{
			// We don't send X-Try to buggy clients.
			send_ConnectError( "503 Remote-IP header missing", false );
			return;
		}
	}

	{   // read and use remote Hub state information
		QString sXHub = Parser::getHeaderValue( sHs, "X-Hub" ).toLower();
		if ( sXHub.isEmpty() )
		{
			// fall back to X-Ultrapeer mode if no X-Hub could be found
			sXHub = Parser::getHeaderValue( sHs, "X-Ultrapeer" ).toLower();
			if ( sXHub.isEmpty() )
			{
				// We don't send X-Try to buggy clients.
				send_ConnectError( "503 No hub mode specified", false );
				return;
			}
		}

		bool bHub = ( sXHub == "true" );

		if ( bHub )
		{
			if ( !Neighbours.needMoreG2( G2_HUB ) )
			{
				send_ConnectError( "503 Maximum hub connections reached", true );
				return;
			}
			m_nType = G2_HUB;
		}
		else
		{
			if ( !Neighbours.needMoreG2( G2_LEAF ) )
			{
				send_ConnectError( "503 Maximum leaf connections reached", true );
				return;
			}
			m_nType = G2_LEAF;
		}
	}

	// remember to listen for a reply
	m_bHandshaking = true;

	// send handshake message #2 to remote host
	send_ConnectOK( true, m_bAcceptDeflate );
}

// this is handshake message #2
void CG2Node::parseHandshakeResponse()
{
	//QMutexLocker l(&Neighbours.m_pSection);
	QString sHandshake = read( peek( bytesAvailable() ).indexOf( "\r\n\r\n" ) + 4 );

	//qDebug() << "Handshake receive:\n" << sHs;

	m_sHandshake += "Handshake in:\n" + sHandshake;

	const QString sAccept = Parser::getHeaderValue( sHandshake, "Accept" );
	bool bAcceptG2 = sAccept.contains( "application/x-gnutella2" );

	if ( !bAcceptG2 )
	{
		// We don't send X-Try to nodes not trying to connect to G2.
		send_ConnectError( "503 Required network not accepted", false );
		return;
	}

	if ( readUserAgentSecurity( sHandshake ) )
	{
		return;
	}

	const QString sContentType = Parser::getHeaderValue( sHandshake, "Content-Type" );
	bool bG2Provided = sContentType.contains( "application/x-gnutella2" );

	if ( !bG2Provided )
	{
		// We don't send X-Try to nodes not trying to connect to G2.
		send_ConnectError( "503 Required network not provided", false );
		return;
	}

	const QString sXTry = Parser::getHeaderValue( sHandshake, "X-Try-Hubs" );
	if ( bAcceptG2 && bG2Provided && sXTry.size() )
	{
		hostCache.addXTry( sXTry );
	}

	if ( sHandshake.left( 16 ) != "GNUTELLA/0.6 200" )
	{
		systemLog.postLog( LogSeverity::Error, Components::G2,
						   QString( "Connection to %1 rejected: %2"
									).arg( m_oAddress.toString(),
										   sHandshake.left( sHandshake.indexOf( "\r\n" ) ) ) );

		// TODO: Is it okay to count non-200 response as a failure? Needs some testing...
		hostCache.onFailure( m_oAddress );

		close();
		return;
	}

	{   // read and use remote IP information
		const QString sRemoteIP = Parser::getHeaderValue( sHandshake, "Remote-IP" );
		if ( !networkG2.acquireLocalAddress( sRemoteIP ) )
		{
			// We don't send X-Try to buggy clients.
			send_ConnectError( "503 Remote-IP header missing", false );
			return;
		}
	}

	{   // read and use Hub state information
		QString sHub = Parser::getHeaderValue(sHandshake, "X-Hub").toLower();
		if ( sHub.isEmpty() )
		{
			// use X-Ultrapeer as a fallback only to be able to understand older clients
			sHub = Parser::getHeaderValue(sHandshake, "X-Ultrapeer").toLower();
		}

		//QString sHubNeeded = Parser::getHeaderValue(sHs, "X-Hub-Needed").toLower();

		if ( sHub == "true" )
		{
			if ( !Neighbours.needMoreG2( G2_HUB ) )
			{
				send_ConnectError( "503 Maximum hub connections reached", true );
				return;
			}
			m_nType = G2_HUB;
		}
		else
		{
			if ( !Neighbours.needMoreG2( G2_LEAF ) )
			{
				send_ConnectError( "503 Maximum leaf connections reached", true );
				return;
			}
			m_nType = G2_LEAF;
		}
	}

	bool bAcceptDeflate = false;
#ifndef _DISABLE_COMPRESSION
	const QString sContentEnc = Parser::getHeaderValue( sHandshake, "Content-Encoding" );
	if ( sContentEnc.contains( "deflate" ) )
	{
		if ( !enableInputCompression() )
		{
			systemLog.postLog( LogSeverity::Debug, Components::G2, "Inflate init error!" );
			close();
			return;
		}
	}

	const QString sAcceptEnc = Parser::getHeaderValue( sHandshake, "Accept-Encoding" );
	bAcceptDeflate = sAcceptEnc.contains( "deflate" ) && Neighbours.isG2Hub();

	if ( bAcceptDeflate )
	{
		if ( !enableOutputCompression() )
		{
			systemLog.postLog( LogSeverity::Debug, Components::G2, "Deflate init error!" );
			close();
			return;
		}
	}
#endif

	// send handshake message #3 back
	send_ConnectOK( false, bAcceptDeflate );

	hostCache.updateFailures( m_oAddress, 0 );

	// connection established successfully
	m_nState = nsConnected;
	emit nodeStateChanged();

	sendStartups();
	m_tLastPacketIn = m_tLastPacketOut = common::getTNowUTC();

	if ( m_nType == G2_HUB )
	{
		Q_ASSERT( !m_pLocalTable );
		m_pLocalTable = new CQueryHashTable();
	}

	systemLog.postLog( LogSeverity::Information, Components::G2,
					   tr( "Gnutella2 connection with %1 established."
						   ).arg( m_oAddress.toString() ) );
}

// parses handshake message #3 recieved from remote node
void CG2Node::parseHandShakeAccept()
{
	m_bHandshaking = false;

	QString sHandshake = read( peek( bytesAvailable() ).indexOf( "\r\n\r\n" ) + 4 );

	const QString sContentType = Parser::getHeaderValue( sHandshake, "Content-Type" );
	bool bG2Provided = sContentType.contains("application/x-gnutella2");

	if ( !bG2Provided )
	{
		// We don't send X-Try to nodes not trying to connect to G2.
		send_ConnectError( "503 Required network not provided", false );
		return;
	}

#ifndef _DISABLE_COMPRESSION
	const QString sContentEnc = Parser::getHeaderValue( sHandshake, "Content-Encoding" );
	if ( sContentEnc.contains( "deflate" ) )
	{
		if ( !enableInputCompression() )
		{
			systemLog.postLog( LogSeverity::Debug, Components::G2,
							   QString( "Inflate init error!" ) );
			close();
			return;
		}
	}

	if ( m_bAcceptDeflate )
	{
		if ( !enableOutputCompression() )
		{
			systemLog.postLog( LogSeverity::Debug, Components::G2,
							   QString( "Deflate init error!" ) );
			close();
			return;
		}
	}
#endif

	// connection established successfully
	m_nState = nsConnected;
	emit nodeStateChanged();

	sendStartups();
	m_tLastPacketIn = m_tLastPacketOut = common::getTNowUTC();
	if ( m_nType == G2_HUB )
	{
		Q_ASSERT( !m_pLocalTable );
		m_pLocalTable = new CQueryHashTable();
	}

}

bool CG2Node::readUserAgentSecurity(const QString& sHandShake)
{
	if ( m_sUserAgent.isEmpty() )
	{
		m_sUserAgent = Parser::getHeaderValue( sHandShake, "User-Agent" );
	}

	if ( m_sUserAgent.isEmpty() )
	{
		send_ConnectError( "503 Anonymous clients are not allowed here", true );
		return true;
	}

	if ( securityManager.isAgentDenied( m_sUserAgent ) )
	{
		// We don't like you, so you won't get an X-Try from us!
		send_ConnectError( "403 Access Denied, sorry", false );
		securityManager.ban( m_oAddress, Security::RuleTime::SixHours, true,
							 QString( "[AUTO] UA Blocked (%1)" ).arg( m_sUserAgent ), true
#if SECURITY_LOG_BAN_SOURCES
							 , QString( "g2node.cpp line 495" )
#endif // SECURITY_LOG_BAN_SOURCES
							 );
		return true;
	}

	if ( securityManager.isClientBad( m_sUserAgent ) )
	{
		send_ConnectError( "403 Your client is too old or otherwise bad. Please upgrade.", true );
		return true;
	}
	else
	{
		return false;
	}
}

void CG2Node::send_ConnectError(QString sReason, bool bXTry)
{
	systemLog.postLog( LogSeverity::Information, Components::G2,
					   tr( "Rejecting connection with %1: %2 (%3)"
						   ).arg( m_oAddress.toString(), sReason, m_sUserAgent ) );

	QByteArray sHandshake;

	sHandshake += "GNUTELLA/0.6 " + sReason + "\r\n";
	sHandshake += "User-Agent: " + CQuazaaGlobals::USER_AGENT_STRING() + "\r\n";
	sHandshake += "Accept: application/x-gnutella2\r\n";
	sHandshake += "Content-Type: application/x-gnutella2\r\n";

	if ( bXTry )
	{
		sHandshake += hostCache.getXTry();
	}

	sHandshake += "\r\n\r\n";

	//qDebug() << "Handshake send:\n" << sHs;

	m_sHandshake += "Handshake out:\n" + sHandshake;

	write( sHandshake );

	close( true );
}

void CG2Node::send_ConnectOK(bool bHandshakeStep2, bool bDeflate)
{
	QByteArray sHandshake;

	sHandshake += "GNUTELLA/0.6 200 OK\r\n";
	sHandshake += "User-Agent: " + CQuazaaGlobals::USER_AGENT_STRING() + "\r\n";
	sHandshake += "Content-Type: application/x-gnutella2\r\n";

	// TODO: Remove X-Ultrapeer at a later time.
	if ( Neighbours.isG2Hub() )
	{
		sHandshake += "X-Hub: True\r\n";
		sHandshake += "X-Ultrapeer: True\r\n";
	}
	else
	{
		sHandshake += "X-Hub: False\r\n";
		sHandshake += "X-Ultrapeer: False\r\n";
	}

	if ( bHandshakeStep2 )
	{
		// only for handshake step #2
		sHandshake += "Listen-IP: " + networkG2.getLocalAddress().toStringWithPort() + "\r\n";
		sHandshake += "Remote-IP: " + m_oAddress.toString() + "\r\n";
		sHandshake += "Accept: application/x-gnutella2\r\n";
		sHandshake += "X-Hub-Needed: False\r\n";

		// We only send an X-Try if the other party initiated the connection, as the other party
		// might still be in need for more hubs to connect to. (There is no need to do so if we
		// initiated the connection, as the other party should be well connected in that case.)
		sHandshake += hostCache.getXTry();

#ifndef _DISABLE_COMPRESSION
		if ( Neighbours.isG2Hub() && m_nType == G2_HUB )
		{
			sHandshake += "Accept-Encoding: deflate\r\n";
		}
#endif
	}

#ifndef _DISABLE_COMPRESSION
		if ( bDeflate )
		{
			sHandshake += "Content-Encoding: deflate\r\n";
		}
#endif

	sHandshake += "\r\n";

	//qDebug() << "Handshake send:\n" << sHs;

	m_sHandshake += "Handshake out:\n" + sHandshake;

	write( sHandshake );
}

void CG2Node::sendStartups()
{
	if ( networkG2.isListening() )
	{
		CEndPoint addr = networkG2.getLocalAddress();
		G2Packet* pPacket = G2Packet::newPacket( "PI", true );
		pPacket->writePacket( "UDP", 6 );
		pPacket->writeHostAddress( addr );
		pPacket->writePacket( "TFW", 0 );
		sendPacket( pPacket, false, true );
	}

	sendLNI();
}

void CG2Node::onPacket(G2Packet* pPacket)
{
	//qDebug() << "Got packet " << pPacket->GetType() << pPacket->ToHex() << pPacket->ToASCII();

	//try
	//{
	if ( !networkG2.routePacket( pPacket ) )
	{

		if ( pPacket->isType( "PI" ) )
		{
			onPing( pPacket );
		}
		else if ( pPacket->isType( "PO" ) )
		{
			onPong( pPacket );
		}
		else if ( pPacket->isType( "LNI" ) )
		{
			onLNI( pPacket );
		}
		else if ( pPacket->isType( "KHL" ) )
		{
			onKHL( pPacket );
		}
		else if ( pPacket->isType( "QHT" ) )
		{
			onQHT( pPacket );
		}
		else if ( pPacket->isType( "Q2" ) )
		{
			onQuery( pPacket );
		}
		else if ( pPacket->isType( "QKR" ) )
		{
			onQKR(pPacket );
		}
		else if ( pPacket->isType( "QKA" ) )
		{
			onQKA( pPacket );
		}
		else if ( pPacket->isType( "QA" ) )
		{
			onQA( pPacket );
		}
		else if ( pPacket->isType( "QH2" ) )
		{
			onQH2( pPacket );
		}
		else if ( pPacket->isType( "HAW" ) )
		{
			onHaw( pPacket );
		}
		else
		{
			systemLog.postLog( LogSeverity::Debug, Components::G2,
							   QString( "G2 TCP recieved unknown packet %1"
										).arg( pPacket->getType() ) );
		}
	}
	/*}
	catch(...)
	{
		qDebug() << "Packet error in child packets";
	}*/
}

void CG2Node::onPing(G2Packet* pPacket)
{
	ASSUME_LOCK(Neighbours.m_pSection);

	bool bUdp = false;
	bool bRelay = false;
	//	bool bTestFirewall = false;
	CEndPoint addr;

	if(pPacket->m_bCompound)
	{
		char szType[9];
		quint32 nLength = 0, nNext = 0;

		while(pPacket->readPacket(&szType[0], nLength))
		{
			nNext = pPacket->m_nPosition + nLength;

			if(strcmp("UDP", szType) == 0 && nLength >= 6)
			{
				pPacket->readHostAddress(&addr);
				if(addr.isValid())
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
				//				bTestFirewall = true;
			}

			pPacket->m_nPosition = nNext;
		}

	}

	if(!bUdp && !bRelay)
	{
		// direct ping
		G2Packet* pPong = G2Packet::newPacket("PO", false);
		sendPacket(pPong, false, true);
		return;
	}

	if(bUdp && !bRelay)
	{
		// /PI/UDP

		if(Neighbours.isG2Hub()) // If we are a hub.
		{
			G2Packet* pRelay = G2Packet::newPacket("RELAY");
			pPacket->prependPacket(pRelay);

			int nRelayed = 0, nCount = Neighbours.getCount();
			QList<int> lToRelayIndex;

			for(int i = 0; i < nCount && nRelayed < quazaaSettings.Gnutella2.PingRelayLimit; ++i)
			{
				int nIndex = qrand() % nCount;
				if(!lToRelayIndex.contains(nIndex))
				{
					CNeighbour* pNode = Neighbours.getAt(nIndex);
					if(pNode != this
							&& pNode->m_nProtocol == DiscoveryProtocol::G2
							&& pNode->m_nState == nsConnected
							&& static_cast<CG2Node*>(pNode)->m_nType == G2_LEAF )
					{
						pPacket->addRef();
						((CG2Node*)pNode)->sendPacket(pPacket, true, true);
						nRelayed++;
					}
				}
			}
			return;
		}
	}

	if(bUdp && bRelay)
	{
		G2Packet* pPong = G2Packet::newPacket("PO", true);
		pPong->writePacket("RELAY", 0);
		Datagrams.sendPacket(addr, pPong, true);
		pPong->release();
	}
}

void CG2Node::onPong(G2Packet* pPacket)
{
	Q_UNUSED(pPacket);

	if(m_nPingsWaiting > 0)
	{
		--m_nPingsWaiting;

		if(m_nPingsWaiting == 0)
		{
			m_tRTT = m_tRTTTimer.elapsed();
		}
	}
}

void CG2Node::onLNI(G2Packet* pPacket)
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

	while(pPacket->readPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("NA", szType) == 0 && nLength >= 6)
		{
			if(nLength >= 18)
			{
				pPacket->readHostAddress(&hostAddr, false);
			}
			else
			{
				pPacket->readHostAddress(&hostAddr);
			}

			if(hostAddr.isValid())
			{
				if(securityManager.isDenied(hostAddr)) {
					close();
				} else {
				hasNA = true;

				if(!m_bInitiated)
				{
					m_oAddress = hostAddr;
				}
			}
		}
		}
		else if(strcmp("GU", szType) == 0 && nLength >= 16)
		{
			hasGUID = true;
			pGUID = pPacket->readGUID();
		}
		else if(strcmp("HS", szType) == 0 && nLength >= 4)
		{
			if(m_nType == G2_HUB)
			{
				m_nLeafCount = pPacket->readIntLE<quint16>();
				m_nLeafMax = pPacket->readIntLE<quint16>();
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
		QMutexLocker l(&networkG2.m_pSection);
		networkG2.m_oRoutingTable.add(pGUID, this, true);
	}

}

void CG2Node::onKHL(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	const quint32 tNow = common::getTNowUTC();
	qint32 nDiff = 0;

	m_pHubGroup->clear();

	char szType[9], szInner[9];
	quint32 nLength = 0, nInnerLength = 0;
	bool bCompound = false;
	quint32 nNext = 0, nInnerNext = 0;

	while ( pPacket->readPacket( &szType[0], nLength, &bCompound ) )
	{
		nNext = pPacket->m_nPosition + nLength;

		// NH - Neighbouring Hub
		if(strcmp("NH", szType) == 0)
		{
			// Child packet types:
			// /KHL/NH/GU - GUID
			// /KHL/NH/V  - Vendor Code
			// /KHL/NH/LS - Library Statistics
			// /KHL/NH/HS - Hub Status

			QUuid pGUID;

			if(bCompound)
			{
				while(pPacket->m_nPosition < nNext && pPacket->readPacket(&szInner[0], nInnerLength))
				{
					nInnerNext = pPacket->m_nPosition + nInnerLength;

					if(strcmp("GU", szInner) == 0 && nInnerLength >= 16)
					{
						pGUID = pPacket->readGUID();
					}

					pPacket->m_nPosition = nInnerNext;
				}

				nLength = nNext - pPacket->m_nPosition;
			}

			if(nLength >= 6)
			{
				CEndPoint pAddr;

				if(nLength >= 18)
				{
					pPacket->readHostAddress(&pAddr, false);
				}
				else
				{
					pPacket->readHostAddress(&pAddr);
				}

				if(m_nType == G2_HUB)
				{
					m_pHubGroup->add(pAddr);
				}

				if(!pGUID.isNull())
				{
					QMutexLocker l(&networkG2.m_pSection);

					networkG2.m_oRoutingTable.add(pGUID, this, &pAddr, false);
				}
			}
		}
		// CH - Cached Hub
		else if(strcmp("CH", szType) == 0)
		{
			// Child packet types:
			// /KHL/CH/GU - GUID
			// /KHL/CH/V  - Vendor Code
			// /KHL/CH/LS - Library Statistics
			// /KHL/CH/HS - Hub Status

			QString sVendor;

			if( bCompound )
			{
				while(pPacket->m_nPosition < nNext && pPacket->readPacket(&szInner[0], nInnerLength))
				{
					nInnerNext = pPacket->m_nPosition + nInnerLength;

					if(strcmp("V", szInner) == 0 && nInnerLength >= 4)
					{
						sVendor = pPacket->readString(4);
					}

					pPacket->m_nPosition = nInnerNext;
				}

				nLength = nNext - pPacket->m_nPosition;
			}

			if(nLength >= 10)
			{
				CEndPoint ep;
				// quint32 nTs = 0;

				if(nLength >= 22)
				{
					pPacket->readHostAddress(&ep, false);
				}
				else
				{
					pPacket->readHostAddress(&ep);
				}

				/*nTs = */pPacket->readIntLE<quint32>();

				if ( !sVendor.isEmpty() && securityManager.isVendorBlocked( sVendor ) )
				{
					// TODO: Investigate: This is the cause of many bans of already banned IPs...
					securityManager.ban( ep, Security::RuleTime::SixHours, true,
										 QString( "[AUTO] Vendor blocked (%1)" ).arg( sVendor ),
										 true
#if SECURITY_LOG_BAN_SOURCES
										 , QString( "g2node.cpp line 1131" )
#endif // SECURITY_LOG_BAN_SOURCES
										 );
				}
				else
				{
					hostCache.add( ep, nDiff + tNow );
				}
			}
		}
		// TS - Timestamp
		else if ( strcmp( "TS", szType ) == 0 )
		{
			// Note: No child packets

			if(bCompound)
			{
				pPacket->skipCompound(nLength);
			}

			if(nLength >= 4)
			{
				nDiff = tNow - pPacket->readIntLE<quint32>();
			}
		}
		pPacket->m_nPosition = nNext;
	}
}

void CG2Node::onQHT(G2Packet* pPacket)
{
	if(m_pRemoteTable == 0)
	{
		if(!Neighbours.isG2Hub())
		{
			systemLog.postLog(LogSeverity::Debug, Components::G2, QString("Recieved unexpected Query Routing Table, ignoring"));
			//qDebug() << "Received unexpected Query Routing Table, ignoring";
			return;
		}

		m_pRemoteTable = new CQueryHashTable();
	}

	bool bLive = m_pRemoteTable->m_bLive;

	if(!m_pRemoteTable->onPacket(pPacket))
	{
		systemLog.postLog(LogSeverity::Error, Components::G2, tr("Neighbour %1 sent bad query hash table update. Closing connection.").arg(m_oAddress.toString().toLocal8Bit().constData()));
		close();
		return;
	}

	if(m_pRemoteTable->m_bLive && !bLive)
	{
		systemLog.postLog(LogSeverity::Notice, Components::G2, tr("Neighbour %1 updated its query hash table. %2 bits %3% full.").arg(m_oAddress.toString().toUtf8().constData()).arg(m_pRemoteTable->m_nBits).arg(m_pRemoteTable->getPercent()));
	}

	if(m_nType == G2_LEAF && m_pRemoteTable && m_pRemoteTable->getPercent() > 90)
	{
		systemLog.postLog(LogSeverity::Error, Components::G2, tr("Dropping neighbour %1 - hash table fill percentage too high.").arg(m_oAddress.toString().toLocal8Bit().constData()));
		close();
		return;
	}

	if(m_nType == G2_LEAF && m_pRemoteTable->m_pGroup == 0)
	{
		QueryHashMaster.add(m_pRemoteTable);
	}
}

void CG2Node::onQKR(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound || m_nType != G2_LEAF)
	{
		return;
	}

	char szType[9];
	quint32 nLength = 0, nNext = 0;
	bool bCacheOK = true;
	CEndPoint addr;

	while(pPacket->readPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("QNA", szType) == 0 && nLength >= 6)
		{
			if(nLength >= 18)
			{
				pPacket->readHostAddress(&addr, false);
			}
			else
			{
				pPacket->readHostAddress(&addr);
			}
		}
		else if(strcmp("REF", szType) == 0)
		{
			bCacheOK = false;
		}

		pPacket->m_nPosition = nNext;
	}

	if ( addr.isFirewalled() )
	{
		return;
	}

	const quint32 tNow = common::getTNowUTC();

//#if ENABLE_G2_HOST_CACHE_BENCHMARKING
//	QElapsedTimer tHostCache;
//	tHostCache.start();
//#endif

	QMutexLocker l( &hostCache.m_pSection );

//#if ENABLE_G2_HOST_CACHE_BENCHMARKING
//	qint64 tHCLock = tHostCache.elapsed();
//	QElapsedTimer tHostCacheWork;
//	tHostCacheWork.start();
//#endif

	SharedG2HostPtr pHost = bCacheOK ? hostCache.get( addr ) : SharedG2HostPtr();

//#if ENABLE_G2_HOST_CACHE_BENCHMARKING
//	qint64 tHCWork = tHostCacheWork.elapsed();
//#endif

	if ( pHost && pHost->queryKey() && pHost->keyHost() == networkG2.m_oAddress &&
		 tNow - pHost->keyTime() < quazaaSettings.Gnutella2.QueryKeyTime )
	{
		G2Packet* pQKA = G2Packet::newPacket( "QKA", true );
		if ( addr.protocol() == 0 )
		{
			pQKA->writePacket( "QNA", 6 )->writeHostAddress( addr );
		}
		else
		{
			pQKA->writePacket( "QNA", 18 )->writeHostAddress( addr );
		}
		pQKA->writePacket( "QK", 4 )->writeIntLE<quint32>( pHost->queryKey() );
		pQKA->writePacket( "CACHED", 0 );
		sendPacket( pQKA, true, true );
	}
	else
	{
		G2Packet* pQKR = G2Packet::newPacket( "QKR", true );
		if ( addr.protocol() == 0 )
		{
			pQKR->writePacket( "SNA", 6 )->writeHostAddress( m_oAddress );
		}
		else
		{
			pQKR->writePacket( "SNA", 18 )->writeHostAddress( m_oAddress );
		}

		if ( networkG2.m_oAddress.protocol() == 0 )
		{
			pQKR->writePacket( "RNA", 6 )->writeHostAddress( networkG2.m_oAddress );
		}
		else
		{
			pQKR->writePacket( "RNA", 18 )->writeHostAddress( networkG2.m_oAddress );
		}

		Datagrams.sendPacket( addr, pQKR, false );
		pQKR->release();
	}

//#if ENABLE_G2_HOST_CACHE_BENCHMARKING
//	l.unlock();
//	hostCache.m_nLockWaitTime.fetchAndAddRelaxed( tHCLock );
//	hostCache.m_nWorkTime.fetchAndAddRelaxed( tHCWork );
//#endif
}

void CG2Node::onQKA(G2Packet* pPacket)
{
	if ( !pPacket->m_bCompound )
	{
		return;
	}

	m_tKeyRequest = 0;

	quint32 nKey = 0;
	CEndPoint addr;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while ( pPacket->readPacket( &szType[0], nLength ) )
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("QK", szType) == 0 && nLength >= 4)
		{
			nKey = pPacket->readIntLE<quint32>();
		}
		else if(strcmp("QNA", szType) == 0)
		{
			if(nLength >= 6)
			{
				if(nLength >= 18)
				{
					// IPv6 with port
					pPacket->readHostAddress(&addr, false);
				}
				else if(nLength >= 16)
				{
					// IPv6 without port
					Q_IPV6ADDR ip6;
					pPacket->read(&ip6, 16);
					addr.setAddress(ip6);
				}
				else
				{
					// IPv4 with port
					pPacket->readHostAddress(&addr);
				}
			}
			else if(nLength >= 4)
			{
				// IPv4 without port
				quint32 ip4 = pPacket->readIntBE<quint32>();
				addr.setAddress(ip4);
				addr.setPort(6346);
			}
		}

		pPacket->m_nPosition = nNext;
	}

	const quint32 tNow = common::getTNowUTC();
	hostCache.addKey( addr, tNow, &m_oAddress, nKey, tNow );

#if LOG_QUERY_HANDLING
		systemLog.postLog( LogSeverity::Debug,
						   QString( "Got a query key from %1 via %2 = 0x%3" ).arg(
							   addr.toString().toLocal8Bit().constData()).arg(m_oAddress.toString().toLocal8Bit().constData()).arg(QString().number(nKey, 16)));
		//qDebug("Got a query key from %s via %s = 0x%x", addr.toString().toLocal8Bit().constData(), m_oAddress.toString().toLocal8Bit().constData(), nKey);
#endif // LOG_QUERY_HANDLING
}
void CG2Node::onQA(G2Packet* pPacket)
{
	QUuid oGUID;
	SearchManager.onQueryAcknowledge(pPacket, m_oAddress, oGUID);

	// TCP /QA - no need for routing, it's either for us or to be dropped
}

void CG2Node::onQH2(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	QueryHitInfo* pInfo = CQueryHit::readInfo(pPacket, &m_oAddress);

	if( securityManager.isVendorBlocked( pInfo->m_sVendor ) ) // Block foxy client search results. We can't download from them any way.
	{
		securityManager.ban( pInfo->m_oNodeAddress, Security::RuleTime::SixHours, true,
							 QString( "[AUTO] Vendor blocked (%1)" ).arg( pInfo->m_sVendor ), true
#if SECURITY_LOG_BAN_SOURCES
							 , QString( "g2node.cpp line 1371" )
#endif // SECURITY_LOG_BAN_SOURCES
							 );
	} else {
		if(SearchManager.onQueryHit(pPacket, pInfo))
		{
			networkG2.m_pSection.lock();

			if(Neighbours.isG2Hub() && pInfo->m_nHops < 7)
			{
				networkG2.m_oRoutingTable.add(pInfo->m_oNodeGUID, this, false);
				pPacket->m_pBuffer[pPacket->m_nLength - 17]++;
				networkG2.routePacket(pInfo->m_oGUID, pPacket);
			}

			networkG2.m_pSection.unlock();

			delete pInfo;
		}
	}
}

void CG2Node::onHaw(G2Packet *pPacket)
{
	if ( ! pPacket->m_bCompound ) 	return;

	QString strVendor;
	char szType[9];
	quint32 nLength = 0, nNext = 0;

	CEndPoint addr;

	while ( pPacket->readPacket( &szType[0], nLength ) )
	{
		nNext = pPacket->m_nPosition + nLength;

		if ( strcmp("V", szType) == 0 && nLength >= 4 )
		{
			strVendor = pPacket->readString( 4 );
		}
		else if ( strcmp("NA", szType) == 0 )
		{
			if(nLength >= 6)
			{
				if(nLength >= 18)
				{
					// IPv6 with port
					pPacket->readHostAddress(&addr, false);
				}
				else
				{
					// IPv4 with port
					pPacket->readHostAddress(&addr);
				}
			}
		}

		pPacket->m_nPosition = nNext;
	}

	if ( pPacket->getRemaining() < 2 + 16 ) return;

	BYTE* pPtr	= pPacket->m_pBuffer + pPacket->m_nPosition;
	BYTE nTTL	= pPacket->readByte();
	BYTE nHops	= pPacket->readByte();

	/*	QUuid oGUID = */pPacket->readGUID();

	if( !strVendor.isEmpty() && securityManager.isVendorBlocked(strVendor) )
	{
		securityManager.ban( addr, Security::RuleTime::SixHours, true,
							 QString( "[AUTO] Vendor blocked (%1)" ).arg( strVendor ), true
#if SECURITY_LOG_BAN_SOURCES
							 , QString( "g2node.cpp line 1545" )
#endif // SECURITY_LOG_BAN_SOURCES
							 );
		return;	// We don't want to propagate these...
	}
	else
	{
		hostCache.add( addr, common::getTNowUTC() );
	}

	if ( nTTL > 0 && nHops < 255 )
	{
		//m_pGUIDCache->Add( oGUID, this );

		pPtr[0] = nTTL  - 1;
		pPtr[1] = nHops + 1;

		if ( CG2Node* pNeighbour = (CG2Node*)Neighbours.randomNode( DiscoveryProtocol::G2,
																	G2_HUB,  this ) )
		{
			pNeighbour->sendPacket( pPacket, false, false );
		}
	}
}

void CG2Node::onQuery(G2Packet* pPacket)
{
	if(!pPacket->m_bCompound)
	{
		return;
	}

	CQueryPtr pQuery = CQuery::fromPacket(pPacket);

	if ( pQuery.isNull() )
	{
#if LOG_MALFORMATTED_QUERIES
		systemLog.postLog( LogSeverity::Error,  Components::G2,
						   "Received malformatted query from neighbour %s, ignoring.",
						   qPrintable( m_oAddress.toString() ) );
#endif // LOG_QUERY_HANDLING
		return;
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

	/*if( Neighbours.IsG2Hub() && m_nType == G2_HUB ) // must be both hubs
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

	if(networkG2.m_oRoutingTable.add(pQuery->m_oGUID, this, false))
	{
		if( m_nType == G2_LEAF )
		{
			sendPacket(Neighbours.createQueryAck(pQuery->m_oGUID, true, this, true), false, true);
		}

		if( Neighbours.isG2Hub() )
		{
			Neighbours.routeQuery(pQuery, pPacket, this, (m_nType != G2_HUB));
		}
	}
}

qint64 CG2Node::writeToNetwork(qint64 nBytes)
{
	qint64 nTotalSent = 0;

	do
	{
		if(getOutputBuffer()->isEmpty() && !m_lSendQueue.isEmpty())
		{
			G2Packet* pPacket = m_lSendQueue.dequeue();
			pPacket->toBuffer(getOutputBuffer());
			pPacket->release();
		}

		qint64 nSent = CNeighbour::writeToNetwork(nBytes - nTotalSent);

		if(nSent <= 0)
		{
			return nTotalSent ? nTotalSent : nSent;
		}

		nTotalSent += nSent;
	}
	while(nTotalSent < nBytes);

	return nTotalSent;
}

void CG2Node::onConnectNode()
{
	//QMutexLocker l(&Neighbours.m_pSection);

	systemLog.postLog ( LogSeverity::Information, Components::G2,
						"Connection with %s established, handshaking...",
						qPrintable( m_oAddress.toString() ) );

	// Generate G2 handshake and send it to node.
	initiateHandshake();
}

void CG2Node::onRead()
{
	Neighbours.m_pSection.lock();

	if ( m_nState == nsHandshaking )
	{
		if ( peek( bytesAvailable() ).indexOf("\r\n\r\n") != -1 )
		{
			if ( m_bInitiated )
			{
				// We have initiated the connection and are waiting for a handshake reply.
				// This parses the expected handshake message #2.
				parseHandshakeResponse();
			}
			else if ( !m_bHandshaking )
			{
				// We are not yet handshaking. This parses handshake message #1.
				parseIncomingHandshake();
			}
			else
			{
				// We are currently handshaking. This parses handshake message #3.
				parseHandShakeAccept();
			}
		}
	}
	else if ( m_nState == nsConnected )
	{
		G2Packet* pPacket = NULL;
		try
		{
			// read all packets from buffer, one after an other
			while ( ( pPacket = G2Packet::readBuffer( getInputBuffer() ) ) )
			{
				m_tLastPacketIn = time( 0 );
				++m_nPacketsIn;

				onPacket( pPacket );

				pPacket->release();
			}
		}
		catch ( ... )
		{
			if ( pPacket )
			{
				systemLog.postLog( LogSeverity::Debug, Components::G2,
								   QString( "%1" ).arg( pPacket->dump() ) );
				pPacket->release();
			}

			systemLog.postLog( LogSeverity::Debug, Components::G2,
							   QString( "Packet error - %1" ).arg( m_oAddress.toString() ) );
			close();
		}
	}

	Neighbours.m_pSection.unlock();
}

