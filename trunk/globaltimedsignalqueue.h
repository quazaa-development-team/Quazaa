#ifndef GLOBALTIMEDSIGNALQUEUE_H
#define GLOBALTIMEDSIGNALQUEUE_H

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QUuid>

#include <queue>

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

	quint64	m_tTime;
	quint64	m_tInterval;
	bool	m_bMultiShot;

	QUuid	m_oUUID;

	CTimerObject(QObject* obj, const char* member, quint64 tInterval, bool bMultiShot = true,
				 QGenericArgument val0 = QGenericArgument(), QGenericArgument val1 = QGenericArgument(),
				 QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
				 QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
				 QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
				 QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());
	CTimerObject(const CTimerObject* const pTimerObject);

	void resetTime();
	bool emitSignal() const;

public:
	/*inline bool operator<(const CTimerObject& tObject) const
	{
		return this->m_tTime < tObject.m_tTime;
	}*/

	friend class CGlobalTimedSignalQueue;
};


class CGlobalTimedSignalQueue : public QObject
{
    Q_OBJECT
private:
	typedef std::pair< quint64, CTimerObject* > CTimerPair;
	typedef std::priority_queue< CTimerPair, std::deque<CTimerPair>, std::less<CTimerPair> > CSignalQueue;
	typedef QList< CTimerObject* > CSignalList;

	CSignalQueue m_QueuedSignals;
	CSignalList m_Signals;

	// todo: handle system clock changes...
	QTimer*			m_pTimer;
	static QDateTime m_oTime;

public:
    explicit CGlobalTimedSignalQueue(QObject *parent = 0);
	~CGlobalTimedSignalQueue();

	void setup();
	void clear();

private:
	inline static quint64 getTimeInMs() { return m_oTime.toMSecsSinceEpoch(); }
	QUuid push(CTimerObject* pTimedSignal);
signals:

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
