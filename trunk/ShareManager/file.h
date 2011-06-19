#ifndef FILE_H
#define FILE_H

#include <QFile>

#include "Hashes/hash.h"

class CFile : public QFile
{
    Q_OBJECT

protected:
	bool			m_bNull; // Set to 0 if constructed with default constructor.

	QString			m_sDirectory;
	quint64			m_nDirectoryID;
	QString			m_sFileName;
	quint64			m_nFileID;
	QString			m_sExtension;
	quint32			m_tLastModified;
	qint64			m_nSize;

	QList< CHash >	m_Hashes; // SHA1 (Base32), ED2K (MD4, Base16), BitTorrent Info Hash (Base32), TigerTree Root Hash (Base32), MD5 (Base16)
	QSet< QString > m_Tags;

public:
	typedef enum { Magnet, eD2k } URIType;

public:
	explicit CFile(QObject *parent = NULL);

	// Creates a file with a given name and parent. See QFile::QFile(const QString&
	// name, QObject* parent = NULL) for more information.
	CFile(const QString& name, QObject* parent = NULL);

	// Copy constructor definition required as copy constructor of QFile seems to be private.
	CFile(const CFile& other);

	virtual ~CFile() {}

	// Returns true if the file has been created using the default constructor CFile()
	// without any parameter and a file name has not been set using setFileName.
	inline bool isNull() const;

	// Returns a list of all hashes attributed to this file. Note that this does not
	// perform checking operations, so it is in theory possible to have 2 differnet
	// hashes of the same type returned within this list.
	inline QList< CHash > getHashes() const;

	// Sets a single hash for the file.
	inline void setHash(const CHash& oHash);

	// Sets a list of hashes for the file.
	inline void setHashes(const QList<CHash>& lHashes);

	// Removes a hash from the set of hashes of a file. Returns false if the requested
	// hash could not be found; otherwise returns true.
	bool removeHash(const CHash& oHash);

	// Creates an URI of the files attributes.
	QString toURI(URIType type) const;

	// Allows to store tagging information. This can be used for example to store
	// information about whether a file is shared or not, to which sharing group it
	// belongs or whether it exists on the hard drive or not.
	inline void setTag(const QString& sTag);

	// Removes a specified tag. Returns true if the operation was successful.
	inline bool removeTag(const QString& sTag);

	// Checks whether a file has been marked with a given tag.
	bool isTagged(const QString& sTag) const;

	bool setFileName(const QString& name);
	bool setDirectory(const QString& path);
	inline void setDirectoryID(const quint64& ID);

	inline QString getFileName() const;
	inline QString getDirectory() const;
	inline quint64 getDirectoryID() const;

	// Extracts the directory path, the file name (containing the file extenstion), as
	// well as the file extension from a given string. Returns false if extraction was
	// not successful. In this case, sDirectory, sFileName and sExtension are left unchanged.
	static bool analyseFileName(const QString& sFilePathAndName, QString& sDirectory, QString& sFileName, QString& sExtension);

	/* ------ reimplemented functions ------ */

	// Sets the name of the file. Uses the same syntax as QFile::setFileName(). Sets
	// the tag "physical" if the specified file is physically existant on the hard drive.
	inline void setPhysicalFileName(const QString& name); // reimplements QFile::setFileName()

	// Returns the file size. If it is unknown for some reason, -1 is returned.
	inline virtual qint64 size() const;

	// Resizes the file. Updates the file data within this class as well as resizes the
	// file. See QFile::resize( qint64 sz ) for documentation details.
	bool resize( qint64 sz );

signals:

public slots:

protected:
	void stat();

private:
	// Note: this is implemented as overloaded function to prevent external use of
	// bIgnoreVariables, which could cause an internal mess. :)
	void setPhysicalFileName(const QString& name, bool bIgnoreVariables);
};

bool CFile::isNull() const
{
	return m_bNull;
}

QList< CHash > CFile::getHashes() const
{
	return m_Hashes;
}

void CFile::setHash(const CHash& oHash)
{
	m_Hashes.push_back( oHash );
}

void CFile::setHashes(const QList<CHash>& lHashes)
{
	m_Hashes.append( lHashes );
}

void CFile::setTag(const QString& sTag)
{
	m_Tags.insert( sTag );
}

bool CFile::removeTag(const QString& sTag)
{
	return m_Tags.remove( sTag );
}

void CFile::setDirectoryID(const quint64& ID)
{
	m_nDirectoryID = ID;
}

QString CFile::getFileName() const
{
	return m_sFileName;
}

QString CFile::getDirectory() const
{
	return m_sDirectory;
}

quint64 CFile::getDirectoryID() const
{
	return m_nDirectoryID;
}

void CFile::setPhysicalFileName(const QString& name)
{
	setPhysicalFileName( name, false );
}

qint64 CFile::size() const
{
	return m_nSize;
}

#endif // FILE_H
