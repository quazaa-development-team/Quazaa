//
// ShareManager.cpp
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

#include "ShareManager.h"

#include <QTimer>
#include <QSqlError>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QVariant>
#include <QList>

#include "quazaasettings.h"
#include "queryhashmaster.h"
#include "SharedFile.h"
#include "FileHasher.h"
#include "types.h"

CThread ShareManagerThread;
CShareManager ShareManager;

CShareManager::CShareManager(QObject *parent) :
	QObject(parent)
{
	m_bActive = false;
	m_bReady = false;
	m_bTableReady = false;
	m_pTable = 0;
}

void CShareManager::Start()
{
	QMutexLocker l(&m_oSection);
	qDebug() << "Starting Share Manager...";
	ShareManagerThread.setObjectName("ShareManager Thread");
	connect(this, SIGNAL(sharesReady()), &QueryHashMaster, SLOT(Build()));
	ShareManagerThread.start(&m_oSection, this);
}

void CShareManager::SetupThread()
{
	qDebug() << "Setting up ShareManager thread";
	m_oDatabase = QSqlDatabase::addDatabase("QSQLITE", "Shares");
	m_oDatabase.setDatabaseName("shares.sdb");

	if( !m_oDatabase.open() )
	{
		qWarning() << "Failed to open SQLITE database. Giving up. " << m_oDatabase.lastError().text();
		QTimer::singleShot(1000, &ShareManagerThread, SLOT(quit()));
		return;
	}

	qDebug() << "Database opened successfully.";

	QSqlQuery query(m_oDatabase);

	qDebug() << "Checking tables...";

	// TODO: Better checks and error handling

	if( !m_oDatabase.tables().contains("dirs") || !m_oDatabase.tables().contains("files") || !m_oDatabase.tables().contains("hashes") || !m_oDatabase.tables().contains("hash_queue") )
	{
		qDebug() << "Database is corrupted. Recreating...";

		query.exec("PRAGMA legacy_file_format = 0");

		foreach(QString sTable, m_oDatabase.tables())
		{
			query.exec(QString("DROP TABLE `%1`").arg(sTable));
		}

		// tables
		query.exec("CREATE TABLE 'dirs' ('id' INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , 'path' TEXT NOT NULL, 'parent' INTEGER NOT NULL );");
		query.exec("CREATE TABLE 'files' ('file_id' INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , 'dir_id' INTEGER NOT NULL , 'name' VARCHAR(255) NOT NULL , 'size' INTEGER NOT NULL , 'last_modified' INTEGER NOT NULL , 'shared' BOOL NOT NULL  DEFAULT 1);");
		query.exec("CREATE TABLE 'hashes' ('file_id' INTEGER NOT NULL  UNIQUE , 'sha1' BLOB(20) NOT NULL );");
		query.exec("CREATE TABLE 'hash_queue' ('dir_id' INTEGER NOT NULL, 'filename' VARCHAR(255) NOT NULL);");
		query.exec("CREATE TABLE 'keywords' ('id' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, 'keyword' TEXT NOT NULL);");

		// indexes
		query.exec("CREATE INDEX 'dir_id' ON 'files' ('dir_id' ASC);");
		query.exec("CREATE UNIQUE INDEX 'file_id' ON 'hashes' ('file_id' ASC);");
		query.exec("CREATE INDEX 'name' ON 'files' ('name' ASC);");
		query.exec("CREATE INDEX 'parent' ON 'dirs' ('parent' ASC)");
		query.exec("CREATE UNIQUE INDEX 'keyword' ON 'keywords' ('keyword' ASC);");
		query.exec("CREATE INDEX 'sha1' ON 'hashes' ('sha1' ASC);");

		qDebug() << "Database recreated.";
	}
	else
	{
		qDebug() << "Tables OK";
	}

	qDebug() << "Destroying hash queue.";
	query.exec("DELETE FROM `hash_queue`;");

	m_bActive = true;

	QTimer::singleShot(30000, this, SLOT(SyncShares()));

	connect(this, SIGNAL(executeQuery(const QString&)), this, SLOT(execQuery(const QString&)), Qt::QueuedConnection);
}

void CShareManager::Stop()
{
	QMutexLocker l(&m_oSection);
	m_bActive = false;
	m_bReady = false;
	m_bTableReady = false;
	if( m_pTable )
	{
		delete m_pTable;
		m_pTable = 0;
	}
	disconnect(SIGNAL(executeQuery(const QString&)), this, SLOT(execQuery(const QString&)));
	ShareManagerThread.exit(0);
}

void CShareManager::CleanupThread()
{
	qDebug() << "ShareManager: cleaning up.";

	if( m_oDatabase.isOpen() )
	{
		qDebug() << "Closing Database connection.";
		QSqlDatabase::removeDatabase("Shares");
		m_oDatabase.close();
	}
}

void CShareManager::AddDir(QString sPath)
{
	Q_UNUSED(sPath);

}

void CShareManager::RemoveDir(QString sPath)
{
	Q_UNUSED(sPath);

	QSqlQuery query(m_oDatabase);

	query.prepare("SELECT id FROM dirs WHERE path = '?'");
	query.addBindValue(QVariant(sPath));
	query.exec();

	while( query.next() )
	{
		RemoveDir(query.record().value(0).toUInt());
	}
}

void CShareManager::RemoveDir(quint64 nId)
{
	QSqlQuery delq(m_oDatabase);

	delq.exec(QString("SELECT id FROM dirs WHERE parent = %1").arg(nId));
	while( delq.next() )
	{
		RemoveDir(delq.record().value(0).toUInt());
	}

	delq.exec(QString("DELETE FROM hashes WHERE file_id IN (SELECT dir_id FROM files WHERE dir_id = %1)").arg(nId));
	delq.exec(QString("DELETE FROM files WHERE dir_id = %1").arg(nId));
	delq.exec(QString("DELETE FROM dirs WHERE id = %1").arg(nId));
}

void CShareManager::RemoveFile(QString sPath)
{
	Q_UNUSED(sPath);

}

void CShareManager::RemoveFile(quint64 nFileId)
{
	QSqlQuery delq(m_oDatabase);
	delq.exec(QString("DELETE FROM hashes WHERE file_id = %1").arg(nFileId));
	delq.exec(QString("DELETE FROM files WHERE file_id = %1").arg(nFileId));
}

void CShareManager::SyncShares()
{
	QMutexLocker l(&m_oSection);

	qDebug() << "Syncing Shares...";

	QSqlQuery query(m_oDatabase);

	query.exec("PRAGMA synchronous = 0");
	query.exec("PRAGMA temp_store = 2");

	query.setForwardOnly(true);

	int nMissingDirs = 0, nMissingFiles = 0, nModifiedFiles = 0;

	// 1. Check for missing dirs
	if( !query.exec("SELECT id, path FROM dirs") )
	{
		qDebug() << "SQL Query failed: " << query.lastError().text();
		return;
	}

	while( query.next() )
	{
		QDir d(query.record().value(1).toString());
		if( !d.exists() )
		{
			// delete all file entries that refer to the missing dir
			qDebug() << "Directory " << d.path() << "does not exist";
			RemoveDir(query.record().value(0).toInt());
			nMissingDirs++;
		}
	}

	// 2. Check for missing or modified files
	query.setForwardOnly(true);
	if( !query.exec("SELECT id, path FROM dirs") )
	{
		qDebug() << "SQL Query failed: " << query.lastError().text();
		return;
	}

	while( query.next() )
	{
		QSqlQuery fquery(m_oDatabase);

		if( fquery.exec(QString("SELECT file_id,name,size,last_modified FROM files WHERE dir_id = %1").arg(query.record().value(0).toInt())) )
		{
			while( fquery.next() )
			{
				QString sPath;
				sPath = query.record().value(1).toString().append("/");
				sPath.append(fquery.record().value(1).toString());

				QFileInfo fi(sPath);
				if( !fi.exists() )
				{
					qDebug() << "File:" << sPath.toAscii() << " is missing";
					RemoveFile(fquery.record().value(0).toInt());
					nMissingFiles++;
				}
				else
				{
					// check if modified

					bool bModified = false;

					if( fi.size() != fquery.record().value(2).toLongLong() )
					{
						qDebug() << "Size mismatch";
						bModified = true;
					}
					else if( fi.lastModified().toTime_t() != fquery.record().value(3).toUInt() )
					{
						qDebug() << "Modified recently";
						bModified = true;
					}

					if( bModified )
					{
						qDebug() << "File:" << sPath.toAscii() << " is modified, rehashing";
						RemoveFile(fquery.record().value(0).toInt());
						nModifiedFiles++;
					}
				}
			}
		}
	}

	// 3. Remove non-shared dirs (just in case)

	QList<qint64> lSharedDirs;

	// for each dir from settings
	foreach( QString sPath, quazaaSettings.Library.Shares )
	{
		// find all subdirs and store their IDs in the list
		query.prepare("SELECT id FROM dirs WHERE path LIKE ?");
		query.bindValue(0, QVariant(sPath.append("%")));

		if( !query.exec() )
		{
			qDebug() << "SQL Query failed: " << query.lastError().text() << query.lastError().number() << query.executedQuery();
		}

		while( query.next() )
		{
			lSharedDirs.append(query.record().value(0).toLongLong());
		}
	}

	// now find and remove orphan dirs
	QString sIds;
	foreach( qint64 nId, lSharedDirs )
	{
		sIds.append(QVariant(nId).toString()).append(",");
	}
	if( sIds.size() )
	{
		sIds.truncate(sIds.size() - 1);

		qDebug() << "Dir IDs: " << sIds;

		if( !query.exec(QString("SELECT id FROM dirs WHERE id NOT IN(%1)").arg(sIds) ) )
		{
			qDebug() << "SQL Query failed: " << query.lastError().text();
		}
		else
		{
			int nOrphaned = 0;

			while( query.next() )
			{
				nOrphaned++;
				RemoveDir(query.record().value(0).toLongLong());
			}

			qDebug() << "Found " << nOrphaned << "orphaned dirs";
		}
	}

	// 4. Make sure that shared dirs are in db (quick and dirty, possibly bad :P)
	foreach( QString sPath, quazaaSettings.Library.Shares )
	{
		query.prepare("SELECT id FROM dirs WHERE path LIKE ?");
		query.bindValue(0, QVariant(sPath));
		if( !query.exec() )
		{
			qDebug() << "SQL Query failed: " << query.lastError().text();
			continue;
		}

		bool bFound = false;
		while( query.next() )
		{
			bFound = true;
		}

		if( !bFound )
		{
			QDir d(sPath);

			if( !d.exists() )
			{
				// Try to create the dir
				if( !d.mkpath(sPath) )
				{
					qDebug() << "Cannot create directory " << sPath;
					continue;
				}
			}

			QSqlQuery insq(m_oDatabase);
			insq.prepare("INSERT INTO dirs (path, parent) VALUES(?,0)");
			insq.bindValue(0, QVariant(sPath));
			if( !insq.exec() )
			{
				qDebug() << "Cannot insert new directory entry: " << insq.lastError().text();
			}
		}
	}

	qDebug() << "Missing dirs:" << nMissingDirs << "missing files:" << nMissingFiles << "modified files:" << nModifiedFiles;

	// 5. Now we can start scanning shared dirs

	foreach( QString sPath, quazaaSettings.Library.Shares )
	{
		if( !m_bActive )
			break;

		l.unlock();
		ScanFolder(sPath);
		l.relock();
	}

	query.exec("PRAGMA synchronous = 1");
	m_bReady = true;
	if( m_bActive )
	{
		emit sharesReady();
		l.unlock();
		RunHashing();
	}
}

// Recursively scan sPath for new files (modified files are already handled)
void CShareManager::ScanFolder(QString sPath, qint64 nParentID)
{
	QMutexLocker l(&m_oSection);

	if( !m_bActive )
		return;

	l.unlock();

	qDebug() << "Scanning " << sPath << nParentID;

	QDir d(sPath);

	if( !d.exists(sPath) )
	{
		if( !d.mkpath(sPath) )
		{
			qDebug() << "Cannot create " << sPath << "skipping...";
			return;
		}
	}

	if( !d.isReadable() )
	{
		qDebug() << "Directory " << sPath << "is not readable, skipping...";
		return;
	}

	// check if directory entry already exists
	l.relock();
	if( !m_bActive )
		return;

	qint64 nDirID = 0;

	QSqlQuery query(m_oDatabase);
	query.prepare("SELECT id FROM dirs WHERE parent = ? AND path LIKE ?");
	query.bindValue(0, QVariant(nParentID));
	query.bindValue(1, QVariant(sPath));
	if( !query.exec() )
	{
		qDebug() << "SQL Query failed (dir id): " << query.lastError().text();
		return;
	}

	if( query.next() )
	{
		nDirID = query.record().value(0).toLongLong();
		query.finish();
	}
	else
	{
		query.prepare("INSERT INTO dirs (path, parent) VALUES(?,?)");
		query.bindValue(0, QVariant(sPath));
		query.bindValue(1, QVariant(nParentID));
		if( !query.exec() )
		{
			qDebug() << "SQL Query failed (missing dir): " << query.lastError().text();
			return;
		}
		else
		{
			nDirID = query.lastInsertId().toLongLong();
		}
	}

	// now check if something is added to the dir
	query.prepare("SELECT name FROM files WHERE dir_id = ?");
	query.bindValue(0, QVariant(nDirID));
	if( !query.exec() )
	{
		qDebug() << "SQL Query failed (fetch files): " << query.lastError().text();
		return;
	}

	QList<QString> lFilesInDB;
	QList<QString> lFilesInFS;

	while( query.next() )
	{
		lFilesInDB.append(query.record().value(0).toString());
	}

	lFilesInFS = d.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	foreach( QString sFile, lFilesInDB )
	{
		lFilesInFS.removeOne(sFile);
	}

	m_oDatabase.transaction();
	foreach( QString sFile, lFilesInFS )
	{
		QSqlQuery insq(m_oDatabase);
		insq.prepare("INSERT INTO hash_queue (dir_id, filename) VALUES(?,?)");
		insq.bindValue(0, QVariant(nDirID));
		insq.bindValue(1, QVariant(sFile));
		if( !insq.exec() )
		{
			qDebug() << "Cannot queue file for hashing:" << insq.lastError().text();
		}
	}
	m_oDatabase.commit();

	lFilesInDB.clear();
	lFilesInFS.clear();

	// now find and scan subdirs
	QList<QString> lSubdirs;
	lSubdirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	/*m_oDatabase.transaction();
	foreach( QString sDir, lSubdirs )
	{
		QSqlQuery insq(m_oDatabase);
		insq.prepare("INSERT OR IGNORE INTO dirs (path, parent) VALUES(?,?)");
		insq.bindValue(0, QVariant(d.absolutePath() + "/" + sDir));
		insq.bindValue(1, QVariant(nDirID));
		if( !insq.exec() )
		{
			qDebug() << "SQL Query failed (insert dirs):" << insq.lastError().text();
		}
	}
	m_oDatabase.commit();*/

	if( !m_bActive )
		return;

	l.unlock();

	foreach( QString sDir, lSubdirs )
	{
		ScanFolder(d.absolutePath() + "/" + sDir, nDirID);
	}
}

// meant to be called from other threads
// Don't call it from ShareManager thread or it will deadlock
QList<QSqlRecord> CShareManager::Query(const QString sQuery)
{
	QMutexLocker l(&m_oSection);

	m_lQueryResults.clear();
	emit executeQuery(sQuery);
	m_oQueryCond.wait(&m_oSection);

	QList<QSqlRecord> lRecs = m_lQueryResults;
	m_lQueryResults.clear();

	return lRecs;
}

void CShareManager::execQuery(const QString &sQuery)
{
	m_oSection.lock();

	QSqlQuery query(m_oDatabase);
	if( !query.exec(sQuery) )
	{
		qDebug() << "SQL Query failed: " << query.lastError().text();
	}
	else
	{
		while( query.next() )
		{
			m_lQueryResults.append(query.record());
		}
	}

	m_oQueryCond.wakeAll();
	m_oSection.unlock();
}

void CShareManager::RunHashing()
{
	QMutexLocker l(&m_oSection);

	qDebug() << "CShareManager::RunHashing()";

	QSqlQuery query(m_oDatabase);
	query.prepare("SELECT hq.rowid, hq.filename, d.id, d.path FROM hash_queue hq LEFT JOIN dirs d ON(hq.dir_id = d.id) LIMIT 100");
	if( !query.exec() )
	{
		qDebug() << "SQL query failed: " << query.lastError().text();
		return;
	}

	QString sRowIDs;

	bool bFinished = true;

	while( query.next() )
	{
		sRowIDs.append(query.record().value(0).toString()).append(",");

		CSharedFilePtr pFile(new CSharedFile);
		pFile->m_sFileName = query.record().value(1).toString();
		pFile->m_nDirectoryID = query.record().value(2).toLongLong();
		pFile->m_sDirectory = query.record().value(3).toString();

		CFileHasher::HashFile(pFile);

		bFinished = false;
	}

	sRowIDs.truncate(sRowIDs.size() - 1);

	query.exec(QString("DELETE FROM hash_queue WHERE rowid IN(%1)").arg(sRowIDs));

	if( bFinished )
	{
		BuildHashTable();
		emit sharesReady();
	}
}

void CShareManager::OnFileHashed(CSharedFilePtr pFile)
{
	QMutexLocker l(&m_oSection);

	qDebug() << "OnFileHashed";

	pFile->Stat();
	pFile->m_bShared = true;
	pFile->Serialize(&m_oDatabase);
}

CQueryHashTable* CShareManager::GetHashTable()
{
	if( !m_bReady || !m_bTableReady )
		return 0;

	return m_pTable;
}

void CShareManager::BuildHashTable()
{
	if( m_pTable == 0 )
	{
		m_pTable = new CQueryHashTable();
		if( !m_pTable )
			return;
		m_pTable->Create();
	}

	m_pTable->Clear();

	// TODO: Optimize it

	QSqlQuery q(m_oDatabase);
	q.prepare("SELECT keyword FROM keywords");
	if( !q.exec() )
	{
		qDebug() << "SQL query failed:" << q.lastError().text();
	}
	else
	{
		while( q.next() )
		{
			m_pTable->AddExactString(q.record().value(0).toString());
		}

		q.prepare("SELECT sha1 FROM hashes");
		if( !q.exec() )
		{
			qDebug() << "SQL query failed:" << q.lastError().text();
		}
		else
		{
			while( q.next() )
			{
				QByteArray m_oTemp = q.record().value(0).toByteArray();
				CHash* pHash = CHash::FromRaw(m_oTemp, CHash::SHA1);
				if( pHash )
				{
					m_pTable->AddExactString(pHash->ToURN());
					delete pHash;
				}
			}
		}
		m_bTableReady = true;
	}


}
