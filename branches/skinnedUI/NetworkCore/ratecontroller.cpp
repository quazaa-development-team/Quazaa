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
#include <QMetaObject>

CRateController::CRateController(QMutex* pMutex, QObject* parent): QObject(parent)
{
	m_bTransferSheduled = false;
	m_nUploadLimit = std::numeric_limits<qint32>::max() / 2;
	m_nDownloadLimit = std::numeric_limits<qint32>::max() / 2;
	m_tStopWatch.invalidate();

	m_pMutex = pMutex;
}

void CRateController::AddSocket(CNetworkConnection* pSock)
{
	connect(pSock, SIGNAL(readyToTransfer()), this, SLOT(sheduleTransfer()));
	pSock->setReadBufferSize(8192);
	m_lSockets.insert(pSock);

	QMetaObject::invokeMethod(this, "sheduleTransfer", Qt::QueuedConnection);
}
void CRateController::RemoveSocket(CNetworkConnection* pSock)
{
	disconnect(pSock, SIGNAL(readyToTransfer()), this, SLOT(sheduleTransfer()));
	pSock->setReadBufferSize(0);
	m_lSockets.remove(pSock);
}
void CRateController::sheduleTransfer()
{
	if(m_bTransferSheduled)
	{
		return;
	}

	m_bTransferSheduled = true;
	QTimer::singleShot(50, this, SLOT(transfer()));
}
void CRateController::transfer()
{
	m_bTransferSheduled = false;

	QMutexLocker l(m_pMutex);

	qint64 nMsecs = 1000;
	if(m_tStopWatch.isValid())
	{
		nMsecs = qMin(nMsecs, m_tStopWatch.elapsed());
	}

	qint64 nToRead = (m_nDownloadLimit * nMsecs) / 1000;
	qint64 nToWrite = (m_nUploadLimit * nMsecs) / 1000;

	if(nToRead <= 0 && nToWrite <= 0)
	{
		sheduleTransfer();
		return;
	}

	QSet<CNetworkConnection*> lSockets;
	for(QSet<CNetworkConnection*>::const_iterator itSocket = m_lSockets.constBegin(); itSocket != m_lSockets.constEnd(); ++itSocket)
	{
		CNetworkConnection* pSock = *itSocket;
		if(pSock->HasData())
		{
			lSockets.insert(pSock);
		}
	}

	if(lSockets.isEmpty())
	{
		return;
	}

	m_tStopWatch.start();

	bool bCanTransferMore = false;

	do
	{
		bCanTransferMore = false;
		qint64 nWriteChunk = qMax(qint64(1), nToWrite / lSockets.size());
		qint64 nReadChunk = qMax(qint64(1), nToRead / lSockets.size());

		quint32 nDownloaded = 0, nUploaded = 0;

		for(QSet<CNetworkConnection*>::iterator itSocket = lSockets.begin(); itSocket != lSockets.end() && (nToRead > 0 || nToWrite > 0);)
		{
			CNetworkConnection* pConn = *itSocket;

			bool bDataTransferred = false;
			qint64 nAvailable = qMin(nReadChunk, pConn->networkBytesAvailable());
			if(nAvailable > 0 && nToRead > 0)
			{
				qint64 nReadBytes = pConn->readFromNetwork(qMin(nAvailable, nToRead));
				if(nReadBytes > 0)
				{
					nToRead -= nReadBytes;
					nDownloaded += nReadBytes;
					bDataTransferred = true;
				}
			}

			if(m_nUploadLimit * 2 > pConn->m_pSocket->bytesToWrite())
			{
				qint64 nChunkSize = qMin(qMin(nWriteChunk, nToWrite), m_nUploadLimit * 2 - pConn->bytesToWrite());

				if(nChunkSize > 0)
				{
					qint64 nBytesWritten = pConn->writeToNetwork(nChunkSize);
					if(nBytesWritten > 0)
					{
						nToWrite -= nBytesWritten;
						nUploaded += nBytesWritten;
						bDataTransferred = true;
					}
				}
			}

			if(bDataTransferred && pConn->HasData())
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

		m_mDownload.Add(nDownloaded);
		m_mUpload.Add(nUploaded);
	}
	while(bCanTransferMore && (nToRead > 0 || nToWrite > 0) && !lSockets.isEmpty());

	if(bCanTransferMore || !lSockets.isEmpty())
	{
		sheduleTransfer();
	}
}
