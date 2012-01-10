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

#include "downloads.h"
#include "download.h"
#include "transfers.h"

#include "quazaasettings.h"

#include <QDir>
#include <QFile>

#ifdef _DEBUG
#include "debug_new.h"
#endif

CDownloads Downloads;

CDownloads::CDownloads(QObject *parent) :
    QObject(parent)
{
	qRegisterMetaType<CDownload*>("CDownload*");
	qRegisterMetaType<CDownloadSource*>("CDownloadSource*");
	qRegisterMetaType<CDownload::DownloadState>("CDownload::DownloadState");
}

void CDownloads::add(CQueryHit *pHit)
{
	ASSUME_LOCK(Downloads.m_pSection);

	CDownload* pDownload = new CDownload(pHit);
	pDownload->moveToThread(&TransfersThread);
	m_lDownloads.append(pDownload);
	systemLog.postLog(LogCategory::Network, LogSeverity::Notice, qPrintable(tr("Queued download job for %s")), qPrintable(pDownload->m_sDisplayName));
	emit downloadAdded(pDownload);
}

bool CDownloads::exists(CDownload *pDownload)
{
	return (m_lDownloads.indexOf(pDownload) != -1);
}

void CDownloads::start()
{
	QMutexLocker l(&m_pSection);

	QDir d(quazaaSettings.Downloads.IncompletePath);

	if( !d.exists() )
		d.mkpath(quazaaSettings.Downloads.IncompletePath);

	if( d.isReadable() )
	{
		QStringList files = d.entryList(QStringList() << "*.!qd");

		foreach(QString f, files)
		{
			QFile file(quazaaSettings.Downloads.IncompletePath + "/" + f);

			if( file.exists() && file.open(QFile::ReadOnly) )
			{
				CDownload* pDownload = new CDownload();
				QDataStream stream(&file);

				stream >> *pDownload;

				pDownload->moveToThread(&TransfersThread);
				m_lDownloads.append(pDownload);
				emit downloadAdded(pDownload);
				systemLog.postLog(LogCategory::Network, LogSeverity::Notice, qPrintable(tr("Loaded download: %s")), qPrintable(pDownload->m_sDisplayName));
			}
		}
	}
}

void CDownloads::stop()
{
	QMutexLocker l(&m_pSection);

	foreach( CDownload* pDownload, m_lDownloads )
	{
		QString sFileName = pDownload->m_sTempName;

		sFileName.append(".!qd");

		QFile f(quazaaSettings.Downloads.IncompletePath + "/" + sFileName);

		if( f.open(QFile::WriteOnly) )
		{
			QDataStream s(&f);

			s << *pDownload;

			f.close();
		}

		delete pDownload;
	}

	m_lDownloads.clear();
}

void CDownloads::emitDownloads()
{
	QMutexLocker l(&m_pSection);

	foreach( CDownload* pDl, m_lDownloads )
	{
		emit downloadAdded(pDl);
	}
}

