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


#include "qtsingleapplication.h"
#include "qtlocalpeer.h"
#include <QtGui/QWidget>
#if defined(_MSC_VER) && defined(_DEBUG)
	#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
	#define new DEBUG_NEW
#endif
void QtSingleApplication::sysInit(const QString &appId)
{
    actWin = 0;
    peer = new QtLocalPeer(this, appId);
    connect(peer, SIGNAL(messageReceived(const QString&)), SIGNAL(messageReceived(const QString&)));
}
QtSingleApplication::QtSingleApplication(int &argc, char **argv, bool GUIenabled)
    : QApplication(argc, argv, GUIenabled)
{
    sysInit();
}
QtSingleApplication::QtSingleApplication(const QString &appId, int &argc, char **argv)
    : QApplication(argc, argv)
{
    sysInit(appId);
}
QtSingleApplication::QtSingleApplication(int &argc, char **argv, Type type)
    : QApplication(argc, argv, type)
{
    sysInit();
}
#if defined(Q_WS_X11)
QtSingleApplication::QtSingleApplication(Display* dpy, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, visual, cmap)
{
    sysInit();
}
QtSingleApplication::QtSingleApplication(Display *dpy, int &argc, char **argv, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, argc, argv, visual, cmap)
{
    sysInit();
}
QtSingleApplication::QtSingleApplication(Display* dpy, const QString &appId, int argc, char **argv, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, argc, argv, visual, cmap)
{
    sysInit(appId);
}
#endif
bool QtSingleApplication::isRunning()
{
    return peer->isClient();
}
bool QtSingleApplication::sendMessage(const QString &message, int timeout)
{
    return peer->sendMessage(message, timeout);
}
QString QtSingleApplication::id() const
{
    return peer->applicationId();
}
void QtSingleApplication::setActivationWindow(QWidget* aw, bool activateOnMessage)
{
    actWin = aw;
    if (activateOnMessage)
        connect(peer, SIGNAL(messageReceived(const QString&)), this, SLOT(activateWindow()));
    else
        disconnect(peer, SIGNAL(messageReceived(const QString&)), this, SLOT(activateWindow()));
}
QWidget* QtSingleApplication::activationWindow() const
{
    return actWin;
}
void QtSingleApplication::activateWindow()
{
    if (actWin) {
        actWin->setWindowState(actWin->windowState() & ~Qt::WindowMinimized);
        actWin->raise();
        actWin->activateWindow();
    }
}

