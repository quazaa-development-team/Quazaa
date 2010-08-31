//
// ShareManager.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef SHAREMANAGER_H
#define SHAREMANAGER_H

#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "thread.h"
#include "SharedFile.h"

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

	void RunHashing();
	void OnFileHashed(CSharedFilePtr pFile);

protected slots:
	void SyncShares();
	void execQuery(const QString& sQuery);
};


extern CThread ShareManagerThread;
extern CShareManager ShareManager;
#endif // SHAREMANAGER_H
