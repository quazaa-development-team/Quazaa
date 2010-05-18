#include "systemlog.h"
#include <QMetaType>

SystemLog systemLog;

SystemLog::SystemLog()
{
	qRegisterMetaType<LogSeverity::Severity>("LogSeverity::Severity");
}

void SystemLog::postLog(QString message, LogSeverity::Severity severity)
{
	emit logPosted(message, severity);
}
