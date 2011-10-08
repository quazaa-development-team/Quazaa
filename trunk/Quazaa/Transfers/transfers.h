#ifndef TRANSFERS_H
#define TRANSFERS_H

#include "types.h"
#include <QObject>
#include <QMutex>
#include <QMultiHash>

#include "thread.h"

class CTransfer;
class CRateController;

class CTransfers : public QObject
{
	Q_OBJECT
public:
	QMutex m_pSection;
	bool   m_bActive;
	CRateController* m_pController;

protected:
	QMultiHash<void*, CTransfer*> m_lTransfers;

public:
	CTransfers(QObject* parent = 0);
	void start();
	void stop();

signals:

public slots:
	void startTransfer(void* pOwner, CTransfer* pTransfer);
	void stopTransfer(void* pOwner, CTransfer* pTransfer);
	void removeAllTransfersByOwner(void* pOwner);
};

extern CTransfers Transfers;
extern CThread TransfersThread;
#endif // TRANSFERS_H
