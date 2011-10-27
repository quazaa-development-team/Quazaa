/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
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

#include "chatsessiong2.h"

#include "quazaaglobals.h"
#include "quazaasettings.h"
#include "parser.h"
#include "g2packet.h"
#include "network.h"
#include <QXmlStreamReader>
#include "chatconverter.h"
#include <QTextDocument>

#ifdef _DEBUG
#include "debug_new.h"
#endif

/*
 *	Gnutella2 after handshake sequence
 *	L - local client (initiating chat connection)
 *	R - remote client
 *
 *	1. L => R /UPROC (mandatory)
 *	1. L <= R /UPROC (mandatory)
 *	2. L <= R /UPROD/XML with user profile (gprofile, xml, mandatory)
 *	2. L => R /UPROD/XML with gprofile (mandatory)
 *	3. L => R /CHATREQ (mandatory)
 *			  /CHATREQ/USERGUID (with R's raw GUID, read from R's /UPROD packet, mandatory)
 *	4. L <= R /CHATANS (mandatory)
 *			  /CHATANS/USERGUID (with R's raw GUID, mandatory)
 *			  /CHATANS/AWAY (optional, sent when R is away, payload text representation of idle time)
 *			  /CHATANS/ACCEPT (optional, sent if /?/USERGUID match and R wants to accept chat)
 *			  /CHATANS/DENY (optional, sent if /?/USERGUID do not match or R does not want to chat, in this case connection should be closed)
 *
 *	Exchanging messages
 *	/CMSG
 *	/CMSG/BODY (required, the message)
 *	/CMSG/ACT (optional, sent on /me events)
 *	/CMSG packet should be sent only after successfully completed above sequence, if sent before, it should be ignored
 *
 */

CChatSessionG2::CChatSessionG2(CEndPoint oRemoteHost, QObject *parent) :
	CChatSession(parent)
{
	m_oRemoteHost = m_oAddress = oRemoteHost;
}

void CChatSessionG2::Connect()
{
	CNetworkConnection::ConnectTo(m_oRemoteHost);
	CChatSession::Connect();
}

void CChatSessionG2::OnConnect()
{
	qDebug() << "OnConnect";

	emit systemMessage("Connected, handshaking...");

	qDebug() << "sending handshake";
	m_nState = csHandshaking;

	QByteArray baHs;

	baHs = "CHAT CONNECT/0.2\r\n";
	baHs+= "Accept: application/x-gnutella2\r\n";
        baHs+= "User-Agent: " + QuazaaGlobals::USER_AGENT_STRING() + "\r\n";
	baHs+= "Listen-IP: " + Network.GetLocalAddress().toStringWithPort() + "\r\n\r\n";

	Write(baHs);

	qDebug() << "Chat send:\n" << baHs;
}
void CChatSessionG2::OnDisconnect()
{
	CChatSession::OnDisconnect();
}
void CChatSessionG2::OnRead()
{
	if( m_nState == csHandshaking )
	{
		if(Peek(bytesAvailable()).indexOf("\r\n\r\n") != -1)
		{
			if(m_bInitiated)
			{
				ParseOutgoingHandshake();
			}
			else
			{
				//ParseIncomingHandshake();
			}
		}
	}
	else if( m_nState == csConnected || m_nState == csActive )
	{
		G2Packet* pPacket = 0;
		try
		{
			while((pPacket = G2Packet::ReadBuffer(GetInputBuffer())))
			{
				OnPacket(pPacket);

				pPacket->Release();
			}
		}
		catch(...)
		{
			if(pPacket)
			{
				pPacket->Release();
			}

			emit systemMessage("Received corrupted G2 packet, connection lost.");

			CNetworkConnection::Close();
		}
	}
}

void CChatSessionG2::ParseOutgoingHandshake()
{
	QString sHs = Read(Peek(bytesAvailable()).indexOf("\r\n\r\n") + 4);

	qDebug() << "Chat received:\n" << sHs;

	if( sHs.left(12) == "CHAT/0.2 200" )
	{
		QString sAccept = Parser::GetHeaderValue(sHs, "Accept");
		if( !sAccept.contains("application/x-gnutella2") )
		{
			Send_ChatError("503 Required protocol not accepted");
			CNetworkConnection::Close(true);
			emit systemMessage("Remote host does not support Gnutella2");
			return;
		}

		QString sContentType = Parser::GetHeaderValue(sHs, "Content-Type");
		if( !sContentType.contains("application/x-gnutella2") )
		{
			Send_ChatError("503 Required protocol not provided");
			CNetworkConnection::Close(true);
			emit systemMessage("Remote host does not support Gnutella2");
			return;
		}

		QString sUA = Parser::GetHeaderValue(sHs, "User-Agent");
		if( sUA.indexOf("shareaza", 0, Qt::CaseInsensitive) != -1 )
			m_bShareaza = true;

		emit systemMessage("Exchanging profile information...");
		m_nState = csConnected;
		Send_ChatOK(true);
		SendStartups();
		return;
	}

	qDebug() << m_oAddress.toStringWithPort() << "does not accept chat";

	Close();
}

void CChatSessionG2::Send_ChatOK(bool bReply)
{
	QByteArray sHs;

	sHs = "CHAT/0.2 200 OK\r\n";
	if( !bReply )
	{
		// 2nd header
		sHs+= "Accept: application/x-gnutella2\r\n";
                sHs+= "User-Agent: " + QuazaaGlobals::USER_AGENT_STRING() + "\r\n";
		sHs+= "Listen-IP: " + Network.GetLocalAddress().toStringWithPort() + "\r\n";
	}
	sHs+= "Content-Type: application/x-gnutella2\r\n\r\n";

	Write(sHs);

	qDebug() << "Chat send:\n" << sHs;

}

void CChatSessionG2::Send_ChatError(QString sReason)
{
	QByteArray sHs;

	sHs = "CHAT/0.2 " + sReason.toAscii() + "\r\n";
        sHs+= "User-Agent: " + QuazaaGlobals::USER_AGENT_STRING() + "\r\n\r\n";

	Write(sHs);
}

void CChatSessionG2::SendStartups()
{
	qDebug() << "sending startups";
	G2Packet* pPacket = G2Packet::New("UPROC", false);

	// A dirty workaround for Shareaza bug...
	if( m_bShareaza )
		pPacket->WriteString("dupa");

	SendPacket(pPacket);
}

void CChatSessionG2::SendPacket(G2Packet *pPacket, bool bRelease)
{
	qDebug() << "Sending packet" << pPacket->GetType();
	pPacket->ToBuffer(GetOutputBuffer());
	if(bRelease)
		pPacket->Release();
	emit readyToTransfer();
}

void CChatSessionG2::OnPacket(G2Packet *pPacket)
{
	qDebug() << "Received chat packet: " << pPacket->GetType();

	if( pPacket->IsType("UPROC") )
	{
		OnUPROC(pPacket);
	}
	else if( pPacket->IsType("UPROD") )
	{
		OnUPROD(pPacket);
	}
	else if( pPacket->IsType("CHATANS") )
	{
		OnCHATANS(pPacket);
	}
	else if( pPacket->IsType("CMSG") )
	{
		OnCMSG(pPacket);
	}
	else
	{
		qDebug() << "Received unknown chat packet: " << pPacket->GetType();
	}

}

void CChatSessionG2::OnUPROC(G2Packet *pPacket)
{
	Q_UNUSED(pPacket);

	// Temporary code, needs to be moved to GProfile class

	QString sXML = "<?xml version=\"1.0\"?><gprofile xmlns=\"http://www.shareaza.com/schemas/GProfile.xsd\"><gnutella guid=\"%1\"/><identity><handle primary=\"%2\"/></identity></gprofile>";
	sXML = sXML.arg(quazaaSettings.Profile.GUID.toString().toUpper().replace("{", "").replace("}", "")).arg(quazaaSettings.Profile.GnutellaScreenName);

	G2Packet* pD = G2Packet::New("UPROD", true);
	pD->WritePacket("XML", sXML.toUtf8().size());
	pD->WriteString(sXML);

	SendPacket(pD);
}

void CChatSessionG2::OnUPROD(G2Packet *pPacket)
{
	qDebug() << "OnUPROD";

	if( !pPacket->m_bCompound )
		return;

	QUuid oGUID;
	bool hasXML = false;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("XML", szType) == 0)
		{
			// Temporary code, needs to be moved to GProfile class
			QXmlStreamReader oXML;
			bool hasGprofile = false;
			bool hasGUID = false;
			bool hasIdentity = false;
			bool hasNick = false;
			oXML.addData(pPacket->ReadString(nLength));

			while(!oXML.atEnd() && !oXML.hasError())
			{
				QXmlStreamReader::TokenType token = oXML.readNext();

				if( token == QXmlStreamReader::StartDocument)
				{
					continue;
				}
				else if( token == QXmlStreamReader::StartElement )
				{
					if( oXML.name() == "gprofile" )
					{
						/*
						if( oXML.attributes().value("xmlns") != "http://www.shareaza.com/schemas/GProfile.xsd" )
							return;*/
						hasGprofile = true;
						continue;
					}
					else if( hasGprofile && oXML.name() == "gnutella" )
					{
						if( oXML.attributes().hasAttribute("guid") )
						{
							QString sTemp = oXML.attributes().value("guid").toString();
							QUuid oTemp(sTemp);
							oGUID = oTemp;
							hasGUID = true;
							continue;
						}
					}
					else if( hasGprofile && oXML.name() == "identity" )
					{
						hasIdentity = true;
						continue;
					}
					else if( hasGprofile && hasIdentity && oXML.name() == "handle" )
					{
						if( oXML.attributes().hasAttribute("primary") )
						{
							m_sNick = oXML.attributes().value("primary").toString();
							hasNick = true;
							continue;
						}
					}
				}
			}

			if( !hasNick || !hasGUID )
				return;

			hasXML = true;
		}

		pPacket->m_nPosition = nNext;
	}

	if( !hasXML )
		return;

	m_oGUID = oGUID;
	emit guidChanged(oGUID);
	emit nickChanged(m_sNick);

	if( m_bInitiated ) // TODO PUSH handling
	{
		G2Packet* pReq = G2Packet::New("CHATREQ", true);
		pReq->WritePacket("USERGUID", 16);
		pReq->WriteGUID(m_oGUID);

		SendPacket(pReq);
	}
}

void CChatSessionG2::OnCHATANS(G2Packet *pPacket)
{
	char szType[9];
	quint32 nLength = 0, nNext = 0;

	bool bAccepted = false;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("USERGUID", szType) == 0 && nLength >= 16)
		{
			QUuid oGUID;
			oGUID = pPacket->ReadGUID();
			if( !oGUID.isNull() )
			{
				m_oGUID = oGUID;
				emit guidChanged(oGUID);
			}
		}
		else if (strcmp("ACCEPT", szType) == 0 )
		{
			emit systemMessage("Private conversation accepted, you're now chatting with " + m_sNick);
			qDebug() << "chat accepted";
			bAccepted = true;
		}
		else if( strcmp("DENY", szType) == 0 )
		{
			emit systemMessage("Private conversation denied by remote host, sorry.");
			qDebug() << "chat denied";
			CNetworkConnection::Close();
			return;
		}
		else if( strcmp("AWAY", szType) == 0 )
		{
			emit systemMessage(m_sNick + " is away.");
			qDebug() << "he is away";
			bAccepted = true;
		}

		pPacket->m_nPosition = nNext;
	}

	if( bAccepted )
		m_nState = csActive;
}

void CChatSessionG2::OnCMSG(G2Packet *pPacket)
{
	if( !pPacket->m_bCompound || m_nState < csConnected )
		return;

	char szType[9];
	quint32 nLength = 0, nNext = 0;

	QString sMessage;
	bool bAction = false;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("BODY", szType) == 0)
		{
			sMessage = pPacket->ReadString(nLength);
		}
		else if (strcmp("ACT", szType) == 0 )
		{
			bAction = true;
		}

		pPacket->m_nPosition = nNext;
	}

	if( !sMessage.isEmpty() )
	{
		emit incomingMessage(sMessage, bAction);
	}
}

void CChatSessionG2::SendMessage(QString sMessage, bool bAction)
{
	qDebug() << "Send message:" << sMessage << bAction;

	G2Packet* pPacket = G2Packet::New("CMSG", true);
	if( bAction )
		pPacket->WritePacket("ACT", 0);

	G2Packet* pBody = G2Packet::New("BODY");
	pBody->WriteString(sMessage);
	pPacket->WritePacket(pBody);
	pBody->Release();

	SendPacket(pPacket);
}
void CChatSessionG2::SendMessage(QTextDocument *pMessage, bool bAction)
{
	CChatConverter oConv(pMessage);

	SendMessage(oConv.toBBCode(), bAction);

	delete pMessage;
}

