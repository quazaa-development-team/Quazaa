#ifndef GEOIPLIST_H
#define GEOIPLIST_H

#include "types.h"

#include <QObject>
#include <QList>
#include <QPair>

class GeoIPList
{
public:
	struct sGeoID {
		QString countryCode;
		QString countryName;
	}; sGeoID GeoID;

	QList<QPair<quint32, QPair<quint32, QString> > > m_lDatabase;

	GeoIPList();
	void loadGeoIP();
	inline QString findCountryCode(QString IP)
	{
		IPv4_ENDPOINT ipAddress(IP);

		return findCountryCode(ipAddress);
	}
	inline QString findCountryCode(IPv4_ENDPOINT& ip)
	{
		return findCountryCode(ip.ip);
	}

	QString findCountryCode(quint32& nIp, quint32 nBegin = 0, quint32 nEnd = 0xFFFFFFFF);
	QString countryNameFromCode(QString code);
};

extern GeoIPList GeoIP;

#endif // GEOIPLIST_H
