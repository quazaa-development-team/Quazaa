#ifndef TIMEDSIGNALQUEUE_H
#define TIMEDSIGNALQUEUE_H

#include <QElapsedTimer>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QDateTime>
#include <QMultiMap>
#include <QMutex>
#include <QUuid>

#define ENABLE_SIGNAL_QUEUE_DEBUGGING 1

class CTimedSignalQueue;

/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------- CTimerObject ---------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */
/**
 * @brief The CTimerObject class is a helper class that allows to store signals and related relevant
 * information, as well as to emit the signals at a given time.
 */
class CTimerObject
{
private:
	struct
	{
		QObject* obj;
		const char* sName;
		QGenericArgument val0;
		QGenericArgument val1;
		QGenericArgument val2;
		QGenericArgument val3;
		QGenericArgument val4;
		QGenericArgument val5;
		QGenericArgument val6;
		QGenericArgument val7;
		QGenericArgument val8;
		QGenericArgument val9;
	} m_sSignal; // String based Qt4 signal

	QUuid   m_oUUID;
	quint64 m_tTime;      // relative time in ms as provided by getRelativeTimeInMs()
	quint64 m_tInterval;  // repetition interval in ms
	bool    m_bMultiShot; // repeat after m_tInterval yes/no

private:
	CTimerObject(QObject* obj, const char* member, quint64 tInterval, bool bMultiShot,
	             QGenericArgument val0 = QGenericArgument(), QGenericArgument val1 = QGenericArgument(),
	             QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
	             QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
	             QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
	             QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());

	CTimerObject(QObject* obj, const char* member, quint32 tSchedule,
	             QGenericArgument val0 = QGenericArgument(), QGenericArgument val1 = QGenericArgument(),
	             QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
	             QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
	             QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
	             QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());

	CTimerObject(const CTimerObject* const pTimerObject);

	~CTimerObject();

	void resetTime();
	bool emitSignal() const;

	friend class CTimedSignalQueue;
};

/* ---------------------------------------------------------------------------------------------- */
/* -------------------------------------- CTimedSignalQueue ------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */
class CTimedSignalQueue : public QObject
{
	Q_OBJECT

private:
	// Key: The relative time as provided by getRelativeTimeInMs()
	typedef QMultiMap<quint64, CTimerObject*>           TSignalQueue;
	typedef QMultiMap<quint64, CTimerObject*>::iterator TSignalQueueIterator;

	static QElapsedTimer m_oTime;                // the relative time since the timer was started
	static quint64       m_tTimerStartUTCInMSec; // ms since 1970-01-01T00:00:00 UTC (timer start)

	QBasicTimer			m_oTimer;                // for timer events
	QMutex				m_pSection;
	quint64				m_nPrecision;            // the interval in ms between two timer events
	TSignalQueue		m_QueuedSignals;         // the queued signals

	// Before deleting the timed signal queue object, this lock is aquired. If you have other global
	// objects depending on this component, you can aquire this lock to prevent deletion until
	// you're finished with whatever you're doing.
	QMutex				m_pShutdownLock;

public:
	explicit CTimedSignalQueue(QObject* parent = NULL);
	~CTimedSignalQueue();

	// Hold this lock to delay the deletion of the queue on application shutdown.
	void shutdownLock();
	void shutdownUnlock();

	// (Re)initializes internal structures. Note that this does NOT clear the queue.
	void setup();

	void stop();

	// Clears the queue and removes all scheduled items.
	void clear();

	// Sets the interval used by the queue to check for new signals to be scheduled. This defaults to 1000ms.
	void setPrecision(quint64 tInterval = 1000);

protected:
	void timerEvent(QTimerEvent* event);

private:
	inline static quint64 getRelativeTimeInMs()
	{
		if( !m_oTime.isValid() )
		{
			m_tTimerStartUTCInMSec = (quint64)( QDateTime::currentDateTimeUtc().toTime_t() ) * 1000;
			m_oTime.start();
		}

		return m_oTime.elapsed();
	}
	QUuid push(CTimerObject* pTimedSignal);

public slots:
	// Allows to manually check for new scheduled items in the queue.
	void checkSchedule();

	// This schedules a signal or slot to be invoqued after an interval of tInterval milliseconds.
	// If multiShot is set to true, the slot will be invoqued in the given interval until the signal queue
	// recieves a pop() request for the signal or the given parent turns invalid.
	QUuid push(QObject* parent, const char* signal, quint64 tInterval, bool multiShot,
	           QGenericArgument val0 = QGenericArgument(), QGenericArgument val1 = QGenericArgument(),
	           QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
	           QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
	           QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
	           QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());

	// This schedules a signal to be invoqued once at a given schedule time tSchedule (UTC).
	QUuid push(QObject* parent, const char* signal, quint32 tSchedule,
	           QGenericArgument val0 = QGenericArgument(), QGenericArgument val1 = QGenericArgument(),
	           QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
	           QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
	           QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
	           QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());

	// Removes all scheduled combinations of a given parent and signal/slot from the queue.
	// If no signal/slot is specified, all entries for the given parent are removed.
	bool pop(const QObject* parent, const char* signal = NULL);

	// Removes a scheduled parent + signal/slot combination by its UUID.
	bool pop(QUuid oTimer_ID);

	// Sets the interval time of a given parent + signal/slot combination to tInterval. After this call,
	// the next schedule time of that combination is in tInterval milliseconds, no matter the timing state
	// of the previously scheduled combination.
	bool setInterval(QUuid oTimer_ID, quint64 tInterval);

private:
	CTimerObject* popInternal(QUuid oTimer_ID);

	friend class CTimerObject;
};

extern CTimedSignalQueue signalQueue;

#endif // TIMEDSIGNALQUEUE_H
