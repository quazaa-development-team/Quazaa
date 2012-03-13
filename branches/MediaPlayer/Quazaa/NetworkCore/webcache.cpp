/*
** $Id: webcache.cpp 938 2012-02-23 05:08:24Z smokexyz $
**
** Copyright © Quazaa Development Team, 2009-2012.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public 
** License version 3.0 requirements will be met: 
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version 
** 3.0 along with Quazaa; if not, write to the Free Software Foundation, 
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "webcache.h"
#include "hostcache.h"
#include "systemlog.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>

#ifdef _DEBUG
#include "debug_new.h"
#endif

CWebCache WebCache;

CWebCache::CWebCache()
{
	m_lCaches.append(CWebCacheHost(QUrl("http://gweb2.4octets.co.uk/gwc.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://dkac.trillinux.org/dkac/dkac.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://skulls.ce3c.be/skulls.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://gwc.lame.net/gwcii.php")));
	m_lCaches.append(CWebCacheHost(QUrl("http://cache.leite.us/")));
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
	CancelRequests();
	delete m_pRequest;
	m_pRequest = 0;
}

void CWebCache::CancelRequests()
{
	qDebug("CancelRequests()");

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

	quint32 nIndex = qrand() % m_lCaches.size();

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
		QMutexLocker l(&HostCache.m_pSection);

		systemLog.postLog(LogSeverity::Debug, QString("Parsing GWC response"));
		//qDebug("Parsing GWC response");

		QString ln;

		QDateTime tNow = QDateTime::currentDateTimeUtc();

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
					HostCache.Add(CEndPoint(lp[1]), tNow);
				}
			}
			else
			{
				systemLog.postLog(LogSeverity::Debug, QString("Parse error"));
				//qDebug() << "Parse error";
			}
		}

		HostCache.Save();
	}

	m_bRequesting = false;
	pReply->deleteLater();
	m_pRequest->deleteLater();;
	m_pRequest = 0;
	m_pReply = 0;
}

