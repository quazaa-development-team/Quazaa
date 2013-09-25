/*
** hostcache.h
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

#ifndef HOSTCACHE_H
#define HOSTCACHE_H

#include "types.h"
#include <QList>
#include <QMutex>

// Increment this if there have been made changes to the way of storing security rules.
#define HOST_CACHE_CODE_VERSION	6
// History:
// 4 - Initial implementation.
// 6 - Fixed Hosts having an early date and changed time storage from QDateTime to quint32.

class QFile;

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
					 m_tLastQuery = m_tRetryAfter = QDateTime::fromMSecsSinceEpoch(0).toUTC();
		Q_ASSERT(m_tLastConnect.timeSpec() == Qt::UTC);
	}

	bool canQuery(QDateTime tNow = QDateTime::currentDateTimeUtc());
	void setKey(quint32 nKey, CEndPoint* pHost = 0);
};

typedef QList<CHostCacheHost*>::iterator CHostCacheIterator;

class CHostCache
{

public:
	QList<CHostCacheHost*>  m_lHosts;
	mutable QMutex          m_pSection;
	QDateTime               m_tLastSave;

	quint32                 m_nMaxCacheHosts;
	QString                 m_sMessage;

public:
	CHostCache();
	~CHostCache();

	CHostCacheHost* add(CEndPoint host, QDateTime ts = QDateTime::currentDateTimeUtc());

	CHostCacheIterator find(CEndPoint oHost);
	CHostCacheIterator find(CHostCacheHost* pHost);
	inline CHostCacheHost* take(CEndPoint oHost);
	inline CHostCacheHost* take(CHostCacheHost *pHost);

	CHostCacheHost* update(CEndPoint oHost);
	CHostCacheHost* update(CHostCacheHost* pHost);
	CHostCacheHost* update(CHostCacheIterator itHost);

	void remove(CHostCacheHost* pRemove);
	void remove(CEndPoint oHost);

	void addXTry(QString& sHeader);
	QString getXTry();

	void onFailure(CEndPoint addr);
	CHostCacheHost* get();
	CHostCacheHost* getConnectable(QDateTime tNow = QDateTime::currentDateTimeUtc(),
	                               QList<CHostCacheHost*> oExcept = QList<CHostCacheHost*>(),
	                               QString sCountry = QString("ZZ"));

	bool save();
	void load();

	void pruneOldHosts();
	void pruneByQueryAck();

	static quint32 writeToFile(const void * const pManager, QFile& oFile);

	inline quint32 count();
	inline bool isEmpty();
};

CHostCacheHost* CHostCache::take(CEndPoint oHost)
{
	CHostCacheIterator it = find( oHost );
	return it == m_lHosts.end() ? NULL : *it;
}
CHostCacheHost* CHostCache::take(CHostCacheHost *pHost)
{
	CHostCacheIterator it = find( pHost );
	return it == m_lHosts.end() ? NULL : *it;
}

quint32 CHostCache::count()
{
	return m_lHosts.size();
}

bool CHostCache::isEmpty()
{
	return !count();
}

extern CHostCache hostCache;

#endif // HOSTCACHE_H
