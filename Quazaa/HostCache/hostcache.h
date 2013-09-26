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

#include <QMutex>

#include "hostcachehost.h"

// Increment this if there have been made changes to the way of storing security rules.
#define HOST_CACHE_CODE_VERSION	6
// History:
// 4 - Initial implementation.
// 6 - Fixed Hosts having an early date and changed time storage from QDateTime to quint32.

class QFile;

typedef QList<CHostCacheHost*>::iterator CHostCacheIterator;

class CHostCache
{

public:
	QList<CHostCacheHost*>  m_lHosts;
	mutable QMutex          m_pSection;
	quint32                 m_tLastSave;

	quint32                 m_nMaxCacheHosts;
	QString                 m_sMessage;

public:
	CHostCache();
	~CHostCache();

	CHostCacheHost* add(CEndPoint host, const QDateTime& ts);
	CHostCacheHost* add(CEndPoint host, quint32 tTimeStamp);

	CHostCacheIterator find(CEndPoint oHost);
	CHostCacheIterator find(CHostCacheHost* pHost);
	inline CHostCacheHost* take(CEndPoint oHost);
	inline CHostCacheHost* take(CHostCacheHost *pHost);

	CHostCacheHost* update(CEndPoint oHost, const quint32 tTimeStamp);
	CHostCacheHost* update(CHostCacheHost* pHost, const quint32 tTimeStamp);
	CHostCacheHost* update(CHostCacheIterator itHost, const quint32 tTimeStamp);

	void remove(CHostCacheHost* pRemove);
	void remove(CEndPoint oHost);

	void addXTry(QString& sHeader);
	QString getXTry();

	void onFailure(CEndPoint addr);
	CHostCacheHost* get();
	CHostCacheHost* getConnectable(const quint32 tNow = common::getTNowUTC(),
	                               QList<CHostCacheHost*> oExcept = QList<CHostCacheHost*>(),
	                               QString sCountry = QString("ZZ"));

	bool save(const quint32 tNow);
	void load();

	void pruneOldHosts(const quint32 tNow);
	void pruneByQueryAck(const quint32 tNow);

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
