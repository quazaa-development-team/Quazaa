#include "file.h"

CFile::CFile(QObject *parent) :
    QFile(parent)
{
}

bool CFile::isTagged(QString sTag)
{
	QSet< QString >::ConstIterator i = m_setTags.find( sTag );

	if ( i != m_setTags.end() )
		return true;

	return false;
}

void CFile::addTag(QString sTag)
{
	m_setTags.insert( sTag );
}

bool CFile::removeTag(QString sTag)
{
	return m_setTags.remove( sTag );
}
