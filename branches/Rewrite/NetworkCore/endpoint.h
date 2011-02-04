//
// endpoint.h
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

#ifndef ENDPOINT_H
#define RNDPOINT_H

#include <QHostAddress>

class CEndPoint : public QHostAddress
{
protected:
	quint16	m_nPort;

public:
	CEndPoint();
	explicit CEndPoint(quint32 ip4Addr, quint16 nPort = 0);
	explicit CEndPoint(quint8 *ip6Addr, quint16 nPort = 0);
	explicit CEndPoint(const Q_IPV6ADDR &ip6Addr, quint16 nPort = 0);
	explicit CEndPoint(const sockaddr *sockaddr, quint16 nPort = 0);
	explicit CEndPoint(const QString &address, quint16 nPort);
	explicit CEndPoint(const QString &address);
	explicit CEndPoint(const QHostAddress &address, quint16 nPort);
	CEndPoint(const CEndPoint &copy);
	CEndPoint(SpecialAddress address, quint16 nPort = 0);

public:
	void clear();
	QString toStringWithPort() const;
	quint16 port() const;
	void setPort(const quint16 nPort);
};

#endif // ENDPOINT_H
