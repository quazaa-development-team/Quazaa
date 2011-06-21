/*
** sharemanager.h
**
** Copyright © Quazaaa Development Team, 2009-2011.
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

#ifndef SHAREMANAGER_H
#define SHAREMANAGER_H

#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "thread.h"
#include "sharedfile.h"

class CQueryHashTable;

class CShareManager : public QObject
{
	Q_OBJECT

public:
	QMutex			m_oSection;
protected:
	QWaitCondition	m_oQueryCond;
	QSqlDatabase	m_oDatabase;
	bool			m_bActive;
	bool			m_bReady;

	QList<QSqlRecord> m_lQueryResults;

	CQueryHashTable* 	m_pTable;
	bool				m_bTableReady;
public:
	explicit CShareManager(QObject* parent = 0);

	void Start();
	void Stop();

	void AddDir(QString sPath);

	void RemoveDir(QString sPath);
	void RemoveDir(quint64 nId);
	void RemoveFile(QString sPath);
	void RemoveFile(quint64 nFileId);

	void ScanFolder(QString sPath, qint64 nParentID = 0);

	CQueryHashTable* GetHashTable();

	bool SharesReady()
	{
		return m_bReady;
	}

	QList<QSqlRecord> Query(const QString sQuery);

protected:
	void BuildHashTable();
signals:
	void sharesReady();
	void executeQuery(const QString& sQuery);

signals:
	void hasherStarted(int); // int - hasher id
	void hasherFinished(int); // int - hasher id
	void hashingProgress(int, QString, double, int); // hasher id, filename, percent, rate

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
