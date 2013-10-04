/*
** systemlog.h
**
** Copyright © Quazaa Development Team, 2009-2013.
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

#ifndef SYSTEMLOG_H
#define SYSTEMLOG_H

#include <QObject>

namespace LogSeverity
{
enum Severity { Information, // Inform the user about something
	            Security,    // security related
	            Notice,      // Inform the user about something less important
	            Debug,       // Debugging output
	            Warning,
	            Error,
	            Critical };
}

namespace Components
{
enum Component { None         =  0,
                 Chat         =  1, // P2P chat
                 IRC          =  2,
                 Discovery    =  3,
                 Network      =  4, // global network stuff
                 Ares         =  5,
                 BitTorrent   =  6,
                 eD2k         =  7,
                 G2           =  8,
                 Security     =  9,
                 Library      = 10, // Share manager
                 Downloads    = 11,
                 Uploads      = 12,
                 GUI          = 13,
	             SignalQueue  = 14,
	             HostCache    = 15,
                 NoComponents = 16 };
}

class SystemLog : public QObject
{
	Q_OBJECT
private:
	QString* m_pComponents;

public:
	SystemLog();
	~SystemLog();

	void start();

	QString msgFromComponent(Components::Component eComponent);

signals:
	void logPosted(QString message, LogSeverity::Severity severity);

public slots:
	void postLog(LogSeverity::Severity severity, QString message,
	             Components::Component component = Components::None);

public:
	void postLog(LogSeverity::Severity severity, Components::Component component,
	             const char* format, ...);
};

extern SystemLog systemLog;

#endif // SYSTEMLOG_H
