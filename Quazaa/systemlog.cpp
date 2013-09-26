/*
** $Id$
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

#include "systemlog.h"
#include <QMetaType>
#include <QtCore>

#include "debug_new.h"

SystemLog systemLog;

SystemLog::SystemLog()
{
	m_pComponents = new QString[Components::NoComponents];

	qRegisterMetaType<LogSeverity::Severity>( "LogSeverity::Severity" );
	qRegisterMetaType<Components::Component>( "Components::Component" );
}

SystemLog::~SystemLog()
{
	delete[] m_pComponents;
}

void SystemLog::start()
{
	m_pComponents[Components::None]       = QString();
	m_pComponents[Components::Chat]       = tr( "[Chat] "       );
	m_pComponents[Components::IRC]        = tr( "[IRC] "        );
	m_pComponents[Components::Discovery]  = tr( "[Discovery] "  );
	m_pComponents[Components::Network]    = tr( "[Network] "    );
	m_pComponents[Components::Ares]       = tr( "[Ares] "       );
	m_pComponents[Components::BitTorrent] = tr( "[BitTorrent] " );
	m_pComponents[Components::eD2k]       = tr( "[eD2k] "       );
	m_pComponents[Components::G2]         = tr( "[G2] "         );
	m_pComponents[Components::Security]   = tr( "[Sacurity] "   );
	m_pComponents[Components::Library]    = tr( "[Library] "    );
	m_pComponents[Components::Downloads]  = tr( "[Downloads] "  );
	m_pComponents[Components::Uploads]    = tr( "[Uploads] "    );
	m_pComponents[Components::GUI]        = tr( "[GUI] "        );
}

QString SystemLog::msgFromComponent(Components::Component eComponent)
{
	return m_pComponents[eComponent];
}

void SystemLog::postLog(LogSeverity::Severity severity, QString message,
                        Components::Component component)
{
	static LogSeverity::Severity lastSeverity  = LogSeverity::Information;
	static Components::Component lastComponent = Components::None;
	static QString lastMessage;
	static int suppressed = 0;
	static bool bCheck = true;

	if ( bCheck )
	{
		if ( severity == lastSeverity && component == lastComponent && message == lastMessage )
		{
			++suppressed;
			return;
		}
		else
		{
			if ( suppressed > 0 )
			{
				bCheck = false;
				postLog( lastSeverity,
				         tr( "Suppressed %n identical message(s).", 0, suppressed ),
				         lastComponent );
				bCheck = true;
			}
			lastMessage   = message;
			lastSeverity  = severity;
			lastComponent = component;
			suppressed = 0;
		}
	}

	message = msgFromComponent( component ) + message;

	switch ( severity )
	{
		case LogSeverity::Debug:
		case LogSeverity::Warning:
		case LogSeverity::Critical:
		case LogSeverity::Error:
			qCritical() << qPrintable(message);
			break;
		default:
			break;
	}

	emit logPosted( message, severity );
}

void SystemLog::postLog(LogSeverity::Severity severity, Components::Component component,
                        const char* format, ...)
{
	va_list argList;
	va_start( argList, format );
	QString message = QString().vsprintf( format, argList );
	postLog( severity, message, component );
	va_end( argList );
}

