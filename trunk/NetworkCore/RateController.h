#ifndef RATECONTROLLER_H
#define RATECONTROLLER_H

#include <QtGlobal>
#include <QObject>
#include <QTime>
#include <QSet>

class CNetworkConnection;

class CRateController : public QObject
{
    Q_OBJECT
protected:
    qint64  m_nUploadLimit;
    qint64  m_nDownloadLimit;
    QTime   m_tMeterTimer;
    quint32 m_nUpload;
    quint32 m_nUploadAvg;
    quint32 m_nDownload;
    quint32 m_nDownloadAvg;
    QTime   m_tStopWatch;
    bool    m_bTransferSheduled;

    QSet<CNetworkConnection*>   m_lSockets;
public:
    CRateController(QObject* parent = 0);
    void AddSocket(CNetworkConnection* pSock);
    void RemoveSocket(CNetworkConnection* pSock);

    void UpdateStats();

    void SetDownloadLimit(qint32 nLimit)
    {
        m_nDownloadLimit = nLimit;
    }
    void SetUploadLimit(qint32 nLimit)
    {
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
        UpdateStats();
        return m_nDownloadAvg;
    }
    quint32 UploadSpeed()
    {
        UpdateStats();
        return m_nUploadAvg;
    }

public slots:
    void sheduleTransfer();
    void transfer();
};

#endif // RATECONTROLLER_H
