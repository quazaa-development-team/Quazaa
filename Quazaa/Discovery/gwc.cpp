/*
** gwc.cpp
**
** Copyright © Quazaa Development Team, 2012.
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

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>

#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

#include "gwc.h"

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

		// if there is a request, there must be a manager
		Q_ASSERT( m_pNAMgr );

		m_pNAMgr.clear();
	}
}


void CGWC::doQuery() throw()
{
	QUrl oURL = m_oServiceURL;

	// build query
#if QT_VERSION >= 0x050000
	{
		QUrlQuery query;
		query.addQueryItem("get", "1");
		query.addQueryItem("hostfile", "1");
		query.addQueryItem("net", "gnutella2");
		query.addQueryItem("client", "BROV1.0");
		oURL.setQuery(query);
	}
#else
		u.addQueryItem("get", "1");
		u.addQueryItem("hostfile", "1");
		u.addQueryItem("net", "gnutella2");
		u.addQueryItem("client", "BROV1.0");
#endif

	// inform user
	systemLog.postLog( LogSeverity::Debug,
					   discoveryManager.m_sMessage
					   + QString( "Querying GWC: %1" ).arg( oURL.toString() ) );

	setLastQueried( time( nullptr ) );

	// generate request
	m_pRequest = new QNetworkRequest( oURL );
	m_pRequest->setRawHeader( "User-Agent", "G2Core/0.1" );

	// obtain network access manager for query
	m_pNAMgr = discoveryManager.requestNAM();


#if QT_VERSION >= 0x050000
	connect( m_pNAMgr.data(), &QNetworkAccessManager::finished, this, &CGWC::queryRequestCompleted );
#else
	connect( m_pNAMgr.data(), SIGNAL(finished(QNetworkReply*)), this, SLOT(queryRequestCompleted(QNetworkReply*)) );
#endif

	// do query
	m_pNAMgr->get( *m_pRequest );
}

void CGWC::doUpdate() throw()
{
	// Network.GetLocalAddress()

	// TODO: Implement.
}


void CGWC::doCancelRequest() throw()
{
#if QT_VERSION >= 0x050000
	disconnect( m_pNAMgr.data(), &QNetworkAccessManager::finished, this, &CGWC::queryRequestCompleted );
#else
	disconnect( m_pNAMgr.data(), SIGNAL(finished(QNetworkReply*)), this, SLOT(queryRequestCompleted(QNetworkReply*)) );
#endif

	delete m_pRequest;
	m_pRequest = nullptr;
	m_pNAMgr.clear();     // we don't need the network access manager anymore

	// TODO: Check locking
	resetRunning();
}

void CGWC::queryRequestCompleted(QNetworkReply* pReply)
{
	QWriteLocker l( &m_oRWLock );

	if ( pReply->request() != *m_pRequest )
	{
		return; // reply was meant for sb else
	}

	systemLog.postLog( LogSeverity::Debug, QString( "OnRequestComplete()" ) );

	// used for statistics update at a later time
	quint16 nHosts = 0;

	if ( pReply->error() == QNetworkReply::NoError )
	{
		systemLog.postLog( LogSeverity::Debug, QString( "Parsing GWC response" ) );

		// get first piece of data from reply
		QString ln = pReply->readLine(2048);

		QMutexLocker l( &HostCache.m_pSection );

		// parse data
		while ( !ln.isEmpty() )
		{
			systemLog.postLog( LogSeverity::Debug, QString( "Line: %1" ).arg( ln ) );

			QStringList lp = ln.split("|");
			if ( lp.size() > 1 )
			{
				if ( lp[0] == "H" || lp[0] == "h" )
				{
					// found host
					HostCache.Add( CEndPoint( lp[1] ) );
					++nHosts;
				}
			}
			else
			{
				systemLog.postLog( LogSeverity::Debug, QString( "Parsing error!" ) );
			}

			// get next piece of data
			ln = pReply->readLine(2048);
		}
	}
	else
	{
		systemLog.postLog( LogSeverity::Error, tr( "Error querying GWC: " ) + url() );
	}

	// make sure all statistics and failure counters are updated
	updateStatistics( true, nHosts );

	// clean up
	pReply->deleteLater();
	delete m_pRequest;
	m_pRequest = nullptr;
	m_pNAMgr.clear();

	resetRunning();
}

void CGWC::updateRequestCompleted(QNetworkReply* /*pReply*/)
{
	QWriteLocker l( &m_oRWLock );

	// TODO: Implement.
	// updateStatistics( false, bSuccess );
}
