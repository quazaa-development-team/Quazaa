#ifndef GLOBALTIMEDSIGNALQUEUE_H
#define GLOBALTIMEDSIGNALQUEUE_H

#include <QBasicTimer>
#include <QDateTime>
#include <QList>
#include <QMutexLocker>
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
	CTimerObject(QObject* obj, const char* member, quint64 tInterval, bool bMultiShot = true,
				 QGenericArgument val0 = QGenericArgument(), QGenericArgument val1 = QGenericArgument(),
				 QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
				 QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
				 QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
				 QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());
	CTimerObject(const CTimerObject* const pTimerObject);

	void resetTime();
	bool emitSignal() const;

	friend class CGlobalTimedSignalQueue;
};

/* -------------------------------------------------------------------------------- */
/* ---------------------------- CGlobalTimedSignalQueue --------------------------- */
/* -------------------------------------------------------------------------------- */
class CGlobalTimedSignalQueue : public QObject
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
    explicit CGlobalTimedSignalQueue(QObject *parent = 0);
	~CGlobalTimedSignalQueue();

	void setup();
	void clear();
	void setPrecision(quint64 tInterval);

protected:
	void timerEvent(QTimerEvent* event);

private:
	inline static quint64 getTimeInMs() { return m_oTime.toMSecsSinceEpoch(); }
	QUuid push(CTimerObject* pTimedSignal);

public slots:
	void checkSchedule();

	QUuid push(QObject* parent, const char* signal, quint64 tInterval, bool bMultiShot = true,
			  QGenericArgument val0 = QGenericArgument(), QGenericArgument val1 = QGenericArgument(),
			  QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
			  QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
			  QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
			  QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());

	bool pop(const QObject* parent, const char* signal);
	bool pop(QUuid oTimer_ID);

	bool setInterval(QUuid oTimer_ID, quint64 tInterval);

	friend class CTimerObject;
};

extern CGlobalTimedSignalQueue SignalQueue;

#endif // GLOBALTIMEDSIGNALQUEUE_H
