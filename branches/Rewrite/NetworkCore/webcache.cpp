//
// webcache.cpp
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

#include "webcache.h"
#include "hostcache.h"
#include "systemlog.h"

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
	systemLog.postLog(LogSeverity::Debug, QString("Destroying CWebCache"));
	//qDebug("Destroying CWebCache");
	delete m_pRequest;
	m_pRequest = 0;
	CancelRequests();
}

void CWebCache::CancelRequests()
{
	systemLog.postLog(LogSeverity::Debug, QString("CancelRequests()"));
	//qDebug("CancelRequests()");

	if(m_bRequesting)
	{
		m_pRequest->deleteLater();
		m_pRequest = 0;
		m_bRequesting = false;
	}
}

void CWebCache::RequestRandom()
{
	systemLog.postLog(LogSeverity::Debug, QString("RequestRandom()"));
	qDebug("RequestRandom()");

	quint32 nIndex = rand() % m_lCaches.size();

	if(m_lCaches[nIndex].CanQuery())
	{
		CancelRequests();

		m_bRequesting = true;

		QUrl u = m_lCaches.at(nIndex).m_sUrl;
		u.addQueryItem("get", "1");
		u.addQueryItem("hostfile", "1");
		u.addQueryItem("net", "gnutella2");
		u.addQueryItem("client", "BROV1.0");

		systemLog.postLog(LogSeverity::Debug, QString("Querying %1").arg(u.toString()));
		//qDebug("Querying " + u.toString().toAscii());

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
	systemLog.postLog(LogSeverity::Debug, QString("OnRequestComplete()"));
	//qDebug("OnRequestComplete()");

	if(pReply->error() == QNetworkReply::NoError)
	{
		systemLog.postLog(LogSeverity::Debug, QString("Parsing GWC response"));
		//qDebug("Parsing GWC response");

		QString ln;

		while(true)
		{
			ln = pReply->readLine(2048);

			if(ln.isEmpty())
			{
				break;
			}

			systemLog.postLog(LogSeverity::Debug, QString("Line: %1").arg(ln));
			qDebug() << "Line: " << ln;

			QStringList lp = ln.split("|");

			if(lp.size() >= 2)
			{
				if(lp[0] == "H" || lp[0] == "h")
				{
					// host
					HostCache.Add(CEndPoint(lp[1]), 0);
				}
			}
			else
			{
				systemLog.postLog(LogSeverity::Debug, QString("Parse error"));
				//qDebug() << "Parse error";
			}
		}

	}

	m_bRequesting = false;
	pReply->deleteLater();
	m_pRequest->deleteLater();;
	m_pRequest = 0;
	m_pReply = 0;
}
