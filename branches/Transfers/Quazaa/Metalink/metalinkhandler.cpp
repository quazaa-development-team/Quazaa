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

#include "metalinkhandler.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

MetaFile::MetaFile(quint16 ID) :
	m_nID( ID ),
	m_nFileSize( 0 )
{}

bool MetaFile::isValid() const
{
	return m_lHashes.size() || m_sURIs.size();
}

CMetalinkHandler::CMetalinkHandler(QFile& oFile) :
	m_bNull( true ),
	m_nSize( 0 )
{
	if ( oFile.open( QIODevice::ReadOnly ) )
	{
		m_oMetaLink.setDevice( &oFile );
		m_bNull = false;
	}

	m_nParsingState = m_oMetaLink.error();
}

CMetalinkHandler::~CMetalinkHandler()
{}

QList<CDownload*> CMetalinkHandler::files() const
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

