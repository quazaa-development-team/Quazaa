#include "file.h"

CFile::CFile(QObject *parent) :
	QFile( parent ),
	m_bNull( true ),	// The file is supposed to be invaild until its name has been set.
	m_nSize( -1 )		// The file size is unknown as of now.
{
}

CFile::CFile(const QString& name, QObject* parent) :
	m_bNull( false ),
	m_nSize( -1 )		// The file size is unknown as of now.
{
	setup();
}

bool CFile::removeHash(CHash oHash)
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

bool CFile::isTagged(QString sTag) const
{
	QSet< QString >::ConstIterator i = m_Tags.find( sTag );

	if ( i != m_Tags.end() )
		return true;

	return false;
}

void CFile::setFileName(const QString& name)
{
	m_bNull = false;
	QFile::setFileName( name );

	setup();
}

bool CFile::resize( qint64 sz )
{
	bool bSuccess = QFile::resize( sz );
	if ( bSuccess )
		setup();
	return bSuccess;
}

void CFile::setup()
{
	if ( exists() )
	{
		setTag( "physical" ); // Tag the file as being physically existant on HDD.

		// TODO: Find out whether it is necessary to open a file to get it size from disk.
		// size() describes as: "Reimplemented from QIODevice::size()./Returns the size of the file."
		// This wouldn't be a problem if the description of QIODevice::size() didn't contain:
		// "If the device is closed, the size returned will not reflect the actual size of the device."
		open( QIODevice::ReadOnly );
		m_nSize = QFile::size();
		close();
	}
}

