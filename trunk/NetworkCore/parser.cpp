#include "parser.h"

#include <QString>
#include <QList>

namespace Parser
{
    QString GetHeaderValue(QString & headers, QString headerName)
    {
        qint32 nStart;
        qint32 nEnd;
        qint32 nColon;

        headerName += ":";

        nStart = headers.indexOf(headerName, 0, Qt::CaseInsensitive);

        if( nStart < 0 )
            return QString();

        nEnd = headers.indexOf("\r\n", nStart);

        if( nEnd < 0 )
            return QString();

        nColon = headers.indexOf(":", nStart);

        if( nColon < 0 || nColon > nEnd )
            return QString();

        return headers.mid(nColon + 1, nEnd - nColon).trimmed();
    }
};
