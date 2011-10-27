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

#include "download.h"
#include "queryhit.h"
#include "downloadsource.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

CDownload::CDownload(CQueryHit* pHit, QObject *parent) :
    QObject(parent)
{
	Q_ASSERT(pHit != NULL);

	int nSources = 0;
	m_sDisplayName = pHit->m_sDescriptiveName;
	m_nSize = pHit->m_nObjectSize;
	m_lCompleted.setSize(m_nSize);
	m_lVerified.setSize(m_nSize);
	m_lActive.setSize(m_nSize);
	m_nCompletedSize = 0;

	FileListItem oFile;
	oFile.sFileName = m_sDisplayName;
	oFile.nStartOffset = 0;
	oFile.nEndOffset = m_nSize;
	m_lFiles.append(oFile);
	m_bMultifile = false;
	// hashes are not needed here, for single file download

	nSources = addSource(pHit);

	m_nState = dsQueued;

	systemLog.postLog(LogSeverity::Notice, qPrintable(tr("Created download for %s with %d sources.")), qPrintable(m_sDisplayName), nSources);
}

bool CDownload::addSource(CDownloadSource *pSource)
{
	foreach(CDownloadSource* pThis, m_lSources)
	{
		if( (!pThis->m_oGUID.isNull() && pThis->m_oGUID == pSource->m_oGUID)
				|| (pThis->m_oAddress == pSource->m_oAddress)
				|| (!pThis->m_sURL.isEmpty() && pThis->m_sURL == pSource->m_sURL))
		{
			return false;
		}
	}

	m_lSources.append(pSource);

	return true;
}
int CDownload::addSource(CQueryHit *pHit)
{
	CQueryHit* pThis = pHit;
	int nSources = 0;

	while(pThis != NULL)
	{
		// add hashes
		for(QList<CHash>::const_iterator it = pThis->m_lHashes.begin(); it != pThis->m_lHashes.end(); ++it)
		{
			bool bFound = false;
			for(QList<CHash>::const_iterator it2 = m_lHashes.begin(); it2 != m_lHashes.end(); ++it)
			{
				if(*it == *it2)
				{
					bFound = true;
					break;
				}
			}

			if( !bFound )
			{
				m_lHashes.append(*it);
			}
		}

		CDownloadSource* pSource = new CDownloadSource(pThis);
		if( addSource(pSource) )
		{
			nSources++;
		}
		else
		{
			delete pSource;
		}
		pThis = pThis->m_pNext;
	}

	return nSources;
}

