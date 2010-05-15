#include "systemlog.h"

SystemLog systemLog;

SystemLog::SystemLog()
{
}

void SystemLog::postLog(QString message, LogSeverity::Severity severity)
{
        emit logPosted(message, severity);
}
