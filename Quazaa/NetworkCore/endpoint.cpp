/*
** $Id$
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

#include "endpoint.h"

#include <QStringList>
#include <QtEndian>
#include "geoiplist.h"

#include "debug_new.h"

CEndPoint::CEndPoint() :
    QHostAddress(),
	m_nPort( 0 )
{
}

CEndPoint::CEndPoint(quint32 ip4Addr, quint16 nPort) :
    QHostAddress( ip4Addr ),
	m_nPort( nPort )
{
}

CEndPoint::CEndPoint(quint8* ip6Addr, quint16 nPort) :
    QHostAddress( ip6Addr ),
	m_nPort( nPort )
{
}

CEndPoint::CEndPoint(const Q_IPV6ADDR& ip6Addr, quint16 nPort) :
    QHostAddress( ip6Addr ),
	m_nPort( nPort )
{
}

CEndPoint::CEndPoint(const sockaddr* sockaddr, quint16 nPort) :
    QHostAddress( sockaddr ),
	m_nPort( nPort )
{
}

CEndPoint::CEndPoint(const QString& address, quint16 nPort) :
    QHostAddress( address ),
	m_nPort( nPort )
{
}

CEndPoint::CEndPoint(const QHostAddress& address, quint16 nPort) :
    QHostAddress( address ),
	m_nPort( nPort )
{
}

CEndPoint::CEndPoint(const QString& address)
{
	if ( address.count( ":" ) >= 2 )
	{
		// IPv6

		if ( address.left( 1 ) == "[" )
		{
			// IPv6 with port in brackets
			int pos = address.lastIndexOf( "]:" );

			if ( pos == -1 )
			{
				// error
				QHostAddress::setAddress( quint32( 0 ) );
				m_nPort = 0;
			}
			else
			{
				QString sAddr = address.mid( 1, pos - 1 );
				QHostAddress::setAddress( sAddr );
				m_nPort = address.mid( pos + 2 ).toUShort();
			}
		}
		else
		{
			// IPv6, address only
			m_nPort = 0;
			QHostAddress::setAddress( address );
		}
	}
	else
	{
		// IPv4

		QStringList l1 = address.split( ":" );
		if ( l1.count() != 2 )
		{
			QHostAddress::setAddress( quint32( 0 ) );
			m_nPort = 0;
			return;
		}

		m_nPort = l1.at( 1 ).toUShort();

		QHostAddress::setAddress( l1.at( 0 ) );
	}
}

CEndPoint::CEndPoint(const CEndPoint& copy) :
    QHostAddress( copy ),
    m_nPort( copy.m_nPort ),
    m_sCountry( copy.m_sCountry )
{
}

CEndPoint::CEndPoint(SpecialAddress address, quint16 nPort) :
    QHostAddress( address ),
	m_nPort( nPort )
{
}

void CEndPoint::clear()
{
	m_nPort = 0;
	m_sCountry.clear();
	QHostAddress::clear();
}

QString CEndPoint::toStringWithPort() const
{
	if ( protocol() == QAbstractSocket::IPv4Protocol )
	{
		return toString().append( QString( ":%1" ).arg( m_nPort ) );
	}
	else
	{
		return QString( "[%1]:%2" ).arg( toString() ).arg( m_nPort );
	}
}

void CEndPoint::setAddressWithPort(const QString& address)
{
	if ( address.count(":") >= 2 )
	{
		// IPv6

		if(address.left(1) == "[")
		{
			// IPv6 with port in brackets
			int pos = address.lastIndexOf("]:");

			if(pos == -1)
			{
				// error
				QHostAddress::setAddress(quint32(0));
				m_nPort = 0;
			}
			else
			{
				QString sAddr = address.mid(1, pos - 1);
				QHostAddress::setAddress(sAddr);
				m_nPort = address.mid(pos + 2).toUShort();
			}
		}
		else
		{
			// IPv6, address only
			m_nPort = 0;
			QHostAddress::setAddress(address);
		}
	}
	else
	{
		// IPv4

		QStringList l1 = address.split(":");
		if(l1.count() != 2)
		{
			QHostAddress::setAddress(quint32(0));
			m_nPort = 0;
			return;
		}

		m_nPort = l1.at(1).toUShort();

		QHostAddress::setAddress(l1.at(0));
	}
}

bool CEndPoint::isFirewalled() const
{
	if ( isNull() )
		return true;

	if ( protocol() == 0 ) // IPv4
	{
		quint32 nIp = qToBigEndian( toIPv4Address() );

		if ( (nIp & 0xffff) == 0xa8c0 ) return true; // 192.168
		if ( (nIp & 0xff  ) == 0x0a   ) return true; // 10
		if ( (nIp & 0xf0ff) == 0x10ac ) return true; // 172.16
		if ( (nIp & 0xffff) == 0xfea9 ) return true; // 169.254
		if ( (nIp & 0xff  ) == 0x7f   ) return true; // 127
	}

	return false;
}

QString CEndPoint::country() const
{
	if ( m_sCountry.isEmpty() )
		m_sCountry = geoIP.findCountryCode( *this );

	return m_sCountry;
}

CEndPoint & CEndPoint::operator =(const CEndPoint &rhs)
{
	QHostAddress::operator =( rhs );
	m_sCountry = rhs.m_sCountry;
	m_nPort = rhs.m_nPort;
	return *this;
}

QDataStream &operator<<(QDataStream &s, const CEndPoint &rhs)
{
	s << *static_cast<const QHostAddress*>( &rhs );
	s << rhs.m_sCountry;
	s << rhs.m_nPort;

	return s;
}

QDataStream &operator>>(QDataStream &s, CEndPoint &rhs)
{
	QHostAddress* pHa = static_cast<QHostAddress*>(&rhs);
	s >> *pHa;
	s >> rhs.m_sCountry;
	s >> rhs.m_nPort;

	return s;
}
