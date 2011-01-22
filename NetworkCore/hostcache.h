//
// hostcache.h
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
	quint32         m_tTimestamp;   // Kiedy ostatnio widziany

	quint32         m_nQueryKey;    // QK
	CEndPoint       m_nKeyHost;     // host dla ktorego jest QK
	quint32         m_nKeyTime;     // kiedy odebrano OK?

	quint32         m_tAck;         // czas ostatniej operacji wymagajacej potwierdzenia

	quint32         m_tLastQuery;   // kiedy poslano ostatnie zapytanie?
	quint32         m_tRetryAfter;  // kiedy mozna ponowic?
	quint32         m_tLastConnect; // kiedy ostatnio sie polaczylismy?

public:
	CHostCacheHost()
	{
		m_tTimestamp = 0;

		m_nQueryKey = 0;
		m_nKeyTime = 0;

		m_tRetryAfter = 0;
		m_tAck = 0;

		m_tLastQuery = 0;
		m_tLastConnect = 0;
	}

	bool CanQuery(quint32 tNow = 0)
	{
		if(tNow == 0)
		{
			tNow = time(0);
		}

		if(m_tAck != 0)
		{
			return false;
		}

		if(tNow - m_tTimestamp > 3600)
		{
			return false;
		}

		if(m_tRetryAfter != 0 && tNow < m_tRetryAfter)
		{
			return false;
		}

		if(m_tLastQuery == 0)
		{
			return true;
		}

		return (tNow - m_tLastQuery) >= 120;
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

	CHostCacheHost* Add(CEndPoint host, quint32 ts);
	void AddXTry(QString& sHeader);
	QString GetXTry();
	void Update(CEndPoint oHost);
	void Update(CHostCacheHost* pHost);
	void Remove(CHostCacheHost* pRemove);
	CHostCacheHost* Find(CEndPoint oHost);
	void OnFailure(CEndPoint addr);
	CHostCacheHost* Get();
	CHostCacheHost* GetConnectable(quint32 tNow = 0, QString sCountry = QString("ZZ"));

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
