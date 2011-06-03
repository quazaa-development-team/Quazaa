#ifndef FILE_H
#define FILE_H

#include <QFile>

#include "Hashes/hash.h"

class CFile : public QFile
{
    Q_OBJECT

public:
	QList< CHash* > m_lHashes;
	QSet< QString > m_setTags;

public:
    explicit CFile(QObject *parent = 0);

	bool isTagged(QString sTag);
	void addTag(QString sTag);
	void removeTag(QString sTag);

signals:

public slots:

};

#endif // FILE_H
