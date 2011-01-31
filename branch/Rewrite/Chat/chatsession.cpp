#include "chatsession.h"
#include "chatcore.h"
#include "network.h"
#include "quazaaglobals.h"
#include "quazaasettings.h"
#include "parser.h"
#include "g2packet.h"

#include <QXmlStreamReader>

CChatSession::CChatSession(DialogPrivateMessage* pDialog, QUuid* oGUID, QObject *parent) :
	CNetworkConnection(parent), m_pDialog(pDialog)
{
	if( pDialog )
		SetupDialog();

	if( oGUID != 0 )
		m_oGUID = *oGUID;

	m_nState = csNull;

	m_bShareaza = false;
}
CChatSession::~CChatSession()
{
	ChatCore.Remove(this);
}

void CChatSession::ConnectTo(CEndPoint oAddress)
{
	m_nState = csConnecting;
	CNetworkConnection::ConnectTo(oAddress);
	ChatCore.Add(this);
}


void CChatSession::OnTimer(quint32 tNow)
{
	if( m_nState < csConnected )
	{
		if( tNow - m_tConnected > quazaaSettings.Connection.TimeoutConnect )
		{
			Close();
		}
	}
	else if( m_nState == csConnected )
	{

	}
}

void CChatSession::OnConnect()
{
	qDebug() << "OnConnect";

	if( m_nProtocol == dpGnutella2 )
	{
		qDebug() << "sending handshake";
		m_nState = csHandshaking;

		QByteArray baHs;

		baHs = "CHAT CONNECT/0.2\r\n";
		baHs+= "Accept: application/x-gnutella2\r\n";
		baHs+= "User-Agent: " + quazaaGlobals.UserAgentString() + "\r\n";
		baHs+= "Listen-IP: " + Network.GetLocalAddress().toStringWithPort() + "\r\n\r\n";

		Write(baHs);

		qDebug() << "Chat send:\n" << baHs;
	}
	qDebug() << "OnConnect finish";
}
void CChatSession::OnDisconnect()
{
	delete this;
}
void CChatSession::OnRead()
{
	if( m_nState == csHandshaking )
	{
		if( m_nProtocol == dpGnutella2 )
		{
			if(Peek(bytesAvailable()).indexOf("\r\n\r\n") != -1)
			{
				if(m_bInitiated)
				{
					ParseOutgoingHandshakeG2();
				}
				else
				{
					//ParseIncomingHandshakeG2();
				}
			}
		}
	}
	else if( m_nState == csConnected )
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

			Close();
		}
		m_bReadyReadSent = false;
	}
}
void CChatSession::OnError(QAbstractSocket::SocketError e)
{
	Q_UNUSED(e);
}

void CChatSession::OnStateChange(QAbstractSocket::SocketState s)
{
	Q_UNUSED(s);
}

void CChatSession::SetupDialog()
{

}

void CChatSession::ParseOutgoingHandshakeG2()
{
	QString sHs = Read(Peek(bytesAvailable()).indexOf("\r\n\r\n") + 4);

	qDebug() << "Chat received:\n" << sHs;

	if( sHs.left(12) == "CHAT/0.2 200" )
	{
		QString sAccept = Parser::GetHeaderValue(sHs, "Accept");
		if( !sAccept.contains("application/x-gnutella2") )
		{
			Send_ChatErrorG2("503 Required protocol not accepted");
			Close(true);
			return;
		}

		QString sContentType = Parser::GetHeaderValue(sHs, "Content-Type");
		if( !sContentType.contains("application/x-gnutella2") )
		{
			Send_ChatErrorG2("503 Required protocol not provided");
			Close(true);
			return;
		}

		QString sUA = Parser::GetHeaderValue(sHs, "User-Agent");
		if( sUA.indexOf("shareaza", 0, Qt::CaseInsensitive) != -1 )
			m_bShareaza = true;

		m_nState = csConnected;
		Send_ChatOKG2(true);
		SendStartupsG2();
		return;
	}

	qDebug() << m_oAddress.toStringWithPort() << "does not accept chat";

	Close();
}

void CChatSession::Send_ChatOKG2(bool bReply)
{
	QByteArray sHs;

	sHs = "CHAT/0.2 200 OK\r\n";
	if( !bReply )
	{
		// 2nd header
		sHs+= "Accept: application/x-gnutella2\r\n";
		sHs+= "User-Agent: " + quazaaGlobals.UserAgentString() + "\r\n";
		sHs+= "Listen-IP: " + Network.GetLocalAddress().toStringWithPort() + "\r\n";
	}
	sHs+= "Content-Type: application/x-gnutella2\r\n\r\n";

	Write(sHs);

	qDebug() << "Chat send:\n" << sHs;

}

void CChatSession::Send_ChatErrorG2(QString sReason)
{
	QByteArray sHs;

	sHs = "CHAT/0.2 " + sReason.toAscii() + "\r\n";
	sHs+= "User-Agent: " + quazaaGlobals.UserAgentString() + "\r\n\r\n";

	Write(sHs);
}

void CChatSession::SendStartupsG2()
{
	qDebug() << "sending startups";
	G2Packet* pPacket = G2Packet::New("UPROC", false);

	// A dirty workaround for Shareaza bug...
	if( m_bShareaza )
		pPacket->WriteString("dupa");

	SendPacketG2(pPacket);
}

void CChatSession::SendPacketG2(G2Packet *pPacket, bool bRelease)
{
	qDebug() << "Sending packet" << pPacket->GetType();
	pPacket->ToBuffer(GetOutputBuffer());
	if(bRelease)
		pPacket->Release();
	emit readyToTransfer();
}

void CChatSession::OnPacket(G2Packet *pPacket)
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
	else
	{
		qDebug() << "Received unknown chat packet: " << pPacket->GetType();
	}

}

void CChatSession::OnUPROC(G2Packet *pPacket)
{
	Q_UNUSED(pPacket);

	// Temporary code, needs to be moved to GProfile class

	QString sXML = "<?xml version=\"1.0\"?><gprofile xmlns=\"http://www.shareaza.com/schemas/GProfile.xsd\"><gnutella guid=\"%1\"/><identity><handle primary=\"%2\"/></identity></gprofile>";
	sXML = sXML.arg(quazaaSettings.Profile.GUID.toString().toUpper().replace("{", "").replace("}", "")).arg(quazaaSettings.Profile.GnutellaScreenName);

	G2Packet* pD = G2Packet::New("UPROD", true);
	pD->WritePacket("XML", sXML.toUtf8().size());
	pD->WriteString(sXML);

	SendPacketG2(pD);
}

void CChatSession::OnUPROD(G2Packet *pPacket)
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

	if( /*m_pDialog != 0*/ m_bInitiated ) // TODO PUSH handling
	{
		G2Packet* pReq = G2Packet::New("CHATREQ", true);
		pReq->WritePacket("USERGUID", 16);
		pReq->WriteGUID(m_oGUID);

		SendPacketG2(pReq);
	}
}

void CChatSession::OnCHATANS(G2Packet *pPacket)
{
	char szType[9];
	quint32 nLength = 0, nNext = 0;

	while(pPacket->ReadPacket(&szType[0], nLength))
	{
		nNext = pPacket->m_nPosition + nLength;

		if(strcmp("USERGUID", szType) == 0 && nLength >= 16)
		{
			QUuid oGUID;
			oGUID = pPacket->ReadGUID();
			if( !oGUID.isNull() )
				m_oGUID = oGUID;
		}
		else if (strcmp("ACCEPT", szType) == 0 )
		{
			qDebug() << "chat accepted";
			// ignore this child
			continue;
		}
		else if( strcmp("DENY", szType) == 0 )
		{
			qDebug() << "chat denied";
			Close();
			return;
		}
		else if( strcmp("AWAY", szType) == 0 )
		{
			qDebug() << "he is away";
		}


		pPacket->m_nPosition = nNext;
	}
}


