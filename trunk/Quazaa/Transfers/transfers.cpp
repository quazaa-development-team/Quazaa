#include "transfers.h"
#include "ratecontroller.h"
#include "transfer.h"
#include "downloads.h"

#include <QMutexLocker>

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
