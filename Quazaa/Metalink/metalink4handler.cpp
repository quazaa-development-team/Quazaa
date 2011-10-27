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

#include "metalink4handler.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

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

