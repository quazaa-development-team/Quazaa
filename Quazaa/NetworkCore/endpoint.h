/*
** endpoint.h
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

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <QHostAddress>

class CEndPoint : public QHostAddress
{
protected:
	quint16	m_nPort;
	mutable QString m_sCountry;

public:
	CEndPoint();
	explicit CEndPoint(quint32 ip4Addr, quint16 nPort = 0);
	explicit CEndPoint(quint8* ip6Addr, quint16 nPort = 0);
	explicit CEndPoint(const Q_IPV6ADDR& ip6Addr, quint16 nPort = 0);
	explicit CEndPoint(const sockaddr* sockaddr, quint16 nPort = 0);
	explicit CEndPoint(const QString& address, quint16 nPort);
	explicit CEndPoint(const QString& address);
	explicit CEndPoint(const QHostAddress& address, quint16 nPort);
	CEndPoint(const CEndPoint& copy);
	CEndPoint(SpecialAddress address, quint16 nPort = 0);
	void setAddressWithPort(const QString& address);

	CEndPoint& operator=(const CEndPoint& rhs);
	inline bool operator ==(const CEndPoint& rhs) const;
	inline bool operator !=(const CEndPoint& rhs) const;
	inline bool operator ==(const QHostAddress& rhs) const;
	inline bool operator !=(const QHostAddress& rhs) const;

public:
	void clear();
	QString toStringWithPort() const;

	inline quint16 port() const;
	inline void setPort(const quint16 nPort);

	QString country() const;

	bool isFirewalled() const;
	inline bool isValid() const;

	inline void setAddress(quint32 ip4Addr);
	inline void setAddress(quint8* ip6Addr);
	inline void setAddress(const Q_IPV6ADDR& ip6Addr);
	inline bool setAddress(const QString& address);
	inline void setAddress(const sockaddr* sockaddr);

	friend QDataStream &operator<<(QDataStream &, const CEndPoint &);
	friend QDataStream &operator>>(QDataStream &, CEndPoint &);
};

bool CEndPoint::operator ==(const CEndPoint& rhs) const
{
	return ( QHostAddress::operator ==( rhs ) && m_nPort == rhs.m_nPort );
}
bool CEndPoint::operator !=(const CEndPoint& rhs) const
{
	return !operator==( rhs );
}
bool CEndPoint::operator ==(const QHostAddress& rhs) const
{
	return QHostAddress::operator ==( rhs );
}
bool CEndPoint::operator !=(const QHostAddress& rhs) const
{
	return QHostAddress::operator !=( rhs );
}

quint16 CEndPoint::port() const
{
	return m_nPort;
}

void CEndPoint::setPort(const quint16 nPort)
{
	m_nPort = nPort;
}

bool CEndPoint::isValid() const
{
	return ( !isNull() && m_nPort  &&
	         QHostAddress::operator !=( QHostAddress::Any ) &&
			QHostAddress::operator !=( QHostAddress::AnyIPv6 ) );
}

void CEndPoint::setAddress(quint32 ip4Addr)
{
	m_sCountry.clear();
	QHostAddress::setAddress(ip4Addr);
}

void CEndPoint::setAddress(quint8 *ip6Addr)
{
	m_sCountry.clear();
	QHostAddress::setAddress(ip6Addr);
}

void CEndPoint::setAddress(const Q_IPV6ADDR &ip6Addr)
{
	m_sCountry.clear();
	QHostAddress::setAddress(ip6Addr);
}

bool CEndPoint::setAddress(const QString &address)
{
	m_sCountry.clear();
	return QHostAddress::setAddress(address);
}

void CEndPoint::setAddress(const sockaddr *sockaddr)
{
	m_sCountry.clear();
	QHostAddress::setAddress(sockaddr);
}

QDataStream &operator<<(QDataStream &s, const CEndPoint &rhs);
QDataStream &operator>>(QDataStream &s, CEndPoint &rhs);
#endif // ENDPOINT_H
