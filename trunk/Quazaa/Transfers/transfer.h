#ifndef TRANSFER_H
#define TRANSFER_H

#include "networkconnection.h"

class CTransfer : public CNetworkConnection
{
    Q_OBJECT
public:
	CTransfer(QObject *parent = 0);

signals:

public slots:

};

#endif // TRANSFER_H
