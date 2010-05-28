#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "geoiplist.h"
#include "types.h"
#include "systemlog.h"

GeoIPList GeoIP;

GeoIPList::GeoIPList()
{

}

void GeoIPList::loadGeoIP()
{
	QFile file(qApp->applicationDirPath() + "/GeoIP/geoip.dat");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	clear();
	QTextStream in(&file);
	while (!in.atEnd()) {
		QStringList line = in.readLine().split(" ");
		append(line);
		//process_line(line);
	}
}

QString GeoIPList::findCountryCode(QString IP)
{
	IPv4_ENDPOINT ipAddress(IP);
	for (int index = 0; index < size(); ++index)
	{
		QStringList currentItem = at(index);
		IPv4_ENDPOINT rangeBegin(currentItem.at(0) + ":0");
		IPv4_ENDPOINT rangeEnd(currentItem.at(1) + ":0");

		if (ipAddress.ip > rangeBegin.ip && ipAddress.ip < rangeEnd.ip)
			return currentItem.at(2);
	}
	return "ZZ";
}
