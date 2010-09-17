//
// handshakes.h
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

#ifndef HANDSHAKES_H
#define HANDSHAKES_H

#include <QTcpServer>
#include <QSet>
#include "types.h"
#include "thread.h"

class CHandshake;
class CRateController;
class QTimer;

class CHandshakes : public QTcpServer
{
    Q_OBJECT

protected:
    QSet<CHandshake*>   m_lHandshakes;
    quint32             m_nAccepted;
	CRateController*	m_pController;
	QTimer*				m_pTimer;
	bool				m_bActive;

public:
	QMutex	m_pSection;

public:
    CHandshakes(QObject *parent = 0);
    ~CHandshakes();

    bool IsFirewalled()
    {
		return (m_nAccepted == 0);
    }

public slots:
	void Listen();
    void Disconnect();
	void OnTimer();

protected slots:
	void SetupThread();
	void CleanupThread();

protected:
    void incomingConnection(int handle);
    void RemoveHandshake(CHandshake* pHs);

    friend class CHandshake;
};

extern CHandshakes Handshakes;
extern CThread HandshakesThread;

#endif // HANDSHAKES_H
