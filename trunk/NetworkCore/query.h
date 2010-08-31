//
// Query.h
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

#ifndef QUERY_H
#define QUERY_H

#include <QList>
#include <QString>
#include "types.h"

class G2Packet;

class CQuery
{
	QList<QString>  m_lURNs;
    QString         m_sMetadata;
    quint64         m_nMinimumSize;
    quint64         m_nMaximumSize;
	QString         m_sDescriptiveName;

    QUuid           m_oGUID;

    G2Packet*       m_pCachedPacket;
public:
	CQuery();
	QString DescriptiveName() { return m_sDescriptiveName; }

    void SetGUID(QUuid& guid);

    void AddURN(const char* pURN, quint32 nLength);
    void SetDescriptiveName(QString sDN);
    void SetSizeRestriction(quint64 nMin, quint64 nMax);
    void SetMetadata(QString sMeta);

    G2Packet* ToG2Packet(IPv4_ENDPOINT* pAddr = 0, quint32 nKey = 0);
};

#endif // QUERY_H
