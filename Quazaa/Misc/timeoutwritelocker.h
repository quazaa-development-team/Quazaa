#ifndef TIMEOUTWRITELOCKER_H
#define TIMEOUTWRITELOCKER_H

#include <QReadWriteLock>

/**
 * Convenience class to make sure the lock state is always well defined while allowing to use
 * timeouts. This Class can also be used recursively. Plz make sure you don't modify the
 * QReadWriteLock externally while using it in this class, as this will result in undefined
 * behavior.
 */
class CTimeoutWriteLocker
{
private:
	QReadWriteLock*	m_pRWLock;
	quint32				m_nLockCount;

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
	m_nLockCount += (quint32)success;
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
	m_nLockCount += (quint32)result;
	return result;
}

void CTimeoutWriteLocker::unlock()
{
	m_pRWLock->unlock();
	--m_nLockCount;
}

#endif // TIMEOUTWRITELOCKER_H
