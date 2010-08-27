#ifndef SHAREMANAGER_H
#define SHAREMANAGER_H

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
	QSqlDatabase	m_oDatabase;
	bool			m_bActive;
	bool			m_bReady;
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

signals:
	void sharesReady();

public slots:
	void SetupThread();
	void CleanupThread();

protected slots:
	void SyncShares();
};

extern CThread ShareManagerThread;
extern CShareManager ShareManager;
#endif // SHAREMANAGER_H
