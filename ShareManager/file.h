#ifndef FILE_H
#define FILE_H

#include <QFile>

#include "Hashes/hash.h"

class CFile : public QFile
{
    Q_OBJECT

private:
	QList < CHash* > m_Hashes;
	QSet< QString > m_Tags;

public:
    explicit CFile(QObject *parent = 0);

	bool isTagged(QString sTag);
	void addTag(QString sTag);
	bool removeTag(QString sTag);

	QList< CHash* > getHashes();
	void addHash( CHash* pHash );
	bool removeHash( CHash* pHash );

signals:

public slots:

};

#endif // FILE_H
