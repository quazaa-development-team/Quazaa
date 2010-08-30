//
// types.h
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

#ifndef TYPES_H
#define TYPES_H

#include <time.h>

#ifdef __cplusplus

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QHostAddress>
#include <QtGlobal>
#include <QtEndian>
#include <QUuid>

#include "systemlog.h"

struct IPv4_ENDPOINT
{
    quint32 ip;
    quint16  port;

    IPv4_ENDPOINT(const quint32 addr = 0, const quint16 p = 0)
    {
        ip = addr;
        port = p;
	}

    IPv4_ENDPOINT(const QString& s)
    {
        QStringList l1 = s.split(":");
        if( l1.count() != 2 )
        {
            ip = 0;
            port = 0;
            return;
        }

        port = l1.at(1).toShort();

        QStringList l = l1.at(0).split(".");
        if( l.count() != 4 )
        {
            ip = 0;
            port = 0;
            return;
        }
        ip = 0;

        for( int i = 0; i < 4; i++)
        {
            uint b = l.at(i).toUInt();
            if( b > 255 )
            {
                ip = 0;
                port = 0;
                return;
            }
            ip <<= 8;
            ip += b;
        }
    }

    QString toString()
    {
        QString r;

        r.sprintf("%d.%d.%d.%d:%d", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF , port);

        return r;
    }
    QString toStringNoPort()
    {
        QString r;

        r.sprintf("%d.%d.%d.%d", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

        return r;
    }


    bool operator==(IPv4_ENDPOINT& rhs)
    {
        return (ip == rhs.ip);
    }
    bool operator!=(IPv4_ENDPOINT& rhs)
    {
        return (ip != rhs.ip);
    }
};

/*struct GUID
{
    quint8 m_pData[16];

    GUID()
    {
        memset(&m_pData[0], 0, 16);
    }
    GUID(GUID& other)
    {
        memcpy(&m_pData[0], &other.m_pData[0], 16);
    }

    bool isValid()
    {
        for( int i = 0; i < 16; i++ )
        {
            if( m_pData[i] != 0 )
                return true;
        }

        return false;
    }

    bool operator==(GUID& rhs)
    {
        return (memcmp(&m_pData[0], &rhs.m_pData[0], 16) == 0);
    }
    bool operator!=(GUID& rhs)
    {
        return (memcmp(&m_pData[0], &rhs.m_pData[0], 16) != 0);
    }
    QString toString()
    {

        return QString().sprintf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                  m_pData[0], m_pData[1], m_pData[2], m_pData[3], m_pData[4], m_pData[5], m_pData[6]
                  , m_pData[7], m_pData[8], m_pData[9], m_pData[10], m_pData[11], m_pData[12], m_pData[13], m_pData[14], m_pData[15]);
    }

    bool fromData(const char* pData, quint32 nLength)
    {
        if( nLength < 16 )
            return false;
        memcpy(&m_pData[0], pData, 16);
        return true;
    }
};*/

uint qHash(const QUuid& key);

enum G2NodeType {G2_UNKNOWN = 0, G2_LEAF = 1, G2_HUB = 2};

#endif
#endif // TYPES_H
