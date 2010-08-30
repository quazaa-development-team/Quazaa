//
// SharedFile.cpp
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

#include "SharedFile.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QVariant>
#include <QMap>

CSharedFile::CSharedFile()
{
	m_nFileID = 0;
	m_nDirectoryID = 0;
	m_bShared = false;
	m_nSize = 0;
	m_nLastModified = 0;
}

void CSharedFile::Serialize(QSqlDatabase *pDatabase)
{
	// get directory id if needed
	if( m_nDirectoryID == 0 )
	{
		QSqlQuery query(*pDatabase);

		query.prepare("SELECT id FROM dirs WHERE path LIKE '?'");
		query.bindValue(0, m_sDirectory);
		if( !query.exec() )
		{
			qDebug() << "Cannot fetch directory id: " << query.lastError().text();
			return;
		}

		if( query.size() != 1 )
		{
			qDebug() << "Bad record number: " << query.size();
			return;
		}

		query.next();

		m_nDirectoryID = query.value(0).toLongLong();
	}

	// now insert or update file record
	if( m_nFileID == 0 )
	{
		// insert

		pDatabase->transaction();

		QSqlQuery query(*pDatabase);
		query.prepare("INSERT INTO files (dir_id, name, size, last_modified, shared) VALUES (?,?,?,?,?)");
		query.bindValue(0, m_nDirectoryID);
		query.bindValue(1, m_sFileName);
		query.bindValue(2, m_nSize);
		query.bindValue(3, m_nLastModified);
		query.bindValue(4, m_bShared);
		if( !query.exec() )
		{
			qDebug() << "Cannot insert new record: " << query.lastError().text();
			return;
		}

		qint64 nFileID = query.lastInsertId().toLongLong();
		qDebug() << "New file ID: " << nFileID;

		QSqlQuery q2(*pDatabase);
		q2.exec(QString("DELETE FROM hashes WHERE file_id = %1").arg(nFileID));

		QMap<QString,QVariant> mapValues;
		mapValues.insert("file_id", QVariant(nFileID));

		foreach( CHash oHash, m_lHashes)
		{
			if( pDatabase->record("hashes").contains(oHash.GetFamilyName()) )
			{
				mapValues.insert(oHash.GetFamilyName(), oHash.RawValue());
			}
		}

		if( mapValues.count() > 1 )
		{
			QSqlQuery qh(*pDatabase);

			QString sFields, sValues;

			for( QMap<QString,QVariant>::iterator itValues = mapValues.begin(); itValues != mapValues.end(); itValues++ )
			{
				sFields.append(itValues.key()).append(",");
			}
			sFields.truncate(sFields.size() - 1);

			qh.prepare(QString("INSERT INTO hashes (" + sFields + ") VALUES (" + sValues + ")") );

			for( int i = 0; i < mapValues.count(); i++ )
			{
				sValues += "?,";
			}
			sValues.truncate(sValues.size() - 1);

			int nCurrPos = 0;
			for( QMap<QString,QVariant>::iterator itValues = mapValues.begin(); itValues != mapValues.end(); itValues++ )
			{
				qh.bindValue(nCurrPos, QVariant(itValues.value()));
				nCurrPos++;
			}

			if( !qh.exec() )
			{
				qDebug() << "Cannot insert hashes: " << qh.lastError().text() << qh.executedQuery();
			}
		}

		pDatabase->commit();

	}
}
