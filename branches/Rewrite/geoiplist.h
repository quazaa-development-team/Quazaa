/*
** geoiplist.h
**
** Copyright © Quazaa Development Team, 2009-2011.
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

#ifndef GEOIPLIST_H
#define GEOIPLIST_H

#include "types.h"

#include <QObject>
#include <QList>
#include <QPair>

class GeoIPList
{
protected:
	bool	m_bListLoaded;
public:
	struct sGeoID
	{
		QString countryCode;
		QString countryName;
	};
	sGeoID GeoID;

	QList<QPair<quint32, QPair<quint32, QString> > > m_lDatabase;

	GeoIPList();
	void loadGeoIP();
	inline QString findCountryCode(QString IP)
	{
		CEndPoint ipAddress(IP);

		return findCountryCode(ipAddress);
	}
	inline QString findCountryCode(CEndPoint& ip)
	{
		if(ip.protocol() == 1)   // IPv6
		{
			return "ZZ";
		}
		quint32 ip4 = ip.toIPv4Address();
		return findCountryCode(ip4);
	}

	QString findCountryCode(quint32& nIp, quint32 nBegin = 0, quint32 nEnd = 0xFFFFFFFF);
	QString countryNameFromCode(QString code);
};

extern GeoIPList GeoIP;

#endif // GEOIPLIST_H
