#include "metalink4handler.h"

CMetalink4Handler::CMetalink4Handler(QFile& meta4File, QObject *parent) :
	CMetalinkHandler( meta4File, parent ),
	m_bNull( true )
{
	if ( meta4File.open( QIODevice::ReadOnly ) )
	{
		if ( metalink.setContent( &meta4File, true ) )
		{
			// Todo: I am assuming that this gives me the element <metalink xmlns=...>; Verify this.
			QDomElement root = metalink.documentElement();

			// Check for .meta4 XML namespace.
			if ( root.namespaceURI() == "urn:ietf:params:xml:ns:metalink" )
			{
				m_lFiles = root.elementsByTagName( "file" );
				m_nSize = m_lFiles.count();
				m_bNull = false;
			}
			else
			{
				metalink.clear();
			}
		}
		meta4File.close();
	}
}

CDownload* CMetalink4Handler::file(const unsigned int& ID)
{
	QDomElement xmlEntry = m_lFiles.at( ID ).toElement();

	return new CDownload();
}

QList<CDownload*> CMetalink4Handler::files()
{
	QList<CDownload*> result;

	for ( unsigned int i = 0; i < m_nSize; i++ )
	{
		CDownload* pFile = file( i );
		if ( pFile )
		{
			result.append( pFile );
		}
	}

	return result;
}
