#ifndef WEBCACHE_H
#define WEBCACHE_H

#include <QList>
#include <QUrl>
#include <QNetworkAccessManager>

#include <time.h>

class QNetworkReply;

// Przeniesc w lepsze miejsce
const quint32 RequeryTime = 3600;

class CWebCacheHost
{
public:
    QUrl    m_sUrl;
    quint32 m_tLastQuery;

    CWebCacheHost(QUrl url)
    {
        m_sUrl = url;
        m_tLastQuery = 0;
    }
    CWebCacheHost(QUrl url, quint32 ts)
    {
        m_sUrl = url;
        m_tLastQuery = ts;
    }

    inline bool CanQuery()
    {
        if( time(0) - m_tLastQuery > RequeryTime )
            return true;

        return false;
    }
};

class CWebCache:public QObject
{
    Q_OBJECT

protected:
    QList<CWebCacheHost>    m_lCaches;
    bool                    m_bRequesting;
    QNetworkAccessManager*  m_pRequest;
    QNetworkReply*          m_pReply;
public:
    CWebCache();
    ~CWebCache();
    void RequestRandom();
    void CancelRequests();

    inline bool isRequesting()
    {
        return m_bRequesting;
    }

public slots:
    void OnRequestComplete(QNetworkReply* pReply);
};

extern CWebCache WebCache;
#endif // WEBCACHE_H
