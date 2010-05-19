#include "Handshake.h"
#include "Handshakes.h"
#include "network.h"

#include <QTcpSocket>

CHandshake::CHandshake(QObject* parent)
    :QObject(parent)
{
    m_pSocket = 0;
}
void CHandshake::acceptFrom(int handle)
{
    m_tConnected = time(0);
    m_pSocket = new QTcpSocket();
    m_pSocket->setReadBufferSize(1024);

	connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(OnRead()), Qt::QueuedConnection);
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));

    m_pSocket->setSocketDescriptor(handle);

    qDebug() << "Accepted an incoming connection from " << m_pSocket->peerAddress().toString().toAscii().constData();
    OnRead();
}
CHandshake::~CHandshake()
{
    if( m_pSocket )
    {
        m_pSocket->abort();
        m_pSocket->deleteLater();
    }

    Handshakes.RemoveHandshake(this);
}

void CHandshake::OnTimer(quint32 tNow)
{
    //qDebug() << "handshake timer";
	if( m_pSocket && tNow - m_tConnected > 15 )
    {
        qDebug() << "Timed out handshaking with " << m_pSocket->peerAddress().toString().toAscii().constData();
        m_pSocket->close();
        deleteLater();
    }
}
void CHandshake::OnRead()
{
    //qDebug() << "CHandshake::OnRead()";

    if( m_pSocket->bytesAvailable() < 8 )
        return;

    //qDebug() << "Handshake received: " << m_pSocket->peek(m_pSocket->bytesAvailable());

    if( m_pSocket->peek(8).startsWith("GNUTELLA") )
    {
        qDebug("Incoming connection from %s is Gnutella Neighbour connection", m_pSocket->peerAddress().toString().toAscii().constData());
        Network.OnAccept(m_pSocket);
        m_pSocket = 0;
        deleteLater();
    }
    else
    {
        qDebug("Closing connection with %s - unknown protocol", m_pSocket->peerAddress().toString().toAscii().constData());
        m_pSocket->abort();;
        deleteLater();
    }
}
