#include "NetworkConnection.h"

#include <QHostAddress>

CNetworkConnection::CNetworkConnection(QObject* parent)
    :QTcpSocket(parent)
{
    //qDebug() << "CNetworkConnection constructor";

    m_pSocket = 0;

    m_pInput = 0;
    m_pOutput = 0;
	m_nInputSize = 8 * 1024;

    m_bInitiated = false;
    m_bConnected = false;
    m_tConnected = 0;
    m_tLastRotate = time(0);

    m_nTotalInput = m_nTotalOutput = 0;

    memset(&m_nInput, 0, sizeof(m_nInput));
    memset(&m_nOutput, 0, sizeof(m_nOutput));

	connect(this, SIGNAL(readyRead()), this, SIGNAL(readyToTransfer()), Qt::QueuedConnection);
    connect(this, SIGNAL(connected()), this, SIGNAL(readyToTransfer()));
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

void CNetworkConnection::connectToHost(IPv4_ENDPOINT oAddress)
{
    QHostAddress hostAddr(oAddress.ip);
    m_oAddress = oAddress;
    m_bInitiated = true;
    m_bConnected = false;
    m_tConnected = time(0);

    Q_ASSERT(m_pInput == 0);
    Q_ASSERT(m_pOutput == 0);
    m_pInput = new QByteArray();
    m_pOutput = new QByteArray();
    Q_ASSERT(m_pSocket == 0);

    m_pSocket = new QTcpSocket(this);
    initializeSocket();

    //m_pSocket->connectToHost(hostAddr, m_oAddress.port);
    QTcpSocket::connectToHost(hostAddr, m_oAddress.port);
    /*qDebug() << "connectToHost - object dump";
    m_pSocket->dumpObjectInfo();
    dumpObjectInfo();
    qDebug() << "--- end of dump ---";*/

}

void CNetworkConnection::AttachTo(QTcpSocket *pOther)
{
    Q_ASSERT(m_pSocket == 0);
    m_pSocket = pOther;
    m_pSocket->disconnect();

    m_bConnected = true;
    m_tConnected = time(0);
    m_bInitiated = false;

    Q_ASSERT(m_pInput == 0);
    Q_ASSERT(m_pOutput == 0);
    m_pInput = new QByteArray();
    m_pOutput = new QByteArray();

    m_oAddress.ip = m_pSocket->peerAddress().toIPv4Address();
    m_oAddress.port = m_pSocket->peerPort();

    initializeSocket();
    //GetInputBuffer()->append(pOther->readAll());
    //emit readyToTransfer();
}
void CNetworkConnection::initializeSocket()
{
    m_pSocket->setReadBufferSize(m_nInputSize);
    connect(m_pSocket, SIGNAL(connected()),
            this, SIGNAL(connected()));
	connect(m_pSocket, SIGNAL(readyRead()),
			this, SIGNAL(readyRead()));
    connect(m_pSocket, SIGNAL(disconnected()),
            this, SIGNAL(disconnected()));
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SIGNAL(error(QAbstractSocket::SocketError)));
    connect(m_pSocket, SIGNAL(bytesWritten(qint64)),
            this, SIGNAL(bytesWritten(qint64)));
    connect(m_pSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    setOpenMode(m_pSocket->openMode());
    setSocketState(m_pSocket->state());
}

qint64 CNetworkConnection::readFromNetwork(qint64 nBytes)
{
    Q_ASSERT(m_pInput != 0);
    Q_ASSERT(nBytes >= 0);

    int nOldSize = m_pInput->size();
    m_pInput->resize(nOldSize + nBytes);
    //qint64 nBytesRead = m_pSocket->readData(m_pInput->data() + nOldSize, nBytes);
    qint64 nBytesRead = m_pSocket->read(m_pInput->data() + nOldSize, nBytes);
    m_pInput->resize((nBytesRead <= 0 ? nOldSize : nOldSize + nBytesRead));

    if( m_pSocket->state() != ConnectedState )
    {
        m_pInput->append(m_pSocket->readAll());
        nBytesRead += m_pInput->size() - (nOldSize + nBytesRead);
    }

    if( nBytesRead > 0 )
    {
        AddIn(nBytesRead);
        emit readyRead();
    }

	if( (uint)m_pInput->capacity() > m_nInputSize && (uint)m_pInput->size() < m_nInputSize / 2 )
		m_pInput->squeeze();

    return nBytesRead;
}
qint64 CNetworkConnection::writeToNetwork(qint64 nBytes)
{
    Q_ASSERT(m_pOutput != 0);
    Q_ASSERT(nBytes >= 0);

    //qint64 nBytesWritten = m_pSocket->writeData(m_pOutput->data(), qMin((qint64)m_pOutput->size(), nBytes));
    qint64 nBytesWritten = m_pSocket->write(m_pOutput->data(), qMin((qint64)m_pOutput->size(), nBytes));

    if( nBytesWritten <= 0 )
        return nBytesWritten;

    m_pOutput->remove(0, nBytesWritten);
    AddOut(nBytesWritten);

	if( (uint)m_pOutput->capacity() > m_nInputSize && (uint)m_pOutput->size() < m_nInputSize / 2 )
		m_pOutput->squeeze();

    return nBytesWritten;
}
qint64 CNetworkConnection::readData(char* data, qint64 maxlen)
{
    Q_ASSERT(m_pInput != 0);

    int nBytesRead = qMin<qint64>(maxlen, m_pInput->size());
    memcpy(data, m_pInput->constData(), nBytesRead);
    m_pInput->remove(0, nBytesRead);

    if( m_pSocket->state() != ConnectedState )
    {
        int nOldSize = m_pInput->size();
        m_pInput->resize(nOldSize + m_pSocket->bytesAvailable());
        //m_pSocket->readData(m_pInput->data() + nOldSize, m_pInput->size() - nOldSize);
        m_pSocket->read(m_pInput->data() + nOldSize, m_pInput->size() - nOldSize);
    }

    return (qint64)nBytesRead;
}
qint64 CNetworkConnection::readLineData(char* data, qint64 maxlen)
{
    //return m_pSocket->readLineData(data, maxlen);
    return QIODevice::readLineData(data, maxlen);
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

qint64 CNetworkConnection::peek(char *data, qint64 maxlen)
{
    qint64 nBytes = qMin((qint64)GetInputBuffer()->size(), maxlen);
    memcpy(data, GetInputBuffer()->constData(), nBytes);

    return nBytes;
}
QByteArray CNetworkConnection::peek(qint64 maxlen)
{
    qint64 nBytes = qMin((qint64)GetInputBuffer()->size(), maxlen);

    return GetInputBuffer()->left(nBytes);
}
qint64 CNetworkConnection::read(char *data, qint64 maxlen)
{
    return readData(data, maxlen);
}
QByteArray CNetworkConnection::read(qint64 maxlen)
{
    QByteArray oResult;
    qint64 nToRead = qMin<qint64>(GetInputBuffer()->size(), maxlen);
    oResult.resize(nToRead);
    readData(oResult.data(), nToRead);
    return oResult;
}

void CNetworkConnection::socketStateChanged(QAbstractSocket::SocketState state)
{
    setLocalAddress(m_pSocket->localAddress());
    setLocalPort(m_pSocket->localPort());
    setPeerName(m_pSocket->peerName());
    setPeerAddress(m_pSocket->peerAddress());
    setPeerPort(m_pSocket->peerPort());
    setSocketState(state);
}

void CNetworkConnection::connectToHostImplementation(const QString &hostName,
                                                     quint16 port, OpenMode openMode)
{
    setOpenMode(openMode);
    m_pSocket->connectToHost(hostName, port, openMode);
}

void CNetworkConnection::diconnectFromHostImplementation()
{
    m_pSocket->disconnectFromHost();
}
