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
#include "network.h"
#include "quazaasettings.h"

class CHostCacheHost
{
public:
	CEndPoint   m_oAddress;     // Adres huba
	quint32     m_tTimestamp;   // Kiedy ostatnio widziany

	quint32     m_nQueryKey;    // QK
	CEndPoint   m_nKeyHost;     // host dla ktorego jest QK
	quint32     m_nKeyTime;     // kiedy odebrano OK?

	quint32     m_tAck;         // czas ostatniej operacji wymagajacej potwierdzenia

	quint32     m_tLastQuery;   // kiedy poslano ostatnie zapytanie?
	quint32     m_tRetryAfter;  // kiedy mozna ponowic?
	quint32     m_tLastConnect; // kiedy ostatnio sie polaczylismy?
	quint32     m_nFailures;

private:
	CHostCacheHost(CEndPoint oAddress, quint32 tTimestamp);
public:
	~CHostCacheHost();

	bool canQuery(const quint32 tNow = common::getTNowUTC());
	void setKey(quint32 nKey, const quint32 tNow = common::getTNowUTC(), CEndPoint* pHost = NULL);

	friend class CHostCache;
};

#endif // HOSTCACHEHOST_H
