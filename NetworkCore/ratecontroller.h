//
// ratecontroller.h
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

#ifndef RATECONTROLLER_H
#define RATECONTROLLER_H

#include <QtGlobal>
#include <QObject>
#include <QElapsedTimer>
#include <QSet>
#include <QMutex>

#include "networkconnection.h"

class CRateController : public QObject
{
    Q_OBJECT
protected:
    qint64  m_nUploadLimit;
    qint64  m_nDownloadLimit;
	bool    m_bTransferSheduled;
	QMutex*	m_pMutex;

	QElapsedTimer   m_tStopWatch;

    QSet<CNetworkConnection*>   m_lSockets;

public:
	TCPBandwidthMeter	m_mDownload;
	TCPBandwidthMeter	m_mUpload;

public:
	CRateController(QMutex* pMutex, QObject* parent = 0);
    void AddSocket(CNetworkConnection* pSock);
    void RemoveSocket(CNetworkConnection* pSock);

    void SetDownloadLimit(qint32 nLimit)
    {
		qDebug() << "New download limit: " << nLimit;
        m_nDownloadLimit = nLimit;
    }
    void SetUploadLimit(qint32 nLimit)
    {
		qDebug() << "New upload limit: " << nLimit;
        m_nUploadLimit = nLimit;
    }
    qint32 UploadLimit() const
    {
        return m_nUploadLimit;
    }
    qint32 DownloadLimit() const
    {
        return m_nDownloadLimit;
    }

    quint32 DownloadSpeed()
    {
		return m_mDownload.AvgUsage();
    }
    quint32 UploadSpeed()
    {
		return m_mUpload.AvgUsage();
    }

public slots:
    void sheduleTransfer();
    void transfer();
};

#endif // RATECONTROLLER_H
