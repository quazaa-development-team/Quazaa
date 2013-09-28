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

// Increment this if there have been made changes to the way of storing Host Cache Hosts.
#define HOST_CACHE_CODE_VERSION	7
// History:
// 4 - Initial implementation.
// 6 - Fixed Hosts having an early date and changed time storage from QDateTime to quint32.
// 7 - Changed host failure counter from quint32 to quint8.

// TODO: in canQuery there are calls to .isNull

class QFile;

class CHostCache : public QObject
{
	Q_OBJECT

public:
	THCLLMap            m_llHosts; // map of (nFailures, QLinkedList);
	                               // QLinkedList sorted by timestamp (descending)

	mutable QMutex      m_pSection;
	mutable quint32     m_tLastSave;

	QString             m_sMessage;

	quint32             m_nSize;

	// Thread used by the Host Cache
	TSharedThreadPtr    m_pHostCacheDiscoveryThread;

public:
	CHostCache();
	~CHostCache();

	void start();

	void add(const CEndPoint host, const quint32 tTimeStamp);

	inline CHostCacheHost* get(const CEndPoint& oHost);
	inline bool check(CHostCacheHost *pHost);

	CHostCacheHost* update(const CEndPoint& oHost,      const quint32 tTimeStamp);
	CHostCacheHost* update(CHostCacheHost* pHost,       const quint32 tTimeStamp);
	CHostCacheHost* update(THostCacheLLIterator itHost, const quint32 tTimeStamp);

	void remove(const CEndPoint& oHost);
	void remove(CHostCacheHost* pRemove);
	void remove(THostCacheLLIterator itHost, const quint8 nFailures);
	void removeWorst(const quint8 nFailures);

	void addXTry(QString& sHeader);
	QString getXTry() const;

	void onFailure(CEndPoint addr);
	CHostCacheHost* getConnectable(QSet<CHostCacheHost*> oExcept = QSet<CHostCacheHost*>(),
	                               QString sCountry = QString("ZZ"));

	bool save(const quint32 tNow) const;

	void pruneOldHosts(const quint32 tNow);
	void pruneByQueryAck(const quint32 tNow);

	static quint32 writeToFile(const void * const pManager, QFile& oFile);

	inline quint32 count() const;
	inline bool isEmpty() const;

public slots:
	CHostCacheHost* addSync(CEndPoint host, const QDateTime& ts, bool bLock = true);
	CHostCacheHost* addSync(CEndPoint host, quint32 tTimeStamp, bool bLock = true);

private:
	THostCacheLLIterator find(const CEndPoint& oHost, quint8& nFailures);
	THostCacheLLIterator find(const CHostCacheHost* const pHost);

	void load();

private slots:
	void asyncStartUpHelper();
	void maintain();
};

CHostCacheHost* CHostCache::get(const CEndPoint& oHost)
{
	quint8 nFailures;
	THostCacheLLIterator it = find( oHost, nFailures );
	return it == m_llHosts.at( nFailures ).end() ? NULL : *it;
}
bool CHostCache::check(CHostCacheHost *pHost)
{
	THostCacheLLIterator it = find( pHost );
	return it != m_llHosts.at( pHost->failures() ).end();
}

quint32 CHostCache::count() const
{
	return m_nSize;
}

bool CHostCache::isEmpty() const
{
	return !m_nSize;
}

extern CHostCache hostCache;

#endif // HOSTCACHE_H
