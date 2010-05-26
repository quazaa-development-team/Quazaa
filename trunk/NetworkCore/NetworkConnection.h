#ifndef NETWORKCONNECTION_H
#define NETWORKCONNECTION_H

#include <QTcpSocket>

#include "types.h"

class QByteArray;

class CNetworkConnection : public QTcpSocket
{
    Q_OBJECT
public:
    QTcpSocket* m_pSocket;  // sockecik ;)

    // Adres hosta
    IPv4_ENDPOINT   m_oAddress;

    // Bufory I/O
    QByteArray* m_pInput;
    QByteArray* m_pOutput;
    quint64     m_nInputSize;

    bool    m_bInitiated;
    bool    m_bConnected;
    qint32  m_tConnected;

    // Statystyki
    quint32 m_nInput[8];
    quint32 m_nOutput[8];
    quint32 m_tLastRotate;  // czas kiedy ostatnio cos robilismy z pasmem

    quint64 m_nTotalInput;  // ilosc danych odebranych z sieci
    quint64 m_nTotalOutput;

public:
    CNetworkConnection(QObject* parent = 0);
    virtual ~CNetworkConnection();

public:
    virtual void connectToHost(IPv4_ENDPOINT oAddress);
    virtual void AttachTo(QTcpSocket* pOther);
    qint64 peek(char* data, qint64 maxlen);
    QByteArray peek(qint64 maxlen);
    QByteArray read(qint64 maxlen);
    qint64 read(char *data, qint64 maxlen);

public:
    virtual qint64 readFromNetwork(qint64 nBytes);
    virtual qint64 writeToNetwork(qint64 nBytes);

protected:
    virtual qint64 readData(char* data, qint64 maxlen);
    virtual qint64 readLineData(char* data, qint64 maxlen);
    virtual qint64 writeData(const char* data, qint64 len);

    void initializeSocket();

public:
    inline quint64  GetTotalIn() const
    {
        return m_nTotalInput;
    }
    inline quint64  GetTotalOut() const
    {
        return m_nTotalOutput;
    }
    inline void UpdateBandwidth(quint32 tNow = 0)
    {
        if( tNow == 0 )
            tNow = time(0);

        qint32 tDiff = tNow - m_tLastRotate;

        if( tDiff == 0 )
            return;

        if( tDiff > 8 )
        {
            for( qint32 i = 0; i < 8; i++ )
            {
                m_nInput[i] = 0;
                m_nOutput[i] = 0;
            }
        }
        else
        {
            for( qint32 i = 0; i < tDiff; i++ )
            {
                for( qint32 j = 6; j >= 0; --j )
                {
                    m_nInput[j + 1] = m_nInput[j];
                    m_nOutput[j + 1] = m_nOutput[j];
                }
                m_nInput[0] = 0;
                m_nOutput[0] = 0;
            }

        }
        m_tLastRotate = tNow;
    }
    inline quint32 AvgIn(quint32 tNow = 0)
    {
        if( tNow == 0 )
            tNow = time(0);

        UpdateBandwidth(tNow);

        quint32 nRet = 0;
        for( qint32 i = 0; i < 8; i++ )
        {
            nRet += m_nInput[i];
        }

        return nRet / 8;
    }
    inline quint32 AvgOut(quint32 tNow = 0)
    {
        if( tNow == 0 )
            tNow = time(0);

        UpdateBandwidth(tNow);

        quint32 nRet = 0;
        for( qint32 i = 0; i < 8; i++ )
        {
            nRet += m_nOutput[i];
        }

        return nRet / 8;
    }
    inline void AddIn(quint32 nLength)
    {
        m_nInput[0] += nLength;
        m_nTotalInput += nLength;
    }
    inline void AddOut(quint32 nLength)
    {
        m_nOutput[0] += nLength;
        m_nTotalOutput += nLength;
    }
    inline IPv4_ENDPOINT GetAddress()
    {
        return m_oAddress;
    }

    inline virtual bool HasData()
    {
        if( !m_pSocket )
            return false;

        bool bRet = false;
        if( m_pInput && !m_pInput->isEmpty() )
            bRet |= true;
        if( m_pOutput && !m_pOutput->isEmpty() )
            bRet |= true;
        if( networkBytesAvailable() )
            bRet |= true;
        return bRet;
    }
    inline qint64 bytesAvailable()
    {
        Q_ASSERT(m_pInput != 0);

        if( m_pSocket->state() != ConnectedState )
        {
            /*int nOldSize = m_pInput->size();
            m_pInput->resize(nOldSize + m_pSocket->bytesAvailable());
            m_pSocket->readData(m_pInput->data() + nOldSize, m_pInput->size() - nOldSize);*/
            m_pInput->append(m_pSocket->readAll());
        }

        return m_pInput->size();
    }
    inline qint64 bytesToWrite() const
    {
        if( !m_pSocket )
            return 0;

        return m_pSocket->bytesToWrite() + m_pOutput->size();
    }
    inline bool isValid() const
    {
        if( m_pSocket == 0 || m_pInput == 0 || m_pOutput == 0 )
            return false;

        return m_pSocket->isValid();
    }
    inline qint64 networkBytesAvailable() const
    {
        if( !isValid() )
            return 0;

        if( m_nInputSize > 0 )
        {
            return qMax(qint64(0), qMin(m_pSocket->bytesAvailable(), qint64(m_nInputSize - m_pInput->size())));
        }

        return m_pSocket->bytesAvailable();
    }

    inline virtual QByteArray* GetInputBuffer()
    {
        Q_ASSERT(m_pInput != 0);

        return m_pInput;
    }
    inline virtual QByteArray* GetOutputBuffer()
    {
        Q_ASSERT(m_pOutput != 0);

        return m_pOutput;
    }
    inline void setReadBufferSize(qint64 nSize)
    {
        m_nInputSize = nSize;
        if( m_pSocket )
            m_pSocket->setReadBufferSize(nSize);
    }

signals:
    void readyToTransfer();

private slots:
    void socketStateChanged(QAbstractSocket::SocketState state);

protected slots:
    void connectToHostImplementation(const QString &hostName,
                                     quint16 port, OpenMode openMode = ReadWrite);
    void diconnectFromHostImplementation();
	void OnAboutToClose();

    friend class CRateController;
};


#endif // NETWORKCONNECTION_H
