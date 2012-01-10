/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
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

QDataStream& operator<<(QDataStream& s, const CDownload& rhs)
{
	// basic info
	s << quint32(1); // version
	s << "dn" << rhs.m_sDisplayName;
	s << "tn" << rhs.m_sTempName;
	s << "s" << rhs.m_nSize;
	s << "cs" << rhs.m_nCompletedSize;
	s << "state" << rhs.m_nState;
	s << "mf" << rhs.m_bMultifile;
	s << "pr" << rhs.m_nPriority;

	// files
	foreach(CDownload::FileListItem i, rhs.m_lFiles)
	{
		s << "file" << quint32(1); // version
		s << "name" << i.sFileName;
		s << "so" << i.nStartOffset;
		s << "eo" << i.nEndOffset;
		foreach(CHash h, i.lHashes)
		{
			s << "hash" << h.ToURN();
		}
		s << "file-end";
	}

	// sources
	foreach(CDownloadSource* pSource, rhs.m_lSources)
	{
		s << *pSource;
	}

	// TODO: save file fragments

	s << "eof";
	return s;
}
QDataStream& operator>>(QDataStream& s, CDownload& rhs)
{
	quint32 nVer;

	s >> nVer;

	if( nVer == 1 )
	{
		QByteArray sTag;
		s >> sTag;
		sTag.chop(1);

		while( sTag != "eof" && s.status() == QDataStream::Ok )
		{
			if( sTag == "dn" )
			{
				s >> rhs.m_sDisplayName;
			}
			else if( sTag == "tn" )
			{
				s >> rhs.m_sTempName;
			}
			else if( sTag == "s" )
			{
				quint64 nSize;
				s >> nSize;
				rhs.m_nSize = nSize;
				rhs.m_lActive.setSize(nSize);
				rhs.m_lCompleted.setSize(nSize);
				rhs.m_lVerified.setSize(nSize);
			}
			else if( sTag == "cs" )
			{
				s >> rhs.m_nCompletedSize;
			}
			else if( sTag == "state" )
			{
				int x;
				s >> x;
				rhs.m_nState = static_cast<CDownload::DownloadState>(x);
			}
			else if( sTag == "mf" )
			{
				s >> rhs.m_bMultifile;
			}
			else if( sTag == "pr" )
			{
				s >> rhs.m_nPriority;
			}
			else if( sTag == "file" )
			{
				quint32 nVerF;
				s >> nVerF;

				if( nVerF == 1 )
				{
					CDownload::FileListItem item;

					QByteArray sTag2;
					s >> sTag2;
					sTag2.chop(1);

					while( sTag2 != "file-end" && s.status() == QDataStream::Ok )
					{
						if( sTag2 == "name" )
						{
							s >> item.sFileName;
						}
						else if( sTag2 == "so" )
						{
							s >> item.nStartOffset;
						}
						else if( sTag2 == "eo" )
						{
							s >> item.nEndOffset;
						}
						else if( sTag2 == "hash" )
						{
							QString sHash;
							s >> sHash;
							CHash* pHash = CHash::FromURN(sHash);
							item.lHashes.append(*pHash);
							delete pHash;
						}

						s >> sTag2;
						sTag2.chop(1);
					}
				}
			}
			else if( sTag == "download-source" )
			{
				CDownloadSource* pSource = new CDownloadSource();
				s >> *pSource;
				rhs.addSource(pSource);
			}

			s >> sTag;
			sTag.chop(1);
		}
	}
	return s;
}

CDownload::CDownload(CQueryHit* pHit, QObject *parent) :
	QObject(parent),
	m_bSignalSources(false),
	m_nPriority(NORMAL)
{
	Q_ASSERT(pHit != NULL);

	int nSources = 0;
	m_sDisplayName = pHit->m_sDescriptiveName;
	m_nSize = pHit->m_nObjectSize;
	m_lCompleted.setSize(m_nSize);
	m_lVerified.setSize(m_nSize);
	m_lActive.setSize(m_nSize);
	m_nCompletedSize = 0;
	m_sTempName = QString().number(qrand() % qrand()).append("-").append(m_sDisplayName).replace(QRegExp("[^a-zA-Z0-9\\s\\-\\_\\.]+"), "");

	FileListItem oFile;
	oFile.sFileName = m_sDisplayName;
	oFile.nStartOffset = 0;
	oFile.nEndOffset = m_nSize;
	m_lFiles.append(oFile);
	m_bMultifile = false;
	// hashes are not needed here, for single file download

	nSources = addSource(pHit);

	setState(dsQueued);

	systemLog.postLog(LogCategory::Network, LogSeverity::Notice, qPrintable(tr("Created download for %s with %d sources.")), qPrintable(m_sDisplayName), nSources);
}

CDownload::~CDownload()
{
	qDeleteAll(m_lSources);
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

	if( m_bSignalSources )
		emit sourceAdded(pSource);

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

bool CDownload::sourceExists(CDownloadSource *pSource)
{
	return (m_lSources.indexOf(pSource) != -1);
}

void CDownload::setState(CDownload::DownloadState state)
{
	m_nState = state;
	emit stateChanged(state);
}

// Invoked by download model
// let the download go to the model first, giving a chance to connect signals to corresponding objects
// then request sources for this download, so model can stay in sync
void CDownload::emitSources()
{
	if( !m_bSignalSources )
	{
		m_bSignalSources = true;
		foreach(CDownloadSource* pSource, m_lSources)
		{
			emit sourceAdded(pSource);
		}
	}
}


