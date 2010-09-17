//
// parser.cpp
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
