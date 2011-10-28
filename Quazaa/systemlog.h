/*
** systemlog.h
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

#ifndef SYSTEMLOG_H
#define SYSTEMLOG_H

#include <QObject>

namespace LogSeverity
{
	enum Severity { Information, Security, Notice, Debug, Warning, Error, Critical };
}

namespace LogCategory
{
	enum Category { General, Network, Chat, MediaPlayer, Library };
}

class SystemLog : public QObject
{
	Q_OBJECT
public:
	SystemLog();

signals:
	void logPosted(QString message, LogCategory::Category category, LogSeverity::Severity severity);

public slots:
	void postLog(LogCategory::Category category, LogSeverity::Severity severity, QString message);
public:
	void postLog(LogCategory::Category category, LogSeverity::Severity severity, const char* format, ...);
};

extern SystemLog systemLog;

#endif // SYSTEMLOG_H
