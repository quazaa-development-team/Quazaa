//
// networkconnection.cpp
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

#include "networkconnection.h"

#include <QHostAddress>
#include <QTcpSocket>
#include <QMetaType>

#include "buffer.h"

CNetworkConnection::CNetworkConnection(QObject* parent)
	:QObject(parent)
{
	static bool bMetaRegistered = false;
	if( !bMetaRegistered )
	{
		qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
		qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
		bMetaRegistered = true;
	}

    //qDebug() << "CNetworkConnection constructor";

    m_pSocket = 0;

    m_pInput = 0;
    m_pOutput = 0;

    m_bInitiated = false;
    m_bConnected = false;
    m_tConnected = 0;
	m_bDelayedClose = false;
	m_bReadyReadSent = false;
}
CNetworkConnection::~CNetworkConnection()
{
    //qDebug() << "CNetworkConnection destructor";

    if( m_pInput )
        delete m_pInput;
    if( m_pOutput )
        delete m_pOutput;

    if( m_pSocket )
        m_pSocket->deleteLater();
}

void CNetworkConnection::ConnectTo(IPv4_ENDPOINT oAddress)
{
	QHostAddress hostAddr(oAddress.ip);

	ConnectTo(hostAddr, oAddress.port);
}
void CNetworkConnection::ConnectTo(QHostAddress oAddress, quint16 nPort)
{
	m_oAddress.ip = oAddress.toIPv4Address();
	m_oAddress.port = nPort;

	m_bInitiated = true;
	m_bConnected = false;
	m_tConnected = time(0);

	Q_ASSERT(m_pInput == 0);
	Q_ASSERT(m_pOutput == 0);
	m_pInput = new CBuffer(8192);
	m_pOutput = new CBuffer(8192);
	Q_ASSERT(m_pSocket == 0);

	m_pSocket = new QTcpSocket();
	initializeSocket();
	m_pSocket->connectToHost(oAddress, nPort);

}

void CNetworkConnection::AttachTo(CNetworkConnection *pOther)
{
    Q_ASSERT(m_pSocket == 0);
	m_pSocket = pOther->m_pSocket;
	pOther->m_pSocket = 0;

	m_bConnected = pOther->m_bConnected;
	m_tConnected = pOther->m_tConnected;
	m_bInitiated = pOther->m_bInitiated;

    Q_ASSERT(m_pInput == 0);
    Q_ASSERT(m_pOutput == 0);
	m_pInput = pOther->m_pInput;
	m_pOutput = pOther->m_pOutput;
	pOther->m_pInput = pOther->m_pOutput = 0;

    m_oAddress.ip = m_pSocket->peerAddress().toIPv4Address();
    m_oAddress.port = m_pSocket->peerPort();

    initializeSocket();

	if( !m_bReadyReadSent )
	{
		m_bReadyReadSent = true;
		emit readyRead();
	}

}
void CNetworkConnection::AcceptFrom(int nHandle)
{
	Q_ASSERT(m_pSocket == 0);
	m_pSocket = new QTcpSocket();

	initializeSocket();

	m_bInitiated = false;
	m_bConnected = true;
	m_tConnected = time(0);

	Q_ASSERT(m_pInput == 0);
	Q_ASSERT(m_pOutput == 0);
	m_pInput = new CBuffer(8192);
	m_pOutput = new CBuffer(8192);

	m_pSocket->setSocketDescriptor(nHandle);
	m_oAddress.ip = m_pSocket->peerAddress().toIPv4Address();
	m_oAddress.port = m_pSocket->peerPort();

	emit readyToTransfer();

}

void CNetworkConnection::Close(bool bDelayed)
{
	if( bDelayed )
	{
		m_bDelayedClose = true;
		if( !GetOutputBuffer()->isEmpty() || !m_pOutput->isEmpty() )
		{
			writeToNetwork(m_pOutput->size() + GetOutputBuffer()->size());
			m_pSocket->flush();
		}
		m_pSocket->close();
	}
	else
	{
		m_pSocket->abort();
	}
	emit disconnected();
}

void CNetworkConnection::moveToThread(QThread *thread)
{
	if( m_pSocket )
		m_pSocket->moveToThread(thread);
	QObject::moveToThread(thread);
}

void CNetworkConnection::initializeSocket()
{
	m_pSocket->disconnect();

	connect(m_pSocket, SIGNAL(connected()),
			this, SIGNAL(connected()));
	connect(m_pSocket, SIGNAL(connected()),
			this, SIGNAL(readyToTransfer()));
	connect(m_pSocket, SIGNAL(readyRead()),
			this, SIGNAL(readyToTransfer()));
    connect(m_pSocket, SIGNAL(disconnected()),
			this, SIGNAL(disconnected()));
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SIGNAL(error(QAbstractSocket::SocketError)));
    connect(m_pSocket, SIGNAL(bytesWritten(qint64)),
			this, SIGNAL(bytesWritten(qint64)));
    connect(m_pSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
			this, SIGNAL(stateChanged(QAbstractSocket::SocketState)));
	connect(m_pSocket, SIGNAL(aboutToClose()),
			this, SLOT(OnAboutToClose()));
}

qint64 CNetworkConnection::readFromNetwork(qint64 nBytes)
{
    Q_ASSERT(m_pInput != 0);
    Q_ASSERT(nBytes >= 0);

    int nOldSize = m_pInput->size();
	qint64 nBytesRead = 0;

	if( m_pSocket->state() != QTcpSocket::ConnectedState )
    {
        m_pInput->append(m_pSocket->readAll());
		nBytesRead += m_pInput->size() - nOldSize;
    }
	else
	{
		m_pInput->resize(nOldSize + nBytes);
		nBytesRead = m_pSocket->read(m_pInput->data() + nOldSize, nBytes);
		m_pInput->resize((nBytesRead <= 0 ? nOldSize : nOldSize + nBytesRead));
	}

    if( nBytesRead > 0 )
    {
		m_mInput.Add(nBytesRead);
		if( !m_bReadyReadSent )
		{
			m_bReadyReadSent = true;
			emit readyRead();
		}
    }

    return nBytesRead;
}
qint64 CNetworkConnection::writeToNetwork(qint64 nBytes)
{
    Q_ASSERT(m_pOutput != 0);
    Q_ASSERT(nBytes >= 0);

    qint64 nBytesWritten = m_pSocket->write(m_pOutput->data(), qMin((qint64)m_pOutput->size(), nBytes));

    if( nBytesWritten <= 0 )
        return nBytesWritten;

    m_pOutput->remove(0, nBytesWritten);
	m_mOutput.Add(nBytesWritten);

    return nBytesWritten;
}
qint64 CNetworkConnection::readData(char* data, qint64 maxlen)
{
    Q_ASSERT(m_pInput != 0);

    int nBytesRead = qMin<qint64>(maxlen, m_pInput->size());
	memcpy(data, m_pInput->data(), nBytesRead);
    m_pInput->remove(0, nBytesRead);

	if( m_pSocket->state() != QTcpSocket::ConnectedState )
    {
        int nOldSize = m_pInput->size();
        m_pInput->resize(nOldSize + m_pSocket->bytesAvailable());
        m_pSocket->read(m_pInput->data() + nOldSize, m_pInput->size() - nOldSize);
    }

	m_bReadyReadSent = false;

    return (qint64)nBytesRead;
}

qint64 CNetworkConnection::writeData(const char* data, qint64 len)
{
    Q_ASSERT(m_pOutput != 0);

    int nOldSize = m_pOutput->size();
    m_pOutput->resize(nOldSize + len);
    memcpy(m_pOutput->data() + nOldSize, data, len);
    emit readyToTransfer();
    return len;
}


void CNetworkConnection::OnAboutToClose()
{
	if( m_bDelayedClose && (!GetOutputBuffer()->isEmpty() || !m_pOutput->isEmpty()) )
	{
		writeToNetwork(m_pOutput->size() + GetOutputBuffer()->size());
	}
	emit aboutToClose();
}


qint64 CNetworkConnection::bytesAvailable()
{
	Q_ASSERT(m_pInput != 0);

	if( m_pSocket->state() != QTcpSocket::ConnectedState )
	{
		m_pInput->append(m_pSocket->readAll());
	}

	return m_pInput->size();
}

qint64 CNetworkConnection::bytesToWrite() const
{
	if( !m_pSocket )
		return 0;

	return m_pSocket->bytesToWrite() + m_pOutput->size();
}
qint64 CNetworkConnection::networkBytesAvailable() const
{
	if( !isValid() )
		return 0;

	if( m_pSocket->state() != QTcpSocket::ConnectedState )
	{
		int nOldSize = m_pInput->size();
		m_pInput->append(m_pSocket->readAll());
		return m_pInput->size() - nOldSize;
	}

	return m_pSocket->bytesAvailable();
}
bool CNetworkConnection::isValid() const
{
	if( m_pSocket == 0 || m_pInput == 0 || m_pOutput == 0 )
		return false;

	return m_pSocket->isValid();
}
void CNetworkConnection::setReadBufferSize(qint64 nSize)
{
	if( m_pSocket )
		m_pSocket->setReadBufferSize(nSize);
}

QByteArray CNetworkConnection::Read(qint64 nMaxSize)
{
	QByteArray baRet;

	baRet.resize(qMin<qint64>(nMaxSize, GetInputBuffer()->size()));
	readData(baRet.data(), baRet.size());
	return baRet;
}

QByteArray CNetworkConnection::Peek(qint64 nMaxLength)
{
	CBuffer* pBuffer = GetInputBuffer();

	return QByteArray::fromRawData(pBuffer->data(), qMin<qint64>(nMaxLength > 0 ? nMaxLength : pBuffer->size(), pBuffer->size()));
}

TCPBandwidthMeter::TCPBandwidthMeter()
{
	memset(&m_pSlots[0], 0, sizeof(m_pSlots));
	m_nCurrentSlot = m_nTotal = 0;
	m_tTime.start();
}

void TCPBandwidthMeter::Add(quint32 nBytes)
{
	if( m_tTime.hasExpired(250) )
	{
		int nSlotsToClear = qMin(20ll, m_tTime.elapsed() / 250ll);

		for( ; nSlotsToClear; nSlotsToClear-- )
		{
			m_nCurrentSlot = (m_nCurrentSlot + 1) % 20;
			m_pSlots[m_nCurrentSlot] = 0;
		}

		m_tTime.start();
	}

	m_pSlots[m_nCurrentSlot] += nBytes;
	m_nTotal += nBytes;
}
quint32 TCPBandwidthMeter::AvgUsage()
{
	quint64 nTotal = 0;

	if( m_tTime.hasExpired(250) )
	{
		int nSlotsToClear = qMin(20ll, m_tTime.elapsed() / 250ll);

		for( ; nSlotsToClear; nSlotsToClear-- )
		{
			m_nCurrentSlot = (m_nCurrentSlot + 1) % 20;
			m_pSlots[m_nCurrentSlot] = 0;
		}

		m_tTime.start();
	}

	for( int i = 0; i < 20; i++ )
		nTotal += m_pSlots[i];

	return nTotal / 5;
}
quint32 TCPBandwidthMeter::Usage()
{
	if( m_tTime.hasExpired(5000) )
		return 0;

	int nSlot = m_nCurrentSlot;
	quint32 nUsage = 0;

	for( int i = 0; i < 4; i++ )
	{
		nUsage += m_pSlots[nSlot];
		nSlot--;
		if( nSlot < 0 )
			nSlot = 19;
	}

	return nUsage;
}
