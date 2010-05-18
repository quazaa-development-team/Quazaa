#ifndef SYSTEMLOG_H
#define SYSTEMLOG_H

#include <QObject>

namespace LogSeverity {
        enum Severity { Information, Security, Notice, Debug, Warning, Error, Critical };
}

class SystemLog : public QObject
{
    Q_OBJECT
public:
	SystemLog();

signals:
        void logPosted(QString message, LogSeverity::Severity severity);

public slots:
        void postLog(QString message, LogSeverity::Severity severity = LogSeverity::Information);
};

extern SystemLog systemLog;

#endif // SYSTEMLOG_H
