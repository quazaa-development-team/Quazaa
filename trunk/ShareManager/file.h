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
	qint64			m_nSize;

public:
	typedef enum { Magnet, eD2k } URIType;

public:
	explicit CFile(QObject *parent = NULL);

	// Creates a file with a given name and parent. See QFile::QFile(const QString&
	// name, QObject* parent = NULL) for more information.
	CFile(const QString& name, QObject* parent = NULL);

	// Returns true if the file has been created using the default constructor CFile()
	// without any parameter and a file name has not been set using setFileName.
	inline bool isNull() const;

	// Returns a list of all hashes attributed to this file. Note that this does not
	// perform checking operations, so it is in theory possible to have 2 differnet
	// hashes of the same type returned within this list.
	inline QList< CHash > getHashes() const;

	// Sets a hash for the file.
	inline void setHash(CHash oHash);

	// Removes a hash from the set of hashes of a file. Returns false if the requested
	// hash could not be found; otherwise returns true.
	bool removeHash(CHash oHash);

	// Creates an URI of the files attributes.
	QString toURI(URIType type) const;

	// Allows to store tagging information. This can be used for example to store
	// information about whether a file is shared or not, to which sharing group it
	// belongs or whether it exists on the hard drive or not.
	inline void setTag(QString sTag);

	// Removes a specified tag. Returns true if the operation was successful.
	inline bool removeTag(QString sTag);

	// Checks whether a file has been marked with a given tag.
	bool isTagged(QString sTag) const;

	/* ------ reimplemented functions ------ */

	// Sets the name of the file. Uses the same syntax as QFile::setFileName(). Sets
	// the tag "physical" if the specified file is physically existant on the hard drive.
	void setFileName(const QString& name);

	// Returns the file size. If it is unknown for some reason, -1 is returned.
	inline virtual qint64 size() const;

	// Resizes the file. Updates the file data within this class as well as resizes the
	// file. See QFile::resize( qint64 sz ) for documentation details.
	bool resize( qint64 sz );

signals:

public slots:

private:
	void setup();

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

qint64 CFile::size() const
{
	return m_nSize;
}

#endif // FILE_H
