#include "Handshakes.h"
#include "network.h"
#include "Handshake.h"
#include <QThread>

CHandshakes Handshakes;

CHandshakes::CHandshakes(QObject *parent) :
    QTcpServer(parent)
{
    m_nAccepted = 0;
}

CHandshakes::~CHandshakes()
{
    Disconnect();
}

bool CHandshakes::Listen()
{
    if( isListening() )
        return true;

    m_nAccepted = 0;
    return QTcpServer::listen(QHostAddress::Any, Network.GetLocalAddress().port);
}
void CHandshakes::Disconnect()
{
    if( isListening( ))
    {
        close();
        foreach( CHandshake* pHs, m_lHandshakes )
        {
            pHs->deleteLater();
        }
    }
}

void CHandshakes::incomingConnection(int handle)
{
    CHandshake* pNew = new CHandshake();
    pNew->acceptFrom(handle);
    m_lHandshakes.insert(pNew);
    m_nAccepted++;
}

void CHandshakes::OnTimer(quint32 tNow)
{
    if( tNow == 0 )
        tNow = time(0);

    foreach(CHandshake* pHs, m_lHandshakes)
    {
        pHs->OnTimer(tNow);
    }
}
void CHandshakes::RemoveHandshake(CHandshake *pHs)
{
    m_lHandshakes.remove(pHs);
}
void CHandshakes::changeThread(QThread* target)
{
    qDebug() << "Handshakes change thread\n current:" << QThread::currentThread() << "new " << target;
    moveToThread(target);
}
