//
// Handshakes.cpp
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

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
