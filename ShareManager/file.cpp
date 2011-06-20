#include <QDateTime>
#include <QFileInfo>

#include "file.h"

CFile::CFile(QObject *parent) :
	QFile( parent ),
	m_bNull( true ),	// The file is supposed to be invaild until its name has been set.
	m_nDirectoryID( 0 ),
	m_nFileID( 0 ),
	m_tLastModified( 0 ),
	m_nSize( -1 )		// The file size is unknown as of now.
{
}

CFile::CFile(const QString& name, QObject* parent) :
	QFile(name, parent),
	m_nDirectoryID( 0 ),
	m_nFileID( 0 ),
	m_tLastModified( 0 ),
	m_nSize( -1 )		// The file size is unknown as of now.
{
	m_bNull = !analyseFileName( name, m_sDirectory, m_sFileName, m_sExtension );

	stat();
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

bool CFile::setFileName(const QString& name)
{
	QString s1, s2, s3;
	if ( analyseFileName( name, s1, s2, s3 ) )
	{
		if ( s2.isEmpty() || s3.isEmpty() )
			return false;

		m_sFileName  = s2;
		m_sExtension = s3;

		// This call prevents setPhysicalFileName from reextracting m_sDirectory
		// and m_sFileName from the string given to the function.
		setPhysicalFileName( m_sDirectory + '/' + m_sFileName, true );

		return true;
	}

	return false;
}

bool CFile::setDirectory(const QString& path)
{
	QString s1, s2, s3;
	if ( analyseFileName( path, s1, s2, s3 ) )
	{
		if ( s1.isEmpty() )
			return false;

		m_sDirectory = s1;

		// This call prevents setPhysicalFileName from reextracting m_sDirectory
		// and m_sFileName from the string given to the function.
		setPhysicalFileName( m_sDirectory + '/' + m_sFileName, true );

		return true;
	}

	return false;
}

bool CFile::analyseFileName(const QString& sFilePathAndName, QString& sDirectory, QString& sFileName, QString& sExtension)
{
	if ( sFilePathAndName.length() < 3 )
		return false;

	int pos = sFilePathAndName.lastIndexOf( '/' );
	if ( pos == -1 )
	{
		pos = sFilePathAndName.lastIndexOf( '.' );

		if ( pos != -1 && pos && pos != sFilePathAndName.length() - 1 )
		{
			// We got only a file name without directory.
			sDirectory.clear();
			sFileName = sFilePathAndName;
			sExtension = sFilePathAndName.mid( ++pos );
			return true;
		}
	}
	else if ( pos > 0 )
	{
		int pos_dot = sFilePathAndName.lastIndexOf( '.' );

		if ( pos == sFilePathAndName.length() - 1 )
		{	// We have been given only a directory name
			sDirectory = sFilePathAndName;
			sExtension.clear();
			sFileName.clear();
			return true;
		}
		else if ( pos < sFilePathAndName.length() - 3 && sFilePathAndName.length() > 4 &&
				  pos_dot != -1 && pos_dot > pos && pos_dot != sFilePathAndName.length() - 1 )
		{
			sDirectory = sFilePathAndName.left( pos );
			sFileName  = sFilePathAndName.mid( ++pos );
			sExtension = sFilePathAndName.mid( ++pos_dot );
			return true;
		}
	}

	return false;
}

void CFile::setPhysicalFileName(const QString& name, bool bIgnoreVariables)
{
	if ( !bIgnoreVariables )
		m_bNull = !analyseFileName( name, m_sDirectory, m_sFileName, m_sExtension );

	if ( !m_bNull )
	{
		QFile::setFileName( name );
		stat();
	}
}

bool CFile::resize( qint64 sz )
{
	bool bSuccess = QFile::resize( sz );
	if ( bSuccess )
		stat();
	return bSuccess;
}

void CFile::stat()
{
	QFileInfo oInfo( fileName() );

	if ( oInfo.exists() )
	{
		setTag( "physical" ); // Tag the file as being physically existant on HDD.

		m_tLastModified = oInfo.lastModified().toTime_t();
		m_nSize = oInfo.size();
	}
}

