#ifndef SHAREMANAGER_H
#define SHAREMANAGER_H

#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "Thread.h"

class CShareManager : public QObject
{
    Q_OBJECT

protected:
	QMutex			m_oSection;
	QWaitCondition	m_oQueryCond;
	QSqlDatabase	m_oDatabase;
	bool			m_bActive;
	bool			m_bReady;

	QList<QSqlRecord> m_lQueryResults;
public:
    explicit CShareManager(QObject *parent = 0);

	void Start();
	void Stop();

	void AddDir(QString sPath);

	void RemoveDir(QString sPath);
	void RemoveDir(quint64 nId);
	void RemoveFile(QString sPath);
	void RemoveFile(quint64 nFileId);

	void ScanFolder(QString sPath, qint64 nParentID = 0);

	bool SharesReady()
	{
		return m_bReady;
	}

	QList<QSqlRecord> Query(const QString sQuery);
signals:
	void sharesReady();
	void executeQuery(const QString& sQuery);

public slots:
	void SetupThread();
	void CleanupThread();

protected slots:
	void SyncShares();
	void execQuery(const QString& sQuery);
};

extern CThread ShareManagerThread;
extern CShareManager ShareManager;
#endif // SHAREMANAGER_H
