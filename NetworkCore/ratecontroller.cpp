//
// ratecontroller.cpp
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

#include "ratecontroller.h"
#include "networkconnection.h"

#include <limits>
#include <QTcpSocket>
#include <QMutexLocker>
#include <QTimer>

CRateController::CRateController(QObject* parent): QObject(parent)
{
	m_bTransferSheduled = false;
    m_nUploadLimit = std::numeric_limits<qint32>::max() / 2;
    m_nDownloadLimit = std::numeric_limits<qint32>::max() / 2;

    m_nDownload = m_nUpload = 0;
    m_nDownloadAvg = m_nUploadAvg = 0;

    m_tMeterTimer.start();
}

void CRateController::AddSocket(CNetworkConnection* pSock)
{
	QMutexLocker l(&m_oMutex);

    //qDebug() << "CRC /" << objectName() << "/ AddSocket " << pSock;
	connect(pSock, SIGNAL(readyToTransfer()), this, SLOT(transfer()));
    pSock->setReadBufferSize(8192);
    m_lSockets.insert(pSock);
    sheduleTransfer();
}
void CRateController::RemoveSocket(CNetworkConnection* pSock)
{
	QMutexLocker l(&m_oMutex);

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
	QTimer::singleShot(50, this, SLOT(transfer()));
}
void CRateController::transfer()
{
	if( !m_oMutex.tryLock() )
	{
		sheduleTransfer();
		return;
	}

	m_bTransferSheduled = false;

	qint64 nMsecs = 1000;
	if( !m_tStopWatch.isValid() )
        nMsecs = qMin(nMsecs, m_tStopWatch.elapsed());

    qint64 nToRead = (m_nDownloadLimit * nMsecs) / 1000;
    qint64 nToWrite = (m_nUploadLimit * nMsecs) / 1000;

	if( nToRead <= 0 && nToWrite <= 0 )
    {
        sheduleTransfer();
        return;
    }

    QSet<CNetworkConnection*> lSockets;
	for( QSet<CNetworkConnection*>::const_iterator itSocket = m_lSockets.constBegin(); itSocket != m_lSockets.constEnd(); ++itSocket )
	{
		CNetworkConnection* pSock = *itSocket;
		if( pSock->HasData() )
			lSockets.insert(pSock);
	}

    if( lSockets.isEmpty() )
	{
		m_oMutex.unlock();
        return;
	}

    m_tStopWatch.start();

    bool bCanTransferMore = false;

    do
    {
        bCanTransferMore = false;
		qint64 nWriteChunk = qMax(qint64(1), nToWrite / lSockets.size());
		qint64 nReadChunk = qMax(qint64(1), nToRead / lSockets.size());

		for( QSet<CNetworkConnection*>::iterator itSocket = lSockets.begin(); itSocket != lSockets.end() && (nToRead > 0 || nToWrite > 0);  )
		{
			CNetworkConnection* pConn = *itSocket;

			bool bDataTransferred = false;
			qint64 nAvailable = qMin(nReadChunk, pConn->networkBytesAvailable());
			if( nAvailable > 0 && nToRead > 0 )
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
				itSocket = lSockets.erase(itSocket);
				continue;
			}

			++itSocket;
		}

    }
	while ( bCanTransferMore && (nToRead > 0 || nToWrite > 0) && !lSockets.isEmpty() );

    if( m_tMeterTimer.elapsed() > 1000 )
        UpdateStats();

	if( bCanTransferMore )
        sheduleTransfer();

	m_oMutex.unlock();
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
