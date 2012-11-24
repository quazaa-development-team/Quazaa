#include "gwc.h"

#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>

using namespace Discovery;

CGWC::CGWC(const QUrl& oURL, const CNetworkType& oNType, quint8 nRating) :
	CDiscoveryService( oURL, oNType, nRating )
{
	m_nServiceType = stGWC;
}

CGWC::~CGWC()
{
	if ( m_pRequest )
	{
		delete m_pRequest;
		m_pRequest = nullptr;
	}
}


void CGWC::doQuery() throw()
{
	// TODO: Implement.
//	CancelRequests();

	m_bRunning = true;

	QUrl u = m_oServiceURL;
#if QT_VERSION >= 0x050000
	{
		QUrlQuery query;
		query.addQueryItem("get", "1");
		query.addQueryItem("hostfile", "1");
		query.addQueryItem("net", "gnutella2");
		query.addQueryItem("client", "BROV1.0");
		u.setQuery(query);
	}
#else
		u.addQueryItem("get", "1");
		u.addQueryItem("hostfile", "1");
		u.addQueryItem("net", "gnutella2");
		u.addQueryItem("client", "BROV1.0");
#endif

	systemLog.postLog( LogSeverity::Debug, QString( "Querying %1" ).arg( u.toString() ) );

	m_tLastQueried = time( nullptr );

	QNetworkRequest req( u );
	req.setRawHeader( "User-Agent", "G2Core/0.1" );

	m_pRequest = new QNetworkAccessManager();
	connect( m_pRequest, SIGNAL(finished(QNetworkReply*)), this, SLOT(queryRequestCompleted(QNetworkReply*)) );
	m_pRequest->get(req);
}

void CGWC::doUpdate() throw()
{
	 //Network.GetLocalAddress()

	// TODO: Implement.
}

void CGWC::queryRequestCompleted(QNetworkReply* pReply)
{
	// TODO: refine

	systemLog.postLog( LogSeverity::Debug, QString("OnRequestComplete()") );
	//qDebug("OnRequestComplete()");

	if ( pReply->error() == QNetworkReply::NoError )
	{
		QMutexLocker l(&HostCache.m_pSection);

		systemLog.postLog(LogSeverity::Debug, QString("Parsing GWC response"));

		QString ln = pReply->readLine(2048);
		QDateTime tNow = QDateTime::currentDateTimeUtc();

		while ( !ln.isEmpty() )
		{
			systemLog.postLog(LogSeverity::Debug, QString("Line: %1").arg(ln));
			qDebug() << "Line: " << ln;

			QStringList lp = ln.split("|");

			if ( lp.size() >= 2 )
			{
				if ( lp[0] == "H" || lp[0] == "h" )
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

			ln = pReply->readLine(2048);
		}

		HostCache.Save();
	}
	else
	{
		//TODO: handle error
	}

	QWriteLocker l( &m_oRWLock );

	m_bRunning = false;
	pReply->deleteLater();
	m_pRequest->deleteLater();;
	m_pRequest = 0;
}

void CGWC::updateRequestCompleted(QNetworkReply* /*pReply*/)
{
	// TODO: Implement.
}
