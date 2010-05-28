#ifndef GEOIPLIST_H
#define GEOIPLIST_H

#include <QObject>
#include <QList>
#include <QStringList>

class GeoIPList : public QList<QStringList>
{
public:
	struct sGeoID {
		QString countryCode;
		QString countryName;
	}; sGeoID GeoID;

	GeoIPList();
	void loadGeoIP();
	QString findCountryCode(QString IP);
	QString countryNameFromCode(QString code);
	QList<QStringList> *countryNameList;
};

extern GeoIPList GeoIP;

#endif // GEOIPLIST_H
