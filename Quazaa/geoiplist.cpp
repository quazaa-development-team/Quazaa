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

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "geoiplist.h"
#include "types.h"
#include "systemlog.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

GeoIPList GeoIP;

GeoIPList::GeoIPList()
{
	m_bListLoaded = false;
}

void GeoIPList::loadGeoIP()
{
	bool readFromOriginalGeoIP = false;
	//First trying to deserialize from ser file
	QFile iFile( qApp->applicationDirPath() + "/geoIP.ser" );
	if ( ! iFile.open( QIODevice::ReadOnly ) )
	{
		systemLog.postLog(LogSeverity::Warning, QObject::tr("Unable to load GeoIP serialization file for loading"));
		readFromOriginalGeoIP = true;
	}
	else
	{
		try
		{
			QDataStream iStream( &iFile );
			iStream >> m_lDatabase;
		}
		catch(...)
		{
			systemLog.postLog(LogSeverity::Warning, QObject::tr("Unable to deserialize GeoIP list"));
			readFromOriginalGeoIP = true;
		}
		iFile.close();
	}

	if(readFromOriginalGeoIP)
	{
		QFile file(qApp->applicationDirPath() + "/GeoIP/geoip.dat");
		if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return;
		}

		m_lDatabase.clear();

		// TODO: optimize it

		QTextStream in(&file);
		while(!in.atEnd())
		{
			QStringList line = in.readLine().split(" ");

			CEndPoint rBegin(line[0] + ":0");
			CEndPoint rEnd(line[1] + ":0");
			QString sCountry = line[2];

			GeoIPEntry item = QPair<quint32, QPair<quint32, QString> >(rBegin.toIPv4Address(), QPair<quint32, QString>(rEnd.toIPv4Address(), sCountry));
			m_lDatabase.append(item);
		}

		QFile oFile( qApp->applicationDirPath() +  "/geoIP.ser" );
		if ( ! oFile.open( QIODevice::WriteOnly ) )
		{
			systemLog.postLog(LogSeverity::Error, QObject::tr("Unable to open GeoIP serialization file for saving"));
		}
		else
		{
			try
			{
				QDataStream oStream( &oFile );
				oStream << m_lDatabase;
			}
			catch(...)
			{
				systemLog.postLog(LogSeverity::Error, QObject::tr("Unable to serialize GeoIP list"));
			}
			oFile.close();
		}

	}

	// sort the database so binary search can work
	qSort(m_lDatabase);

	m_bListLoaded = !m_lDatabase.isEmpty();
}

QString GeoIPList::findCountryCode(const quint32 nIp) const
{
	if(!m_bListLoaded)
	{
		return "ZZ";
	}

	int nMiddle;
	int nBegin = 0;
	int nEnd = m_lDatabase.size();

	int n = nEnd - nBegin;

	int nHalf;

	while (n > 0)
	{
		nHalf = n >> 1;

		nMiddle = nBegin + nHalf;

		if (nIp < m_lDatabase.at(nMiddle).first )
		{
			n = nHalf;
		}
		else
		{
			if( nIp <= m_lDatabase.at(nMiddle).second.first )
			{
				return m_lDatabase.at(nMiddle).second.second;
			}
			nBegin = nMiddle + 1;
			n -= nHalf + 1;
		}
	}

	return "ZZ";
}

QString GeoIPList::countryNameFromCode(const QString& code) const
{
	if(code == "AF")
	{
		return QObject::tr("Afghanistan");
	}
	if(code == "AX")
	{
		return QObject::tr("Aland Islands");
	}
	if(code == "AL")
	{
		return QObject::tr("Albania");
	}
	if(code == "DZ")
	{
		return QObject::tr("Algeria");
	}
	if(code == "AS")
	{
		return QObject::tr("American Samoa");
	}
	if(code == "AD")
	{
		return QObject::tr("Andorra");
	}
	if(code == "AO")
	{
		return QObject::tr("Angola");
	}
	if(code == "AI")
	{
		return QObject::tr("Anguilla");
	}
	if(code == "AQ")
	{
		return QObject::tr("Antarctica");
	}
	if(code == "AG")
	{
		return QObject::tr("Antigua And Barbuda");
	}
	if(code == "AR")
	{
		return QObject::tr("Argentina");
	}
	if(code == "AM")
	{
		return QObject::tr("Armenia");
	}
	if(code == "AW")
	{
		return QObject::tr("Aruba");
	}
	if(code == "AU")
	{
		return QObject::tr("Australia");
	}
	if(code == "AT")
	{
		return QObject::tr("Austria");
	}
	if(code == "AZ")
	{
		return QObject::tr("Azerbaijan");
	}
	if(code == "BS")
	{
		return QObject::tr("Bahamas");
	}
	if(code == "BH")
	{
		return QObject::tr("Bahrain");
	}
	if(code == "BD")
	{
		return QObject::tr("Bangladesh");
	}
	if(code == "BB")
	{
		return QObject::tr("Barbados");
	}
	if(code == "BY")
	{
		return QObject::tr("Belarus");
	}
	if(code == "BE")
	{
		return QObject::tr("Belgium");
	}
	if(code == "BZ")
	{
		return QObject::tr("Belize");
	}
	if(code == "BJ")
	{
		return QObject::tr("Benin");
	}
	if(code == "BM")
	{
		return QObject::tr("Bermuda");
	}
	if(code == "BT")
	{
		return QObject::tr("Bhutan");
	}
	if(code == "BO")
	{
		return QObject::tr("Bolivia, Plurinational State Of");
	}
	if(code == "BA")
	{
		return QObject::tr("Bosnia And Herzegovina");
	}
	if(code == "BW")
	{
		return QObject::tr("Botswana");
	}
	if(code == "BV")
	{
		return QObject::tr("Bouvet Island");
	}
	if(code == "BR")
	{
		return QObject::tr("Brazil");
	}
	if(code == "IO")
	{
		return QObject::tr("British Indian Ocean Territory");
	}
	if(code == "BN")
	{
		return QObject::tr("Brunei Darussalam");
	}
	if(code == "BG")
	{
		return QObject::tr("Bulgaria");
	}
	if(code == "BF")
	{
		return QObject::tr("Burkina Faso");
	}
	if(code == "BI")
	{
		return QObject::tr("Burundi");
	}
	if(code == "KH")
	{
		return QObject::tr("Cambodia");
	}
	if(code == "CM")
	{
		return QObject::tr("Cameroon");
	}
	if(code == "CA")
	{
		return QObject::tr("Canada");
	}
	if(code == "CV")
	{
		return QObject::tr("Cape Verde");
	}
	if(code == "KY")
	{
		return QObject::tr("Cayman Islands");
	}
	if(code == "CF")
	{
		return QObject::tr("Central African Republic");
	}
	if(code == "TD")
	{
		return QObject::tr("Chad");
	}
	if(code == "CL")
	{
		return QObject::tr("Chile");
	}
	if(code == "CN")
	{
		return QObject::tr("China");
	}
	if(code == "CX")
	{
		return QObject::tr("Christmas Island");
	}
	if(code == "CC")
	{
		return QObject::tr("Cocos (Keeling) Islands");
	}
	if(code == "CO")
	{
		return QObject::tr("Colombia");
	}
	if(code == "KM")
	{
		return QObject::tr("Comoros");
	}
	if(code == "CG")
	{
		return QObject::tr("Congo");
	}
	if(code == "CD")
	{
		return QObject::tr("Congo, The Democratic Republic Of The");
	}
	if(code == "CK")
	{
		return QObject::tr("Cook Islands");
	}
	if(code == "CR")
	{
		return QObject::tr("Costa Rica");
	}
	if(code == "CI")
	{
		return QObject::tr("Cote D'Ivoire");
	}
	if(code == "HR")
	{
		return QObject::tr("Croatia");
	}
	if(code == "CU")
	{
		return QObject::tr("Cuba");
	}
	if(code == "CY")
	{
		return QObject::tr("Cyprus");
	}
	if(code == "CZ")
	{
		return QObject::tr("Czech Republic");
	}
	if(code == "DK")
	{
		return QObject::tr("Denmark");
	}
	if(code == "DJ")
	{
		return QObject::tr("Djibouti");
	}
	if(code == "DM")
	{
		return QObject::tr("Dominica");
	}
	if(code == "DO")
	{
		return QObject::tr("Dominican Republic");
	}
	if(code == "EC")
	{
		return QObject::tr("Ecuador");
	}
	if(code == "EG")
	{
		return QObject::tr("Egypt");
	}
	if(code == "SV")
	{
		return QObject::tr("El Salvador");
	}
	if(code == "GQ")
	{
		return QObject::tr("Equatorial Guinea");
	}
	if(code == "ER")
	{
		return QObject::tr("Eritrea");
	}
	if(code == "EE")
	{
		return QObject::tr("Estonia");
	}
	if(code == "ET")
	{
		return QObject::tr("Ethiopia");
	}
	if(code == "EU")
	{
		return QObject::tr("European Union");
	}
	if(code == "FK")
	{
		return QObject::tr("Falkland Islands (Malvinas)");
	}
	if(code == "FO")
	{
		return QObject::tr("Faroe Islands");
	}
	if(code == "FJ")
	{
		return QObject::tr("Fiji");
	}
	if(code == "FI")
	{
		return QObject::tr("Finland");
	}
	if(code == "FR")
	{
		return QObject::tr("France");
	}
	if(code == "GF")
	{
		return QObject::tr("French Guiana");
	}
	if(code == "PF")
	{
		return QObject::tr("French Polynesia");
	}
	if(code == "TF")
	{
		return QObject::tr("French Southern Territories");
	}
	if(code == "GA")
	{
		return QObject::tr("Gabon");
	}
	if(code == "GM")
	{
		return QObject::tr("Gambia");
	}
	if(code == "GE")
	{
		return QObject::tr("Georgia");
	}
	if(code == "DE")
	{
		return QObject::tr("Germany");
	}
	if(code == "GH")
	{
		return QObject::tr("Ghana");
	}
	if(code == "GI")
	{
		return QObject::tr("Gibraltar");
	}
	if(code == "GR")
	{
		return QObject::tr("Greece");
	}
	if(code == "GL")
	{
		return QObject::tr("Greenland");
	}
	if(code == "GD")
	{
		return QObject::tr("Grenada");
	}
	if(code == "GP")
	{
		return QObject::tr("Guadeloupe");
	}
	if(code == "GU")
	{
		return QObject::tr("Guam");
	}
	if(code == "GT")
	{
		return QObject::tr("Guatemala");
	}
	if(code == "GG")
	{
		return QObject::tr("Guernsey");
	}
	if(code == "GN")
	{
		return QObject::tr("Guinea");
	}
	if(code == "GW")
	{
		return QObject::tr("Guinea-Bissau");
	}
	if(code == "GY")
	{
		return QObject::tr("Guyana");
	}
	if(code == "HT")
	{
		return QObject::tr("Haiti");
	}
	if(code == "HM")
	{
		return QObject::tr("Heard Island And McDonald Islands");
	}
	if(code == "VA")
	{
		return QObject::tr("Holy See (Vatican City State)");
	}
	if(code == "HN")
	{
		return QObject::tr("Honduras");
	}
	if(code == "HK")
	{
		return QObject::tr("Hong Kong");
	}
	if(code == "HU")
	{
		return QObject::tr("Hungary");
	}
	if(code == "IS")
	{
		return QObject::tr("Iceland");
	}
	if(code == "IN")
	{
		return QObject::tr("India");
	}
	if(code == "ID")
	{
		return QObject::tr("Indonesia");
	}
	if(code == "IR")
	{
		return QObject::tr("Iran (Islamic Republic Of)");
	}
	if(code == "IQ")
	{
		return QObject::tr("Iraq");
	}
	if(code == "IE")
	{
		return QObject::tr("Ireland");
	}
	if(code == "IM")
	{
		return QObject::tr("Isle Of Man");
	}
	if(code == "IL")
	{
		return QObject::tr("Israel");
	}
	if(code == "IT")
	{
		return QObject::tr("Italy");
	}
	if(code == "JM")
	{
		return QObject::tr("Jamaica");
	}
	if(code == "JP")
	{
		return QObject::tr("Japan");
	}
	if(code == "JE")
	{
		return QObject::tr("Jersey");
	}
	if(code == "JO")
	{
		return QObject::tr("Jordan");
	}
	if(code == "KZ")
	{
		return QObject::tr("Kazakhstan");
	}
	if(code == "KE")
	{
		return QObject::tr("Kenya");
	}
	if(code == "KI")
	{
		return QObject::tr("Kiribati");
	}
	if(code == "KP")
	{
		return QObject::tr("Korea, Democratic People's Republic Of");
	}
	if(code == "KR")
	{
		return QObject::tr("Korea, Republic Of");
	}
	if(code == "KW")
	{
		return QObject::tr("Kuwait");
	}
	if(code == "KG")
	{
		return QObject::tr("Kyrgyzsyan");
	}
	if(code == "LA")
	{
		return QObject::tr("Lao People's Democratic Republic");
	}
	if(code == "LV")
	{
		return QObject::tr("Latvia");
	}
	if(code == "LB")
	{
		return QObject::tr("Lebanon");
	}
	if(code == "LS")
	{
		return QObject::tr("Lesotho");
	}
	if(code == "LR")
	{
		return QObject::tr("Liberia");
	}
	if(code == "LY")
	{
		return QObject::tr("Libyan Arab Jamahiriya");
	}
	if(code == "LI")
	{
		return QObject::tr("Lirchtenstein");
	}
	if(code == "LT")
	{
		return QObject::tr("Lithuania");
	}
	if(code == "LU")
	{
		return QObject::tr("Luxembourg");
	}
	if(code == "MO")
	{
		return QObject::tr("Macao");
	}
	if(code == "MK")
	{
		return QObject::tr("Macedonia, The Former Yugoslav Republic Of");
	}
	if(code == "MG")
	{
		return QObject::tr("Madagascar");
	}
	if(code == "MW")
	{
		return QObject::tr("Malawi");
	}
	if(code == "MY")
	{
		return QObject::tr("Malaysia");
	}
	if(code == "MV")
	{
		return QObject::tr("Maldives");
	}
	if(code == "ML")
	{
		return QObject::tr("Mali");
	}
	if(code == "MT")
	{
		return QObject::tr("Malta");
	}
	if(code == "MH")
	{
		return QObject::tr("Marshall Islands");
	}
	if(code == "MQ")
	{
		return QObject::tr("Martinique");
	}
	if(code == "MR")
	{
		return QObject::tr("Mauritania");
	}
	if(code == "MU")
	{
		return QObject::tr("Mauritius");
	}
	if(code == "YT")
	{
		return QObject::tr("Mayotte");
	}
	if(code == "MX")
	{
		return QObject::tr("Mexico");
	}
	if(code == "FM")
	{
		return QObject::tr("Mirconesia, Federated States Of");
	}
	if(code == "MD")
	{
		return QObject::tr("Moldova, Republic Of");
	}
	if(code == "MC")
	{
		return QObject::tr("Monacao");
	}
	if(code == "MN")
	{
		return QObject::tr("Mongolia");
	}
	if(code == "ME")
	{
		return QObject::tr("Montenegro");
	}
	if(code == "MS")
	{
		return QObject::tr("Montserrat");
	}
	if(code == "MA")
	{
		return QObject::tr("Morocco");
	}
	if(code == "MZ")
	{
		return QObject::tr("Mozambique");
	}
	if(code == "MM")
	{
		return QObject::tr("Myanmar");
	}
	if(code == "NA")
	{
		return QObject::tr("Namibia");
	}
	if(code == "NR")
	{
		return QObject::tr("Nauru");
	}
	if(code == "NP")
	{
		return QObject::tr("Nepal");
	}
	if(code == "NL")
	{
		return QObject::tr("Netherlands");
	}
	if(code == "AN")
	{
		return QObject::tr("Netherlands Anitlles");
	}
	if(code == "NC")
	{
		return QObject::tr("New Caledonia");
	}
	if(code == "NZ")
	{
		return QObject::tr("New Zealand");
	}
	if(code == "NI")
	{
		return QObject::tr("Nicaragua");
	}
	if(code == "NE")
	{
		return QObject::tr("Niger");
	}
	if(code == "NG")
	{
		return QObject::tr("Nigeria");
	}
	if(code == "NU")
	{
		return QObject::tr("Niue");
	}
	if(code == "NF")
	{
		return QObject::tr("Norfolk Island");
	}
	if(code == "MP")
	{
		return QObject::tr("Northern Mariana Islands");
	}
	if(code == "NO")
	{
		return QObject::tr("Norway");
	}
	if(code == "OM")
	{
		return QObject::tr("Oman");
	}
	if(code == "PK")
	{
		return QObject::tr("Pakistan");
	}
	if(code == "PW")
	{
		return QObject::tr("Palau");
	}
	if(code == "PS")
	{
		return QObject::tr("Palestinian Territory, Occupied");
	}
	if(code == "PA")
	{
		return QObject::tr("Panama");
	}
	if(code == "PG")
	{
		return QObject::tr("Papua New Guinea");
	}
	if(code == "PY")
	{
		return QObject::tr("Paraguay");
	}
	if(code == "PE")
	{
		return QObject::tr("Peru");
	}
	if(code == "PH")
	{
		return QObject::tr("Philippines");
	}
	if(code == "PN")
	{
		return QObject::tr("Pitcairn");
	}
	if(code == "PL")
	{
		return QObject::tr("Poland");
	}
	if(code == "PT")
	{
		return QObject::tr("Portugal");
	}
	if(code == "PR")
	{
		return QObject::tr("Puerto Rico");
	}
	if(code == "QA")
	{
		return QObject::tr("Qatar");
	}
	if(code == "RE")
	{
		return QObject::tr("Reunion");
	}
	if(code == "RO")
	{
		return QObject::tr("Romania");
	}
	if(code == "RU")
	{
		return QObject::tr("Russian Federation");
	}
	if(code == "RW")
	{
		return QObject::tr("Rwanda");
	}
	if(code == "BL")
	{
		return QObject::tr("Saint Barthelemy");
	}
	if(code == "SH")
	{
		return QObject::tr("Saint Helena, Ascension And Tristan Da Cunha");
	}
	if(code == "KN")
	{
		return QObject::tr("Saint Kitts And Nevis");
	}
	if(code == "LC")
	{
		return QObject::tr("Saint Lucia");
	}
	if(code == "MT")
	{
		return QObject::tr("Saint Martin (French Part)");
	}
	if(code == "PM")
	{
		return QObject::tr("Saint Pierre And Miquelon");
	}
	if(code == "VC")
	{
		return QObject::tr("Saint Vincent And The Grenadines");
	}
	if(code == "WS")
	{
		return QObject::tr("Samoa");
	}
	if(code == "SM")
	{
		return QObject::tr("San Marino");
	}
	if(code == "ST")
	{
		return QObject::tr("Sao Tome And Principe");
	}
	if(code == "SA")
	{
		return QObject::tr("Saudi Arabia");
	}
	if(code == "SN")
	{
		return QObject::tr("Senegal");
	}
	if(code == "RS")
	{
		return QObject::tr("Serbia");
	}
	if(code == "CS")
	{
		return QObject::tr("Serbia and Montenegro");
	}
	if(code == "SC")
	{
		return QObject::tr("Seychelles");
	}
	if(code == "SL")
	{
		return QObject::tr("Sierra Leone");
	}
	if(code == "SG")
	{
		return QObject::tr("Singapore");
	}
	if(code == "SK")
	{
		return QObject::tr("Slovakia");
	}
	if(code == "SI")
	{
		return QObject::tr("Slovenia");
	}
	if(code == "SB")
	{
		return QObject::tr("Solomon Islands");
	}
	if(code == "SO")
	{
		return QObject::tr("Somalia");
	}
	if(code == "ZA")
	{
		return QObject::tr("South Africa");
	}
	if(code == "GS")
	{
		return QObject::tr("South Georgia And The South Sandwich Islands");
	}
	if(code == "ES")
	{
		return QObject::tr("Spain");
	}
	if(code == "LK")
	{
		return QObject::tr("Sri Lanka");
	}
	if(code == "SD")
	{
		return QObject::tr("Sudan");
	}
	if(code == "SR")
	{
		return QObject::tr("Suriname");
	}
	if(code == "SJ")
	{
		return QObject::tr("Svalbard And Jan Mayen");
	}
	if(code == "SZ")
	{
		return QObject::tr("Swaziland");
	}
	if(code == "SE")
	{
		return QObject::tr("Sweden");
	}
	if(code == "CH")
	{
		return QObject::tr("Switzerland");
	}
	if(code == "SY")
	{
		return QObject::tr("Syrian Arab Republic");
	}
	if(code == "TW")
	{
		return QObject::tr("Taiwan, Province Of China");
	}
	if(code == "TJ")
	{
		return QObject::tr("Tajikistan");
	}
	if(code == "TZ")
	{
		return QObject::tr("Tanzania, United Republic Of");
	}
	if(code == "TH")
	{
		return QObject::tr("Thailand");
	}
	if(code == "TL")
	{
		return QObject::tr("Timor-Leste");
	}
	if(code == "TG")
	{
		return QObject::tr("Togo");
	}
	if(code == "TK")
	{
		return QObject::tr("Tokelau");
	}
	if(code == "TO")
	{
		return QObject::tr("Tonga");
	}
	if(code == "TT")
	{
		return QObject::tr("Trinadad And Tobago");
	}
	if(code == "TN")
	{
		return QObject::tr("Tunisia");
	}
	if(code == "TR")
	{
		return QObject::tr("Turkey");
	}
	if(code == "TM")
	{
		return QObject::tr("Turkmenistan");
	}
	if(code == "TC")
	{
		return QObject::tr("Turks And Caicos Islands");
	}
	if(code == "TV")
	{
		return QObject::tr("Tubalu");
	}
	if(code == "UG")
	{
		return QObject::tr("Uganda");
	}
	if(code == "UA")
	{
		return QObject::tr("Ukraine");
	}
	if(code == "AE")
	{
		return QObject::tr("United Arab Emirates");
	}
	if(code == "GB")
	{
		return QObject::tr("United Kingdom");
	}
	if(code == "US")
	{
		return QObject::tr("United States");
	}
	if(code == "UM")
	{
		return QObject::tr("United States Minor Ooutlying Islands");
	}
	if(code == "UY")
	{
		return QObject::tr("Uraguay");
	}
	if(code == "UZ")
	{
		return QObject::tr("Uzbekistan");
	}
	if(code == "VU")
	{
		return QObject::tr("Vanuatu");
	}
	if(code == "VA")
	{
		return QObject::tr("Vatican City State (Holy See)");
	}
	if(code == "VE")
	{
		return QObject::tr("Venezuela, Boliviarian Ropublic Of");
	}
	if(code == "VN")
	{
		return QObject::tr("Viet Nam");
	}
	if(code == "VG")
	{
		return QObject::tr("Virgin Islands (British)");
	}
	if(code == "VI")
	{
		return QObject::tr("Virgin Islands (U.S.)");
	}
	if(code == "WF")
	{
		return QObject::tr("Wallis And Futuna");
	}
	if(code == "EH")
	{
		return QObject::tr("Western Sahara");
	}
	if(code == "YE")
	{
		return QObject::tr("Yemen");
	}
	if(code == "YU")
	{
		return QObject::tr("Yugoslavia");
	}
	if(code == "ZM")
	{
		return QObject::tr("Zambia");
	}
	if(code == "ZW")
	{
		return QObject::tr("Zimbabwe");
	}
	else
	{
		return QObject::tr("Unknown");
	}
}

