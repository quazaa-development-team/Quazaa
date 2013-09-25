/*
** hostcachehost.h
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

#ifndef HOSTCACHEHOST_H
#define HOSTCACHEHOST_H

#include "types.h"

class CHostCacheHost
{
public:
	CEndPoint       m_oAddress;     // Adres huba
	QDateTime       m_tTimestamp;   // Kiedy ostatnio widziany

	quint32         m_nQueryKey;    // QK
	CEndPoint       m_nKeyHost;     // host dla ktorego jest QK
	QDateTime       m_nKeyTime;     // kiedy odebrano OK?

	QDateTime       m_tAck;         // czas ostatniej operacji wymagajacej potwierdzenia

	QDateTime       m_tLastQuery;   // kiedy poslano ostatnie zapytanie?
	QDateTime       m_tRetryAfter;  // kiedy mozna ponowic?
	QDateTime       m_tLastConnect; // kiedy ostatnio sie polaczylismy?
	quint32         m_nFailures;

	// for statistics purposes only
	bool            m_bCountryObtained;

public:
	CHostCacheHost() :
	    m_bCountryObtained( false )
	{
		m_nQueryKey = 0;
		m_nFailures = 0;
		m_nKeyTime = m_tAck = m_tLastConnect =
					 m_tLastQuery = m_tRetryAfter = QDateTime::fromTime_t(86400).toUTC();
		Q_ASSERT(m_tLastConnect.timeSpec() == Qt::UTC);
	}

	bool canQuery(QDateTime tNow = common::getDateTimeUTC());
	void setKey(quint32 nKey, CEndPoint* pHost = 0);
};

typedef QList<CHostCacheHost*>::iterator CHostCacheIterator;

#endif // HOSTCACHEHOST_H
