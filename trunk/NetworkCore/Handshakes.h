#ifndef HANDSHAKES_H
#define HANDSHAKES_H

#include <QTcpServer>
#include <QSet>
#include "types.h"

class CHandshake;
class QThread;

class CHandshakes : public QTcpServer
{
    Q_OBJECT

protected:
    QSet<CHandshake*>   m_lHandshakes;
    quint32             m_nAccepted;
public:
    CHandshakes(QObject *parent = 0);
    ~CHandshakes();

    bool IsFirewalled()
    {
        return (m_nAccepted > 0);
    }

public slots:
    bool Listen();
    void Disconnect();
    void OnTimer(quint32 tNow = 0);
    void changeThread(QThread* target);

protected:
    void incomingConnection(int handle);
    void RemoveHandshake(CHandshake* pHs);

    friend class CHandshake;
};

extern CHandshakes Handshakes;

#endif // HANDSHAKES_H
