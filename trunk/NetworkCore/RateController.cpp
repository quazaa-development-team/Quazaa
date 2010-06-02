#include "RateController.h"
#include "NetworkConnection.h"

#include <limits>
#include <QAtomicInt>

CRateController::CRateController(QObject* parent): QObject(parent)
{
	m_bTransferSheduled = false;
    m_nUploadLimit = std::numeric_limits<qint32>::max() / 2;
    //m_nUploadLimit = 0x7fffffff;
    m_nDownloadLimit = std::numeric_limits<qint32>::max() / 2;
    //m_nDownloadLimit = 0x7fffffff;

    m_nDownload = m_nUpload = 0;
    m_nDownloadAvg = m_nUploadAvg = 0;
	m_bTransferring = false;

    m_tMeterTimer.start();

	m_tTransferTimer.setInterval(100);
	connect(&m_tTransferTimer, SIGNAL(timeout()), this, SLOT(transfer()), Qt::QueuedConnection);
}

void CRateController::AddSocket(CNetworkConnection* pSock)
{
    //qDebug() << "CRC /" << objectName() << "/ AddSocket " << pSock;
	connect(pSock, SIGNAL(readyToTransfer()), this, SLOT(transfer()));
    pSock->setReadBufferSize(8192);
    m_lSockets.insert(pSock);
    sheduleTransfer();
}
void CRateController::RemoveSocket(CNetworkConnection* pSock)
{
    //qDebug() << "CRC /" << objectName() << "/ RemoveSocket " << pSock;
	disconnect(pSock, SIGNAL(readyToTransfer()), this, SLOT(transfer()));
    pSock->setReadBufferSize(0);
    m_lSockets.remove(pSock);
}
void CRateController::sheduleTransfer()
{
	if( m_bTransferSheduled )
        return;

    m_bTransferSheduled = true;
	//QTimer::singleShot(100, this, SLOT(transfer()));
	m_tTransferTimer.start();
}
void CRateController::transfer()
{
	m_bTransferSheduled = false;
	m_tTransferTimer.stop();

    int nMsecs = 1000;
    if( !m_tStopWatch.isNull() )
        nMsecs = qMin(nMsecs, m_tStopWatch.elapsed());

    qint64 nToRead = (m_nDownloadLimit * nMsecs) / 1000;
    qint64 nToWrite = (m_nUploadLimit * nMsecs) / 1000;

	if( ( nToRead <= 0 && nToWrite <= 0 ) || m_bTransferring )
    {
        sheduleTransfer();
        return;
    }

    QSet<CNetworkConnection*> lSockets;
    foreach(CNetworkConnection* pSock, m_lSockets)
    {
        if( pSock->HasData() )
            lSockets.insert(pSock);
    }

    if( lSockets.isEmpty() )
	{
        return;
	}

    m_tStopWatch.start();
	m_bTransferring = true;

    bool bCanTransferMore = false;

    do
    {
        bCanTransferMore = false;
		qint64 nWriteChunk = qMax(qint64(1), nToWrite / lSockets.size());
		qint64 nReadChunk = qMax(qint64(1), nToRead / lSockets.size());

        QSetIterator<CNetworkConnection*> it(lSockets);
        while( it.hasNext() && (nToRead > 0 || nToWrite > 0))
        {
            CNetworkConnection* pConn = it.next();

            bool bDataTransferred = false;
			qint64 nAvailable = qMin(nReadChunk, pConn->networkBytesAvailable());
            if( nAvailable > 0 )
            {
				qint64 nReadBytes = pConn->readFromNetwork(qMin(nAvailable, nToRead));
                if( nReadBytes > 0 )
                {
                    nToRead -= nReadBytes;
                    m_nDownload += nReadBytes;
                    bDataTransferred = true;
                }
            }

			if( m_nUploadLimit * 2 > pConn->m_pSocket->bytesToWrite() )
            {
				qint64 nChunkSize = qMin(qMin(nWriteChunk, nToWrite), m_nUploadLimit * 2 - pConn->bytesToWrite());

                if( nChunkSize > 0 )
                {
                    qint64 nBytesWritten = pConn->writeToNetwork(nChunkSize);
                    if( nBytesWritten > 0)
                    {
                        nToWrite -= nBytesWritten;
                        m_nUpload += nBytesWritten;
                        bDataTransferred = true;
                    }
                }
            }

            if( bDataTransferred && pConn->HasData() )
            {
                bCanTransferMore = true;
            }
            else
            {
                lSockets.remove(pConn);
            }
        }
    }
	while ( bCanTransferMore && (nToRead > 0 || nToWrite > 0) && !lSockets.isEmpty() );

    if( m_tMeterTimer.elapsed() > 1000 )
        UpdateStats();

	m_bTransferring = false;

	if( bCanTransferMore )
        sheduleTransfer();
}

void CRateController::UpdateStats()
{
    quint32 tElapsed = m_tMeterTimer.elapsed();
    if( tElapsed < 1000 )
        return;

    m_tMeterTimer.start();

    m_nDownloadAvg = (m_nDownloadAvg + m_nDownload) / 2;
    m_nDownload = 0;
    m_nUploadAvg = (m_nUploadAvg + m_nUpload) / 2;
    m_nUpload = 0;

    /*
    qint32 nSteps = tElapsed / 125;

    for( ; nSteps > 0; nSteps-- )
    {
        for( int i = 62; i > 0; i-- )
        {
            m_nDownloadSlots[i + 1] = m_nDownloadSlots[i];
            m_nUploadSlots[i + 1] = m_nUploadSlots[i];
        }
        m_nDownloadSlots[0] = 0;
        m_nUploadSlots[0] = 0;
    }*/
}
