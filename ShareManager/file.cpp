#include "file.h"

CFile::CFile(QObject *parent) :
	QFile( parent )
{
	m_bNull = parent; // Set marker if created with default constructor CFile::CFile().
}

CFile::CFile(const QString& name, QObject* parent) :
	m_bNull( false )
{
	if ( exists() )
		setTag( "physical" ); // Tag the file as being physically existant on HDD.
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
}


