#include <QDateTime>
#include <QFileInfo>

#include "file.h"

CFile::CFile(CFile& file) :
	QObject( file.parent() ),
	QFileInfo( file.absoluteFilePath() ),
	m_nDirectoryID( file.m_nDirectoryID ),
	m_nFileID( file.m_nFileID ),
	m_Hashes( file.m_Hashes ),
	m_Tags( file.m_Tags )
{
	m_pFile = file.m_pFile ? new QFile( file.absoluteFilePath() ) : NULL;
}

CFile::CFile(QObject* parent) :
	QObject( parent ),
	QFileInfo(),
	m_nDirectoryID( 0 ),
	m_nFileID( 0 ),
	m_pFile( NULL )
{
}

CFile::CFile(const QString& file, QObject* parent) :
	QObject( parent ),
	QFileInfo( file ),
	m_nDirectoryID( 0 ),
	m_nFileID( 0 ),
	m_pFile( NULL )
{
	refresh();
}

CFile::CFile(const QFile& file, QObject* parent) :
	QObject( parent ),
	QFileInfo( file ),
	m_nDirectoryID( 0 ),
	m_nFileID( 0 ),
	m_pFile( NULL )
{
	refresh();
}

CFile::CFile(const QDir& dir, const QString& file, QObject* parent) :
	QObject( parent ),
	QFileInfo( dir, file ),
	m_nDirectoryID( 0 ),
	m_nFileID( 0 ),
	m_pFile( NULL )
{
	refresh();
}

CFile::CFile(const QFileInfo& fileinfo, QObject* parent) :
	QObject( parent ),
	QFileInfo( fileinfo ),
	m_nDirectoryID( 0 ),
	m_nFileID( 0 ),
	m_pFile( NULL )
{
	refresh();
}

void CFile::refresh()
{
	if ( m_pFile )
		delete m_pFile;
	m_pFile = NULL;

	if ( exists() )
	{
		setTag( "physical" ); // Tag the file as being physically existant on HDD.
		m_pFile = new QFile( filePath() );
	}
}

bool CFile::removeHash(const CHash& oHash)
{
	for ( QList< CHash >::Iterator i = m_Hashes.begin(); i != m_Hashes.end(); i++ )
	{
		if ( oHash == *i )
		{
			m_Hashes.erase( i );
			return true;
		}
	}
	return false;
}

// todo: implement this
QString CFile::toURI(URIType type) const
{
	return QString();
}

bool CFile::isTagged(const QString& sTag) const
{
	QSet< QString >::ConstIterator i = m_Tags.find( sTag );

	if ( i != m_Tags.end() )
		return true;

	return false;
}

