#ifndef SHAREDFILE_H
#define SHAREDFILE_H

#include "types.h"
#include "Hashes/Hash.h"

class QSqlDatabase;

class CSharedFile
{
public:
	QString m_sFileName;
	quint64	m_nFileID;
	QString	m_sDirectory;
	quint64	m_nDirectoryID;
	quint32	m_nLastModified;
	qint64	m_nSize;
	bool	m_bShared;

	QList<CHash> m_lHashes;
public:
    CSharedFile();
	void Serialize(QSqlDatabase* pDatabase);
};

#endif // SHAREDFILE_H
