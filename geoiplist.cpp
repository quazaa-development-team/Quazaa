//
// geoiplist.cpp
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

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "geoiplist.h"
#include "types.h"
#include "systemlog.h"

GeoIPList GeoIP;

GeoIPList::GeoIPList()
{
	m_bListLoaded = false;
}

void GeoIPList::loadGeoIP()
{
	QFile file(qApp->applicationDirPath() + "/GeoIP/geoip.dat");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	m_lDatabase.clear();

	// TODO: optimize it

	QTextStream in(&file);
	while (!in.atEnd()) {
		QStringList line = in.readLine().split(" ");

		IPv4_ENDPOINT rBegin(line[0] + ":0");
		IPv4_ENDPOINT rEnd(line[1] + ":0");
		QString sCountry = line[2];

		QPair<quint32, QPair<quint32, QString> > item = QPair<quint32, QPair<quint32, QString> >(rBegin.ip, QPair<quint32, QString>(rEnd.ip, sCountry));
		m_lDatabase.append(item);
	}

	m_bListLoaded = !m_lDatabase.isEmpty();
}


QString GeoIPList::findCountryCode(quint32& nIp, quint32 nBegin, quint32 nEnd)
{
	if( !m_bListLoaded )
		return "ZZ";

	nEnd = qMin<quint32>(nEnd, m_lDatabase.size());

	if( nEnd - nBegin < 3 )
	{
		// just in case...
		for( uint i = nBegin; i <= nEnd; i++ )
		{
			if( m_lDatabase[i].first <= nIp && m_lDatabase[i].second.first >= nIp )
			{
				return m_lDatabase[i].second.second;
			}
		}

		return "ZZ";
	}
	else
	{
		int nIndex = ((nEnd - nBegin) / 2) + nBegin;

		if( nIp > m_lDatabase[nIndex].first )
		{
			// range above half

			return findCountryCode(nIp, nIndex, nEnd);
		}
		else if( nIp < m_lDatabase[nIndex].first )
		{
			// range below half

			return findCountryCode(nIp, nBegin, nIndex);
		}

	}

	return "ZZ";
}

QString GeoIPList::countryNameFromCode(QString code)
{
	if (code == "AF")
		return qApp->tr("Afghanistan");
	if (code == "AX")
		return qApp->tr("Aland Islands");
	if (code == "AL")
		return qApp->tr("Albania");
	if (code == "DZ")
		return qApp->tr("Algeria");
	if (code == "AS")
		return qApp->tr("American Samoa");
	if (code == "AD")
		return qApp->tr("Andorra");
	if (code == "AO")
		return qApp->tr("Angola");
	if (code == "AI")
		return qApp->tr("Anguilla");
	if (code == "AQ")
		return qApp->tr("Antarctica");
	if (code == "AG")
		return qApp->tr("Antigua And Barbuda");
	if (code == "AR")
		return qApp->tr("Argentina");
	if (code == "AM")
		return qApp->tr("Armenia");
	if (code == "AW")
		return qApp->tr("Aruba");
	if (code == "AU")
		return qApp->tr("Australia");
	if (code == "AT")
		return qApp->tr("Austria");
	if (code == "AZ")
		return qApp->tr("Azerbaijan");
	if (code == "BS")
		return qApp->tr("Bahamas");
	if (code == "BH")
		return qApp->tr("Bahrain");
	if (code == "BD")
		return qApp->tr("Bangladesh");
	if (code == "BB")
		return qApp->tr("Barbados");
	if (code == "BY")
		return qApp->tr("Belarus");
	if (code == "BE")
		return qApp->tr("Belgium");
	if (code == "BZ")
		return qApp->tr("Belize");
	if (code == "BJ")
		return qApp->tr("Benin");
	if (code == "BM")
		return qApp->tr("Bermuda");
	if (code == "BT")
		return qApp->tr("Bhutan");
	if (code == "BO")
		return qApp->tr("Bolivia, Plurinational State Of");
	if (code == "BA")
		return qApp->tr("Bosnia And Herzegovina");
	if (code == "BW")
		return qApp->tr("Botswana");
	if (code == "BV")
		return qApp->tr("Bouvet Island");
	if (code == "BR")
		return qApp->tr("Brazil");
	if (code == "IO")
		return qApp->tr("British Indian Ocean Territory");
	if (code == "BN")
		return qApp->tr("Brunei Darussalam");
	if (code == "BG")
		return qApp->tr("Bulgaria");
	if (code == "BF")
		return qApp->tr("Burkina Faso");
	if (code == "BI")
		return qApp->tr("Burundi");
	if (code == "KH")
		return qApp->tr("Cambodia");
	if (code == "CM")
		return qApp->tr("Cameroon");
	if (code == "CA")
		return qApp->tr("Canada");
	if (code == "CV")
		return qApp->tr("Cape Verde");
	if (code == "KY")
		return qApp->tr("Cayman Islands");
	if (code == "CF")
		return qApp->tr("Central African Republic");
	if (code == "TD")
		return qApp->tr("Chad");
	if (code == "CL")
		return qApp->tr("Chile");
	if (code == "CN")
		return qApp->tr("China");
	if (code == "CX")
		return qApp->tr("Christmas Island");
	if (code == "CC")
		return qApp->tr("Cocos (Keeling) Islands");
	if (code == "CO")
		return qApp->tr("Colombia");
	if (code == "KM")
		return qApp->tr("Comoros");
	if (code == "CG")
		return qApp->tr("Congo");
	if (code == "CD")
		return qApp->tr("Congo, The Democratic Republic Of The");
	if (code == "CK")
		return qApp->tr("Cook Islands");
	if (code == "CR")
		return qApp->tr("Costa Rica");
	if (code == "CI")
		return qApp->tr("Cote D'Ivoire");
	if (code == "HR")
		return qApp->tr("Croatia");
	if (code == "CU")
		return qApp->tr("Cuba");
	if (code == "CY")
		return qApp->tr("Cyprus");
	if (code == "CZ")
		return qApp->tr("Czech Republic");
	if (code == "DK")
		return qApp->tr("Denmark");
	if (code == "DJ")
		return qApp->tr("Djibouti");
	if (code == "DM")
		return qApp->tr("Dominica");
	if (code == "DO")
		return qApp->tr("Dominican Republic");
	if (code == "EC")
		return qApp->tr("Ecuador");
	if (code == "EG")
		return qApp->tr("Egypt");
	if (code == "SV")
		return qApp->tr("El Salvador");
	if (code == "GQ")
		return qApp->tr("Equatorial Guinea");
	if (code == "ER")
		return qApp->tr("Eritrea");
	if (code == "EE")
		return qApp->tr("Estonia");
	if (code == "ET")
		return qApp->tr("Ethiopia");
	if (code == "EU")
		return qApp->tr("European Union");
	if (code == "FK")
		return qApp->tr("Falkland Islands (Malvinas)");
	if (code == "FO")
		return qApp->tr("Faroe Islands");
	if (code == "FJ")
		return qApp->tr("Fiji");
	if (code == "FI")
		return qApp->tr("Finland");
	if (code == "FR")
		return qApp->tr("France");
	if (code == "GF")
		return qApp->tr("French Guiana");
	if (code == "PF")
		return qApp->tr("French Polynesia");
	if (code == "TF")
		return qApp->tr("French Southern Territories");
	if (code == "GA")
		return qApp->tr("Gabon");
	if (code == "GM")
		return qApp->tr("Gambia");
	if (code == "GE")
		return qApp->tr("Georgia");
	if (code == "DE")
		return qApp->tr("Germany");
	if (code == "GH")
		return qApp->tr("Ghana");
	if (code == "GI")
		return qApp->tr("Gibraltar");
	if (code == "GR")
		return qApp->tr("Greece");
	if (code == "GL")
		return qApp->tr("Greenland");
	if (code == "GD")
		return qApp->tr("Grenada");
	if (code == "GP")
		return qApp->tr("Guadeloupe");
	if (code == "GU")
		return qApp->tr("Guam");
	if (code == "GT")
		return qApp->tr("Guatemala");
	if (code == "GG")
		return qApp->tr("Guernsey");
	if (code == "GN")
		return qApp->tr("Guinea");
	if (code == "GW")
		return qApp->tr("Guinea-Bissau");
	if (code == "GY")
		return qApp->tr("Guyana");
	if (code == "HT")
		return qApp->tr("Haiti");
	if (code == "HM")
		return qApp->tr("Heard Island And McDonald Islands");
	if (code == "VA")
		return qApp->tr("Holy See (Vatican City State)");
	if (code == "HN")
		return qApp->tr("Honduras");
	if (code == "HK")
		return qApp->tr("Hong Kong");
	if (code == "HU")
		return qApp->tr("Hungary");
	if (code == "IS")
		return qApp->tr("Iceland");
	if (code == "IN")
		return qApp->tr("India");
	if (code == "ID")
		return qApp->tr("Indonesia");
	if (code == "IR")
		return qApp->tr("Iran (Islamic Republic Of)");
	if (code == "IQ")
		return qApp->tr("Iraq");
	if (code == "IE")
		return qApp->tr("Ireland");
	if (code == "IM")
		return qApp->tr("Isle Of Man");
	if (code == "IL")
		return qApp->tr("Israel");
	if (code == "IT")
		return qApp->tr("Italy");
	if (code == "JM")
		return qApp->tr("Jamaica");
	if (code == "JP")
		return qApp->tr("Japan");
	if (code == "JE")
		return qApp->tr("Jersey");
	if (code == "JO")
		return qApp->tr("Jordan");
	if (code == "KZ")
		return qApp->tr("Kazakhstan");
	if (code == "KE")
		return qApp->tr("Kenya");
	if (code == "KI")
		return qApp->tr("Kiribati");
	if (code == "KP")
		return qApp->tr("Korea, Democratic People's Republic Of");
	if (code == "KR")
		return qApp->tr("Korea, Republic Of");
	if (code == "KW")
		return qApp->tr("Kuwait");
	if (code == "KG")
		return qApp->tr("Kyrgyzsyan");
	if (code == "LA")
		return qApp->tr("Lao People's Democratic Republic");
	if (code == "LV")
		return qApp->tr("Latvia");
	if (code == "LB")
		return qApp->tr("Lebanon");
	if (code == "LS")
		return qApp->tr("Lesotho");
	if (code == "LR")
		return qApp->tr("Liberia");
	if (code == "LY")
		return qApp->tr("Libyan Arab Jamahiriya");
	if (code == "LI")
		return qApp->tr("Lirchtenstein");
	if (code == "LT")
		return qApp->tr("Lithuania");
	if (code == "LU")
		return qApp->tr("Luxembourg");
	if (code == "MO")
		return qApp->tr("Macao");
	if (code == "MK")
		return qApp->tr("Macedonia, The Former Yugoslav Republic Of");
	if (code == "MG")
		return qApp->tr("Madagascar");
	if (code == "MW")
		return qApp->tr("Malawi");
	if (code == "MY")
		return qApp->tr("Malaysia");
	if (code == "MV")
		return qApp->tr("Maldives");
	if (code == "ML")
		return qApp->tr("Mali");
	if (code == "MT")
		return qApp->tr("Malta");
	if (code == "MH")
		return qApp->tr("Marshall Islands");
	if (code == "MQ")
		return qApp->tr("Martinique");
	if (code == "MR")
		return qApp->tr("Mauritania");
	if (code == "MU")
		return qApp->tr("Mauritius");
	if (code == "YT")
		return qApp->tr("Mayotte");
	if (code == "MX")
		return qApp->tr("Mexico");
	if (code == "FM")
		return qApp->tr("Mirconesia, Federated States Of");
	if (code == "MD")
		return qApp->tr("Moldova, Republic Of");
	if (code == "MC")
		return qApp->tr("Monacao");
	if (code == "MN")
		return qApp->tr("Mongolia");
	if (code == "ME")
		return qApp->tr("Montenegro");
	if (code == "MS")
		return qApp->tr("Montserrat");
	if (code == "MA")
		return qApp->tr("Morocco");
	if (code == "MZ")
		return qApp->tr("Mozambique");
	if (code == "MM")
		return qApp->tr("Myanmar");
	if (code == "NA")
		return qApp->tr("Namibia");
	if (code == "NR")
		return qApp->tr("Nauru");
	if (code == "NP")
		return qApp->tr("Nepal");
	if (code == "NL")
		return qApp->tr("Netherlands");
	if (code == "AN")
		return qApp->tr("Netherlands Anitlles");
	if (code == "NC")
		return qApp->tr("New Caledonia");
	if (code == "NZ")
		return qApp->tr("New Zealand");
	if (code == "NI")
		return qApp->tr("Nicaragua");
	if (code == "NE")
		return qApp->tr("Niger");
	if (code == "NG")
		return qApp->tr("Nigeria");
	if (code == "NU")
		return qApp->tr("Niue");
	if (code == "NF")
		return qApp->tr("Norfolk Island");
	if (code == "MP")
		return qApp->tr("Northern Mariana Islands");
	if (code == "NO")
		return qApp->tr("Norway");
	if (code == "OM")
		return qApp->tr("Oman");
	if (code == "PK")
		return qApp->tr("Pakistan");
	if (code == "PW")
		return qApp->tr("Palau");
	if (code == "PS")
		return qApp->tr("Palestinian Territory, Occupied");
	if (code == "PA")
		return qApp->tr("Panama");
	if (code == "PG")
		return qApp->tr("Papua New Guinea");
	if (code == "PY")
		return qApp->tr("Paraguay");
	if (code == "PE")
		return qApp->tr("Peru");
	if (code == "PH")
		return qApp->tr("Philippines");
	if (code == "PN")
		return qApp->tr("Pitcairn");
	if (code == "PL")
		return qApp->tr("Poland");
	if (code == "PT")
		return qApp->tr("Portugal");
	if (code == "PR")
		return qApp->tr("Puerto Rico");
	if (code == "QA")
		return qApp->tr("Qatar");
	if (code == "RE")
		return qApp->tr("Reunion");
	if (code == "RO")
		return qApp->tr("Romania");
	if (code == "RU")
		return qApp->tr("Russian Federation");
	if (code == "RW")
		return qApp->tr("Rwanda");
	if (code == "BL")
		return qApp->tr("Saint Barthelemy");
	if (code == "SH")
		return qApp->tr("Saint Helena, Ascension And Tristan Da Cunha");
	if (code == "KN")
		return qApp->tr("Saint Kitts And Nevis");
	if (code == "LC")
		return qApp->tr("Saint Lucia");
	if (code == "MT")
		return qApp->tr("Saint Martin (French Part)");
	if (code == "PM")
		return qApp->tr("Saint Pierre And Miquelon");
	if (code == "VC")
		return qApp->tr("Saint Vincent And The Grenadines");
	if (code == "WS")
		return qApp->tr("Samoa");
	if (code == "SM")
		return qApp->tr("San Marino");
	if (code == "ST")
		return qApp->tr("Sao Tome And Principe");
	if (code == "SA")
		return qApp->tr("Saudi Arabia");
	if (code == "SN")
		return qApp->tr("Senegal");
	if (code == "RS")
		return qApp->tr("Serbia");
	if (code == "CS")
		return qApp->tr("Serbia and Montenegro");
	if (code == "SC")
		return qApp->tr("Seychelles");
	if (code == "SL")
		return qApp->tr("Sierra Leone");
	if (code == "SG")
		return qApp->tr("Singapore");
	if (code == "SK")
		return qApp->tr("Slovakia");
	if (code == "SI")
		return qApp->tr("Slovenia");
	if (code == "SB")
		return qApp->tr("Solomon Islands");
	if (code == "SO")
		return qApp->tr("Somalia");
	if (code == "ZA")
		return qApp->tr("South Africa");
	if (code == "GS")
		return qApp->tr("South Georgia And The South Sandwich Islands");
	if (code == "ES")
		return qApp->tr("Spain");
	if (code == "LK")
		return qApp->tr("Sri Lanka");
	if (code == "SD")
		return qApp->tr("Sudan");
	if (code == "SR")
		return qApp->tr("Suriname");
	if (code == "SJ")
		return qApp->tr("Svalbard And Jan Mayen");
	if (code == "SZ")
		return qApp->tr("Swaziland");
	if (code == "SE")
		return qApp->tr("Sweden");
	if (code == "CH")
		return qApp->tr("Switzerland");
	if (code == "SY")
		return qApp->tr("Syrian Arab Republic");
	if (code == "TW")
		return qApp->tr("Taiwan, Province Of China");
	if (code == "TJ")
		return qApp->tr("Tajikistan");
	if (code == "TZ")
		return qApp->tr("Tanzania, United Republic Of");
	if (code == "TH")
		return qApp->tr("Thailand");
	if (code == "TL")
		return qApp->tr("Timor-Leste");
	if (code == "TG")
		return qApp->tr("Togo");
	if (code == "TK")
		return qApp->tr("Tokelau");
	if (code == "TO")
		return qApp->tr("Tonga");
	if (code == "TT")
		return qApp->tr("Trinadad And Tobago");
	if (code == "TN")
		return qApp->tr("Tunisia");
	if (code == "TR")
		return qApp->tr("Turkey");
	if (code == "TM")
		return qApp->tr("Turkmenistan");
	if (code == "TC")
		return qApp->tr("Turks And Caicos Islands");
	if (code == "TV")
		return qApp->tr("Tubalu");
	if (code == "UG")
		return qApp->tr("Uganda");
	if (code == "UA")
		return qApp->tr("Ukraine");
	if (code == "AE")
		return qApp->tr("United Arab Emirates");
	if (code == "GB")
		return qApp->tr("United Kingdom");
	if (code == "US")
		return qApp->tr("United States");
	if (code == "UM")
		return qApp->tr("United States Minor Ooutlying Islands");
	if (code == "UY")
		return qApp->tr("Uraguay");
	if (code == "UZ")
		return qApp->tr("Uzbekistan");
	if (code == "VU")
		return qApp->tr("Vanuatu");
	if (code == "VA")
		return qApp->tr("Vatican City State (Holy See)");
	if (code == "VE")
		return qApp->tr("Venezuela, Boliviarian Ropublic Of");
	if (code == "VN")
		return qApp->tr("Viet Nam");
	if (code == "VG")
		return qApp->tr("Virgin Islands (British)");
	if (code == "VI")
		return qApp->tr("Virgin Islands (U.S.)");
	if (code == "WF")
		return qApp->tr("Wallis And Futuna");
	if (code == "EH")
		return qApp->tr("Western Sahara");
	if (code == "YE")
		return qApp->tr("Yemen");
	if (code == "YU")
		return qApp->tr("Yugoslavia");
	if (code == "ZM")
		return qApp->tr("Zambia");
	if (code == "ZW")
		return qApp->tr("Zimbabwe");
	else
		return qApp->tr("Unknown");
}
