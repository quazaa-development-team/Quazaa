/*
** commonfunctions.h
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

#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <iterator>

#include <QReadWriteLock>


namespace OSVersion {
	enum OSVersion{ Linux, Unix, BSD, MacCheetah, MacPuma, MacJaguar, MacPanther, MacTiger, MacLeopard, MacSnowLeopard, Win2000, WinXP, Win2003, WinVista, Win7 };
};

namespace common
{
	void folderOpen(QString file);
	QString formatBytes(quint64 nBytesPerSec);
	QString vendorCodeToName(QString vendorCode);
	OSVersion::OSVersion osVersion();

	// This generates a read/write iterator from a read-only iterator.
	template<class T> inline typename T::iterator getRWIterator(T container, typename T::const_iterator const_it)
	{
		typename T::iterator i = container.begin();
		typename T::const_iterator container_begin_const = container.begin();
		int nDistance = std::distance< typename T::const_iterator >( container_begin_const, const_it );
		std::advance( i, nDistance );
		return i;
	}
}

// Convenience class to make sure the lock state is always well defined while allowing to use timeouts.
// Class can also be used recursively. Plz make sure you don't modify the QReadWriteLock externally
// while using it in this class.
class CTimeoutWriteLocker
{
private:
	QReadWriteLock*	m_pRWLock;
	int				m_nLockCount;

public:
	inline CTimeoutWriteLocker(QReadWriteLock* lock, bool& success, int timeout = -1);
	inline ~CTimeoutWriteLocker();

	inline QReadWriteLock* readWriteLock() const;

	inline bool relock(int timeout = -1);
	inline void unlock();
};

CTimeoutWriteLocker::CTimeoutWriteLocker(QReadWriteLock* lock, bool& success, int timeout) :
	m_pRWLock( lock ),
	m_nLockCount( 0 )
{
	success = lock->tryLockForWrite( timeout );
	m_nLockCount += (int)success;
}

CTimeoutWriteLocker::~CTimeoutWriteLocker()
{
	while ( m_nLockCount )
	{
		unlock();
	}
}

QReadWriteLock* CTimeoutWriteLocker::readWriteLock() const
{
	return m_pRWLock;
}

bool CTimeoutWriteLocker::relock(int timeout)
{
	bool result = m_pRWLock->tryLockForWrite( timeout );
	m_nLockCount += (int)result;
	return result;
}

void CTimeoutWriteLocker::unlock()
{
	m_pRWLock->unlock();
	m_nLockCount--;
}

#endif // COMMONFUNCTIONS_H
