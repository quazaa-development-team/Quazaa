#include "webcache.h"
#include "hostcache.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>

CWebCache WebCache;

CWebCache::CWebCache()
{
	m_lCaches.append(CWebCacheHost(QUrl("http://gwc.marksieklucki.com/skulls.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://gweb2.4octets.co.uk/gwc.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://htmlhell.com/")));
	m_lCaches.append(CWebCacheHost(QUrl("http://dkac.trillinux.org/dkac/dkac.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://gwc.iblinx.com:2108/gwc/cgi-bin/fc")));
	m_lCaches.append(CWebCacheHost(QUrl("http://www.myfoxy.net/gwc/cgi-bin/fc")));
	m_lCaches.append(CWebCacheHost(QUrl("http://www.gofoxy.net/gwc/cgi-bin/fc")));
	m_lCaches.append(CWebCacheHost(QUrl("http://skulls.ce3c.be/skulls.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://gwc.lame.net/gwcii.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://dogma.cloud.bishopston.net:33559/")));
	m_lCaches.append(CWebCacheHost(QUrl("http://cache.leite.us/")));
	m_lCaches.append(CWebCacheHost(QUrl("http://gofoxy.net/gwc/cgi-bin/fc")));
	m_lCaches.append(CWebCacheHost(QUrl("http://gwebcache.ns1.net/")));
	m_lCaches.append(CWebCacheHost(QUrl("http://cache2.leite.us/")));
	m_lCaches.append(CWebCacheHost(QUrl("http://hub.dynoisp.com/skulls/skulls.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://cache2.bazookanetworks.com/g2/bazooka.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://midian.jayl.de/g2/bazooka.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://gwc2.wodi.org/skulls.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://cache.wru.pl/skulls.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://karma.cloud.bishopston.net:33559/")));
	m_lCaches.append(CWebCacheHost(QUrl("http://gwc.camppneus.com.br/skulls.php")));

    m_pRequest = 0;
    m_pReply = 0;
}
CWebCache::~CWebCache()
{
    qDebug("Destroying CWebCache");
    delete m_pRequest;
    m_pRequest = 0;
    CancelRequests();
}

void CWebCache::CancelRequests()
{
    qDebug("CancelRequests()");

    if( m_bRequesting )
    {
        m_pRequest->deleteLater();
        m_pRequest = 0;
        m_bRequesting = false;
    }
}

void CWebCache::RequestRandom()
{
    qDebug("RequestRandom()");

    quint32 nIndex = rand() % m_lCaches.size();

    if( m_lCaches[nIndex].CanQuery() )
    {
        CancelRequests();

        m_bRequesting = true;

        QUrl u = m_lCaches.at(nIndex).m_sUrl;
        u.addQueryItem("get", "1");
        u.addQueryItem("hostfile", "1");
        u.addQueryItem("net", "gnutella2");
        u.addQueryItem("client", "BROV1.0");

        qDebug("Querying " + u.toString().toAscii());

        m_lCaches[nIndex].m_tLastQuery = time(0);
        QNetworkRequest req(u);
        req.setRawHeader("User-Agent", "G2Core/0.1");

        m_pRequest = new QNetworkAccessManager();
        connect(m_pRequest, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnRequestComplete(QNetworkReply*)));
        m_pReply = m_pRequest->get(req);
    }
}

void CWebCache::OnRequestComplete(QNetworkReply* pReply)
{
    qDebug("OnRequestComplete()");

    if( pReply->error() == QNetworkReply::NoError )
    {
        qDebug("Parsing GWC response");

        QString ln;

        while( true )
        {
            ln = pReply->readLine(2048);

            if( ln.isEmpty() )
                break;

            qDebug() << "Line: " << ln;

            QStringList lp = ln.split("|");

            if( lp.size() >= 2 )
            {
                if( lp[0] == "H" || lp[0] == "h" )
                {
                    // host
                    HostCache.Add(IPv4_ENDPOINT(lp[1]), 0);
                }
            }
            else
            {
                qDebug() << "Parse error";
            }
        }

    }

    m_bRequesting = false;
    pReply->deleteLater();
    m_pRequest->deleteLater();;
    m_pRequest = 0;
    m_pReply = 0;
}
