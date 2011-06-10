#include "file.h"

CFile::CFile(QObject *parent) :
    QFile(parent)
{
	m_bNull = parent; // Set marker if created with default constructor CFile::CFile().
}

bool CFile::isTagged(QString sTag)
{
	QSet< QString >::ConstIterator i = m_Tags.find( sTag );

	if ( i != m_Tags.end() )
		return true;

	return false;
}

void CFile::addTag(QString sTag)
{
	m_Tags.insert( sTag );
}

bool CFile::removeTag(QString sTag)
{
	return m_Tags.remove( sTag );
}

QList< CHash* > CFile::getHashes()
{
	return m_Hashes;
}

void CFile::addHash( CHash* pHash )
{
	m_Hashes.push_back( pHash );
}

bool CFile::removeHash( CHash* pHash )
{
	for ( QList< CHash* >::Iterator i = m_Hashes.begin(); i != m_Hashes.end(); i++ )
	{
		if ( *pHash == **i )
		{
			m_Hashes.erase( i );
			return true;
		}
	}
	return false;
}


