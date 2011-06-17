#ifndef FILE_H
#define FILE_H

#include <QFile>

#include "Hashes/hash.h"

class CFile : public QFile
{
    Q_OBJECT

private:
	QList< CHash >	m_Hashes; // SHA1 (Base32), ED2K (MD4, Base16), BitTorrent Info Hash (Base32), TigerTree Root Hash (Base32), MD5 (Base16)
	QSet< QString > m_Tags;
	bool			m_bNull; // Set to 0 if constructed with default constructor.

public:
	typedef enum { Magnet, eD2k } URIType;

public:
	explicit CFile(QObject *parent = NULL);
	CFile(const QString& name, QObject* parent = NULL);

	inline bool isNull() const;

	inline QList< CHash > getHashes() const;
	inline void setHash(CHash oHash);

	bool removeHash(CHash oHash);

	QString toURI(URIType type) const;

	inline void setTag(QString sTag);
	inline bool removeTag(QString sTag);

	bool isTagged(QString sTag) const;

	// reimplemented functions
	void setFileName(const QString& name);

signals:

public slots:

};

bool CFile::isNull() const
{
	return m_bNull;
}

QList< CHash > CFile::getHashes() const
{
	return m_Hashes;
}

void CFile::setHash(CHash oHash)
{
	m_Hashes.push_back( oHash );
}

void CFile::setTag(QString sTag)
{
	m_Tags.insert( sTag );
}

bool CFile::removeTag(QString sTag)
{
	return m_Tags.remove( sTag );
}

#endif // FILE_H
