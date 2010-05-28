#ifndef GEOIPLIST_H
#define GEOIPLIST_H

#include <QObject>
#include <QList>
#include <QStringList>

class GeoIPList : public QList<QStringList>
{
public:
	GeoIPList();
	void loadGeoIP();
	QString findCountryCode(QString IP);
};

extern GeoIPList GeoIP;

#endif // GEOIPLIST_H
