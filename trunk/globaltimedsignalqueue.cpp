#ifdef _DEBUG
#include <QSignalSpy>
#endif // _DEBUG

#include "globaltimedsignalqueue.h"

CGlobalTimedSignalQueue signalQueue;

/* -------------------------------------------------------------------------------- */
/* --------------------------------- CTimerObject --------------------------------- */
/* -------------------------------------------------------------------------------- */

CTimerObject::CTimerObject(QObject* obj, const char* member, quint64 tInterval, bool bMultiShot,
						   QGenericArgument val0, QGenericArgument val1,
						   QGenericArgument val2, QGenericArgument val3,
						   QGenericArgument val4, QGenericArgument val5,
						   QGenericArgument val6, QGenericArgument val7,
						   QGenericArgument val8, QGenericArgument val9) :
	m_tInterval( tInterval ),
	m_bMultiShot( bMultiShot )
{
	resetTime();

	m_sSignal.obj = obj;
	m_sSignal.sName = member;
	m_sSignal.val0 = val0;
	m_sSignal.val1 = val1;
	m_sSignal.val2 = val2;
	m_sSignal.val3 = val3;
	m_sSignal.val4 = val4;
	m_sSignal.val5 = val5;
	m_sSignal.val6 = val6;
	m_sSignal.val7 = val7;
	m_sSignal.val8 = val8;
	m_sSignal.val9 = val9;

	m_oUUID = QUuid::createUuid();
}

CTimerObject::CTimerObject(const CTimerObject* const pTimerObject)
{
	m_tTime			= pTimerObject->m_tTime;
	m_tInterval		= pTimerObject->m_tInterval;
	m_bMultiShot	= pTimerObject->m_bMultiShot;

	m_oUUID			= pTimerObject->m_oUUID;

	m_sSignal.obj	= pTimerObject->m_sSignal.obj;
	m_sSignal.sName = pTimerObject->m_sSignal.sName;
	m_sSignal.val0	= pTimerObject->m_sSignal.val0;
	m_sSignal.val1	= pTimerObject->m_sSignal.val1;
	m_sSignal.val2	= pTimerObject->m_sSignal.val2;
	m_sSignal.val3	= pTimerObject->m_sSignal.val3;
	m_sSignal.val4	= pTimerObject->m_sSignal.val4;
	m_sSignal.val5	= pTimerObject->m_sSignal.val5;
	m_sSignal.val6	= pTimerObject->m_sSignal.val6;
	m_sSignal.val7	= pTimerObject->m_sSignal.val7;
	m_sSignal.val8	= pTimerObject->m_sSignal.val8;
	m_sSignal.val9	= pTimerObject->m_sSignal.val9;
}

void CTimerObject::resetTime()
{
	m_tTime = CGlobalTimedSignalQueue::getTimeInMs() + m_tInterval;
}

bool CTimerObject::emitSignal() const
{
	return QMetaObject::invokeMethod( m_sSignal.obj, m_sSignal.sName.toLatin1().data(), Qt::QueuedConnection,
									  m_sSignal.val0, m_sSignal.val1, m_sSignal.val2,
									  m_sSignal.val3, m_sSignal.val4, m_sSignal.val5,
									  m_sSignal.val6, m_sSignal.val7, m_sSignal.val8,
									  m_sSignal.val9 );
}

/* -------------------------------------------------------------------------------- */
/* ---------------------------- CGlobalTimedSignalQueue --------------------------- */
/* -------------------------------------------------------------------------------- */

QDateTime CGlobalTimedSignalQueue::m_oTime = QDateTime::currentDateTime();

CGlobalTimedSignalQueue::CGlobalTimedSignalQueue(QObject *parent) :
	QObject( parent ),
	m_nPrecision( 1000 )
{
	setup();
}

CGlobalTimedSignalQueue::~CGlobalTimedSignalQueue()
{
	clear();
}

void CGlobalTimedSignalQueue::setup()
{
	QMutexLocker l( &m_pSection );

	m_oTimer.start( m_nPrecision, this );
}

void CGlobalTimedSignalQueue::clear()
{
	QMutexLocker l( &m_pSection );

	while( !m_QueuedSignals.empty() )
	{
		delete m_QueuedSignals.top().second;
		m_QueuedSignals.pop();
	}

	m_Signals.clear();
}

void CGlobalTimedSignalQueue::setPrecision( quint64 tInterval )
{
	if ( tInterval )
	{
		QMutexLocker l( &m_pSection );

		m_nPrecision = tInterval;
		m_oTimer.start( m_nPrecision, this );
	}
}

void CGlobalTimedSignalQueue::timerEvent(QTimerEvent* event)
{
	if ( event->timerId() == m_oTimer.timerId() )
	{
		checkSchedule();
	}
	else
	{
		QObject::timerEvent( event );
	}
}

void CGlobalTimedSignalQueue::checkSchedule()
{
	if ( m_QueuedSignals.empty() )
		return;

	QMutexLocker mutex( &m_pSection );

	CTimerPair oTimedSignal = m_QueuedSignals.top();

	if ( oTimedSignal.first <= getTimeInMs() )
	{
		if ( oTimedSignal.second != NULL )
		{
			// If emitting the signal was unsuccessful, we're not going to send it again.
			bool bSuccess = oTimedSignal.second->emitSignal();

			if ( oTimedSignal.second->m_bMultiShot && bSuccess )
			{
				oTimedSignal.second->resetTime();
				m_QueuedSignals.push( oTimedSignal );
			}
			else
			{
				delete oTimedSignal.second;
				oTimedSignal.second = NULL;
			}
		}
		m_QueuedSignals.pop();

		mutex.unlock();
		checkSchedule();
	}
}

QUuid CGlobalTimedSignalQueue::push(QObject* parent, const char* signal, quint64 tInterval, bool bMultiShot,
									QGenericArgument val0, QGenericArgument val1,
									QGenericArgument val2, QGenericArgument val3,
									QGenericArgument val4, QGenericArgument val5,
									QGenericArgument val6, QGenericArgument val7,
									QGenericArgument val8, QGenericArgument val9)
{
#ifdef _DEBUG
	QSignalSpy managedSignal( parent, signal );
	Q_ASSERT_X( managedSignal.isValid(), "CGlobalTimedSignalQueue::push()",
				"An invalid signal has been requested to be stored." );
#endif // _DEBUG

	return push( new CTimerObject( parent, signal, tInterval, bMultiShot,
								   val0, val1, val2, val3, val4, val5, val6, val7, val8, val9 ) );
}

QUuid CGlobalTimedSignalQueue::push(CTimerObject* pTimedSignal)
{
	QMutexLocker l( &m_pSection );

	CTimerPair oPair = CTimerPair( pTimedSignal->m_tTime, pTimedSignal );
	m_QueuedSignals.push( oPair );
	m_Signals.append( oPair.second );

	return oPair.second->m_oUUID;
}

bool CGlobalTimedSignalQueue::pop(const QObject* parent, const char* signal)
{
	if ( !parent || !signal )
		return false;

	QString sSignalName = signal;
	bool bFound = false;

	QMutexLocker l( &m_pSection );

	for ( CSignalList::iterator i = m_Signals.begin(); i != m_Signals.end(); ++i )
	{
		if ( (*i)->m_sSignal.obj == parent && (*i)->m_sSignal.sName == sSignalName )
		{
			CTimerObject* tmp = *i;
			delete tmp;
			tmp = NULL;

			i = m_Signals.erase( i )--;
			bFound = true;
		}
	}

	return bFound;
}

bool CGlobalTimedSignalQueue::pop(QUuid oTimer_ID)
{
	QMutexLocker l( &m_pSection );

	CSignalList::const_iterator i = m_Signals.begin();
	while ( i != m_Signals.end() )
	{
		if ( (*i)->m_oUUID == oTimer_ID )
		{
			CTimerObject* tmp = *i;
			delete tmp;
			tmp = NULL;

			return true;
		}
		++i;
	}
	return false;
}

bool CGlobalTimedSignalQueue::setInterval(QUuid oTimer_ID, quint64 tInterval)
{
	QMutexLocker mutex( &m_pSection );

	CSignalList::const_iterator i = m_Signals.begin();
	while ( i != m_Signals.end() )
	{
		if ( (*i)->m_oUUID == oTimer_ID )
		{
			CTimerObject* tmp = *i;
			CTimerObject* pTimerCopy = new CTimerObject( tmp );
			delete tmp;
			tmp = NULL;

			mutex.unlock();

			pTimerCopy->m_tInterval = tInterval;
			pTimerCopy->resetTime();

			push( pTimerCopy );

			return true;
		}
		++i;
	}
	return false;
}

