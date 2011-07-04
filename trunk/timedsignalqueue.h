#ifndef TIMEDSIGNALQUEUE_H
#define TIMEDSIGNALQUEUE_H

#include <QBasicTimer>
#include <QDateTime>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QTimerEvent>
#include <QUuid>

#include <queue>

/* -------------------------------------------------------------------------------- */
/* --------------------------------- CTimerObject --------------------------------- */
/* -------------------------------------------------------------------------------- */
// This is a helper class that allows to store signals and related relevant
// information, as well as to emit the signals at a given time.
class CTimerObject
{
private:
	struct
	{
		QObject* obj;
		QString sName;
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
	} m_sSignal;

	QUuid	m_oUUID;
	quint64	m_tTime;
	quint64	m_tInterval;
	bool	m_bMultiShot;

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

	void resetTime();
	bool emitSignal() const;

	friend class CTimedSignalQueue;
};

/* -------------------------------------------------------------------------------- */
/* ---------------------------- CGlobalTimedSignalQueue --------------------------- */
/* -------------------------------------------------------------------------------- */
class CTimedSignalQueue : public QObject
{
    Q_OBJECT

private:
	typedef std::pair< quint64, CTimerObject* > CTimerPair;
	typedef std::priority_queue< CTimerPair, std::deque<CTimerPair>, std::less<CTimerPair> > CSignalQueue;
	typedef QList< CTimerObject* > CSignalList;

	static QDateTime	m_oTime; // todo: handle system clock changes...
	QBasicTimer			m_oTimer;
	CSignalList			m_Signals;
	QMutex				m_pSection;
	quint64				m_nPrecision;
	CSignalQueue		m_QueuedSignals;

public:
	explicit CTimedSignalQueue(QObject *parent = 0);
	~CTimedSignalQueue();

	// (Re)initializes internal structures. Note that this does NOT clear the queue.
	void setup();

	// Clears the queue and removes all scheduled items.
	void clear();

	// Sets the interval used by the queue to check for new signals to be scheduled. This defaults to 1000ms.
	void setPrecision(quint64 tInterval);

protected:
	void timerEvent(QTimerEvent* event);

private:
	inline static quint64 getTimeInMs() { return m_oTime.toMSecsSinceEpoch(); }
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

	// This schedules a signal to be invoqued once at a given schedule time tSchedule.
	QUuid push(QObject* parent, const char* signal, quint32 tSchedule,
			   QGenericArgument val0 = QGenericArgument(), QGenericArgument val1 = QGenericArgument(),
			   QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
			   QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
			   QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
			   QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());

	// Removes all scheduled combinations of a given parent and signal/slot from the queue.
	bool pop(const QObject* parent, const char* signal);

	// Removes a scheduled parent + signal/slot combination by its UUID.
	bool pop(QUuid oTimer_ID);

	// Sets the interval time of a given parent + signal/slot combination to tInterval. After this call,
	// the next schedule time of that combination is in tInterval milliseconds, no matter the timing state
	// of the previously scheduled combination.
	bool setInterval(QUuid oTimer_ID, quint64 tInterval);

	friend class CTimerObject;
};

extern CTimedSignalQueue signalQueue;

#endif // TIMEDSIGNALQUEUE_H
