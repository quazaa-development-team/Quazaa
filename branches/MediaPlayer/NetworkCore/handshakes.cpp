/*
** handshakes.cpp
**
** Copyright © Quazaa Development Team, 2009-2011.
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

#include "handshakes.h"
#include "network.h"
#include "handshake.h"
#include "ratecontroller.h"
#include "neighbours.h"

#include <QTimer>

CHandshakes Handshakes;
CThread HandshakesThread;

CHandshakes::CHandshakes(QObject* parent) :
	QTcpServer(parent)
{
	m_nAccepted = 0;
	m_bActive = false;
}

CHandshakes::~CHandshakes()
{
	if(m_bActive)
	{
		Disconnect();
	}
}

void CHandshakes::Listen()
{
	QMutexLocker l(&m_pSection);

	if(m_bActive)
	{
		return;
	}

	m_nAccepted = 0;
	m_bActive = true;

	HandshakesThread.start("Handshakes", &m_pSection, this);
}
void CHandshakes::Disconnect()
{
	QMutexLocker l(&m_pSection);

	m_bActive = false;

	qDeleteAll(m_lHandshakes);
	m_lHandshakes.clear();

	HandshakesThread.exit(0);
}

void CHandshakes::incomingConnection(int handle)
{
	QMutexLocker l(&m_pSection);

	CHandshake* pNew = new CHandshake();
	m_lHandshakes.insert(pNew);
	m_pController->AddSocket(pNew);
	pNew->AcceptFrom(handle);
	pNew->moveToThread(&HandshakesThread);
	m_nAccepted++;
}

void CHandshakes::OnTimer()
{
	QMutexLocker l(&m_pSection);

	quint32 tNow = time(0);

	foreach(CHandshake * pHs, m_lHandshakes)
	{
		pHs->OnTimer(tNow);
	}
}
void CHandshakes::RemoveHandshake(CHandshake* pHs)
{
	ASSUME_LOCK(Handshakes.m_pSection);

	m_lHandshakes.remove(pHs);
	if(m_pController)
	{
		m_pController->RemoveSocket(pHs);
	}
}

void CHandshakes::processNeighbour(CHandshake *pHs)
{
	RemoveHandshake(pHs);
	Neighbours.OnAccept(pHs);
}

void CHandshakes::SetupThread()
{
	m_pController = new CRateController(&m_pSection);

	m_pController->moveToThread(&HandshakesThread); // should not be necesarry

	m_pController->SetDownloadLimit(4096);
	m_pController->SetUploadLimit(4096);

	bool bOK = QTcpServer::listen(QHostAddress::Any, Network.GetLocalAddress().port());

	if(bOK)
	{
		systemLog.postLog(LogSeverity::Notice, "Handshakes: listening on port %d.", Network.GetLocalAddress().port());
	}
	else
	{
		systemLog.postLog(LogSeverity::Error, "Handshakes: cannot listen on port %d, incoming connections will be unavailable.", Network.GetLocalAddress().port());
	}

	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimer()));
	m_pTimer->start(1000);
}
void CHandshakes::CleanupThread()
{
	if(isListening())
	{
		close();
		foreach(CHandshake * pHs, m_lHandshakes)
		{
			m_pController->RemoveSocket(pHs);
			pHs->deleteLater();
		}

		delete m_pController;
		delete m_pTimer;
	}
}
