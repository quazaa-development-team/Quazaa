/*
** SharedFile.h
**
** Copyright © Quazaaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
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

#ifndef SHAREDFILE_H
#define SHAREDFILE_H

#include "types.h"
#include "Hashes/hash.h"

#include <QSharedPointer>

class QSqlDatabase;

class CSharedFile
{
protected:
	static bool	m_bMetaRegistered;
public:
	QString m_sFileName;
	quint64	m_nFileID;
	QString	m_sDirectory;
	quint64	m_nDirectoryID;
	quint32	m_nLastModified;
	qint64	m_nSize;
	bool	m_bShared;

	QList<CHash*> m_lHashes;
public:
	CSharedFile();
	~CSharedFile();
	void Stat();
	void Serialize(QSqlDatabase* pDatabase);
};

typedef QSharedPointer<CSharedFile> CSharedFilePtr;

#endif // SHAREDFILE_H
