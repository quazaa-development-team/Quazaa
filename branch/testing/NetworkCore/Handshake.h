#ifndef HANDSHAKE_H
#define HANDSHAKE_H

#include <QObject>
class QTcpSocket;

class CHandshake:public QObject
{
    Q_OBJECT

protected:
    QTcpSocket* m_pSocket;
    quint32     m_tConnected;

public:
    CHandshake(QObject* parent = 0);
    void acceptFrom(int handle);
    ~CHandshake();

    void OnTimer(quint32 tNow);

public slots:
    void OnRead();

};

#endif // HANDSHAKE_H
