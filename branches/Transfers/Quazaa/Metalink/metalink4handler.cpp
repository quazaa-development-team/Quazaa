/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <QXmlStreamReader>

#include "metalink4handler.h"
#include "systemlog.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

CMetalink4Handler::CMetalink4Handler(QFile& oFile) :
	CMetalinkHandler( oFile ),
	nID( 0 )
{
/*

 <?xml version="1.0" encoding="UTF-8"?>
 <metalink xmlns="urn:ietf:params:xml:ns:metalink">
   <published>2009-05-15T12:23:23Z</published>
   <file name="example.ext">
	 <size>14471447</size>
	 <identity>Example</identity>
	 <version>1.0</version>
	 <language>en</language>
	 <description>
	 A description of the example file for download.
	 </description>
	 <hash type="sha-256">3d6fece8033d146d8611eab4f032df738c8c1283620fd02a1f2bfec6e27d590d</hash>
	 <url location="de" priority="1">ftp://ftp.example.com/example.ext</url>
	 <url location="fr" priority="1">http://example.com/example.ext</url>
	 <metaurl mediatype="torrent" priority="2">http://example.com/example.ext.torrent</metaurl>
   </file>
 </metalink>

*/

	quint16 fileID = 0;

	while ( !m_oMetaLink.atEnd() )
	{
		if ( m_oMetaLink.readNextStartElement() && !m_oMetaLink.name().compare( "file" ) )
		{
			if ( parseFile( fileID ) )
			{
				++fileID;
			}
		}

		m_nParsingState = m_oMetaLink.error();
		if ( m_nParsingState != QXmlStreamReader::NoError )
		{
			systemLog.postLog( LogSeverity::Error, QObject::tr(
				"Caught error while parsing Metalink XML in line %1, respectively inside <file> tag ending in line %1."
														).arg( m_oMetaLink.lineNumber() ) );
		}
	}

	m_bNull = fileID;
}

CDownload* CMetalink4Handler::file(const unsigned int& ID) const
{
	// TODO: Implement.

	return new CDownload();
}

bool CMetalink4Handler::parseFile(quint16 ID)
{
	MetaFile oCurrentFile( ID );

	while ( m_oMetaLink.readNextStartElement() )
	{
		QStringRef sComp = m_oMetaLink.name();

		if ( !sComp.compare( "hash" ) )
		{

		}
		else if ( !sComp.compare( "url" ) )
		{

		}
		else if ( !sComp.compare( "size" ) )
		{

		}
		else if ( !sComp.compare( "identity" ) )
		{

		}
		else if ( !sComp.compare( "description" ) )
		{

		}
		else if ( !sComp.compare( "language" ) )
		{

		}
		else if ( !sComp.compare( "version" ) )
		{

		}
		else if ( !sComp.compare( "metaurl" ) )
		{

		}
		else
		{
			systemLog.postLog( LogSeverity::Information, QObject::tr(
				"Caught unexpected XML element while parsing <file> in Metalink XML: %1"
															  ).arg( sComp.toString() ) );
		}

		/*

	 <size>14471447</size>
	 <identity>Example</identity>
	 <version>1.0</version>
	 <language>en</language>
	 <description>
	 A description of the example file for download.
	 </description>
	 <hash type="sha-256">3d6fece8033d146d8611eab4f032df738c8c1283620fd02a1f2bfec6e27d590d</hash>
	 <url location="de" priority="1">ftp://ftp.example.com/example.ext</url>
	 <url location="fr" priority="1">http://example.com/example.ext</url>
	 <metaurl mediatype="torrent" priority="2">http://example.com/example.ext.torrent</metaurl>

		*/
	}

	if ( oCurrentFile.isValid() )
	{
		m_lFiles.push_back( oCurrentFile );
		++nID;

		return true;
	}
	else
	{
		return false;
	}

}