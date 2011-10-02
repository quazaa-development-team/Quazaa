/*
** hostcache.h
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

#ifndef HOSTCACHE_H
#define HOSTCACHE_H

#include "types.h"
#include <QList>
#include <QMutex>

const quint32 ReconnectTime = 3600;

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

public:
	CHostCacheHost()
	{
		m_nQueryKey = 0;
	}

	bool CanQuery(QDateTime tNow = QDateTime::currentDateTimeUtc())
	{
		if(tNow.isNull())
		{
			tNow = QDateTime::currentDateTimeUtc();
		}

		if(!m_tAck.isNull())
		{
			return false;
		}

		if(m_tTimestamp.secsTo(tNow) > 3600)
		{
			return false;
		}

		if(!m_tRetryAfter.isNull() && tNow.secsTo(m_tRetryAfter) < 0)
		{
			return false;
		}

		if(m_tLastQuery.isNull())
		{
			return true;
		}

		return (m_tLastQuery.secsTo(tNow) >= 120);
	}

	void SetKey(quint32 nKey, CEndPoint* pHost = 0);
};

class CHostCache
{

public:
	QList<CHostCacheHost*>  m_lHosts;
	QMutex					m_pSection;

public:
	CHostCache();
	~CHostCache();

	CHostCacheHost* Add(CEndPoint host, QDateTime ts = QDateTime::currentDateTimeUtc());
	void AddXTry(QString& sHeader);
	QString GetXTry();
	void Update(CEndPoint oHost);
	void Update(CHostCacheHost* pHost);
	void Remove(CHostCacheHost* pRemove);
	CHostCacheHost* Find(CEndPoint oHost);
	void OnFailure(CEndPoint addr);
	CHostCacheHost* Get();
	CHostCacheHost* GetConnectable(QDateTime tNow = QDateTime::currentDateTimeUtc(), QString sCountry = QString("ZZ"));

	void Save();

	void PruneOldHosts();
	void PruneByQueryAck();

	inline quint32 size()
	{
		return m_lHosts.size();
	}
	inline bool isEmpty()
	{
		return (size() == 0);
	}
};

extern CHostCache HostCache;

#endif // HOSTCACHE_H
