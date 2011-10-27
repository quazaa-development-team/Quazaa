/*
** $Id$
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

#include "transfers.h"
#include "ratecontroller.h"
#include "transfer.h"
#include "downloads.h"

#include <QMutexLocker>

#ifdef _DEBUG
#include "debug_new.h"
#endif

CTransfers Transfers;
CThread TransfersThread;

CTransfers::CTransfers(QObject* parent)
	: QObject(parent),
	  m_bActive(false)
{
	m_pController = new CRateController(&m_pSection, this);
}

void CTransfers::start()
{
	if( m_bActive )
		return;

	systemLog.postLog(LogSeverity::Notice, qPrintable(tr("Starting transfers...")));

	m_bActive = true;
	TransfersThread.start("Transfers", &m_pSection);
	m_pController->moveToThread(&TransfersThread);
	Downloads.start();
	Downloads.moveToThread(&TransfersThread);
}

void CTransfers::stop()
{
	if( !m_bActive )
		return;

	m_bActive = false;
	Downloads.stop();
}

void CTransfers::startTransfer(void *pOwner, CTransfer *pTransfer)
{
	QMutexLocker l(&m_pSection);

	if(!m_bActive)
	{
		qDebug() << "Not starting new transfer because CTransfers is not active";
		return;
	}

	QList<CTransfer*> oList = m_lTransfers.values(pOwner);
	if( oList.contains(pTransfer) )
	{
		qDebug() << "Not starting new transfer because it's already started";
		return;
	}

	m_lTransfers.insert(pOwner, pTransfer);

	// start();
}

void CTransfers::stopTransfer(void *pOwner, CTransfer *pTransfer)
{
}

void CTransfers::removeAllTransfersByOwner(void *pOwner)
{
}

