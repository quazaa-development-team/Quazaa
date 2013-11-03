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

CSystemLog systemLog;

CSystemLog::CSystemLog() :
m_pSection(QMutex::Recursive)
{
	m_pComponents = new QString[Components::NoComponents];
	m_bProcessingMessage = false;

	qRegisterMetaType<LogSeverity::Severity>( "LogSeverity::Severity" );
	qRegisterMetaType<Components::Component>( "Components::Component" );
}

CSystemLog::~CSystemLog()
{
	delete[] m_pComponents;
}

void CSystemLog::start()
{
	m_pComponents[Components::None]       = QString();
	m_pComponents[Components::Chat]       = tr( "[Chat] " );
	m_pComponents[Components::IRC]        = tr( "[IRC] " );
	m_pComponents[Components::Discovery]  = tr( "[Discovery] " );
	m_pComponents[Components::Network]    = tr( "[Network] " );
	m_pComponents[Components::Ares]       = tr( "[Ares] " );
	m_pComponents[Components::BitTorrent] = tr( "[BitTorrent] " );
	m_pComponents[Components::eD2k]       = tr( "[eD2k] " );
	m_pComponents[Components::G2]         = tr( "[G2] " );
	m_pComponents[Components::Security]   = tr( "[Security] " );
	m_pComponents[Components::Library]    = tr( "[Library] " );
	m_pComponents[Components::Downloads]  = tr( "[Downloads] " );
	m_pComponents[Components::Uploads]    = tr( "[Uploads] " );
	m_pComponents[Components::GUI]        = tr( "[GUI] " );
}

QString CSystemLog::msgFromComponent(Components::Component eComponent)
{
	return m_pComponents[eComponent];
}

void CSystemLog::postLog(const LogSeverity::Severity &severity, const QString &message)
{
	postLog( severity, Components::None, message );
}

void CSystemLog::postLog(const LogSeverity::Severity &severity, const Components::Component &component,
						 const QString &message)
{
	QMutexLocker locker(&m_pSection);

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
				postLog( lastSeverity, lastComponent,
						 tr( "Suppressed %n identical message(s).", 0, suppressed ) );
				bCheck = true;
			}
			lastMessage   = message;
			lastSeverity  = severity;
			lastComponent = component;
			suppressed = 0;
		}
	}

	const QString sComponentMessage = msgFromComponent( component ) + message;

	switch ( severity )
	{
		case LogSeverity::Debug:
		case LogSeverity::Warning:
		case LogSeverity::Critical:
		case LogSeverity::Error:
			qDebug() << qPrintable(sComponentMessage);
			break;
		default:
			break;
	}

	emit logPosted( sComponentMessage, severity );
}

void CSystemLog::postLog(const LogSeverity::Severity &severity, const Components::Component &component,
						 const char* format, ...)
{
	va_list argList;
	va_start( argList, format );
	QString message = QString().vsprintf( format, argList );
	postLog( severity, component, message );
	va_end( argList );
}

