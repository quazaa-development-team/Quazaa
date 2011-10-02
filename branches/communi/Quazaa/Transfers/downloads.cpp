#include "downloads.h"
#include "download.h"

CDownloads Downloads;

CDownloads::CDownloads(QObject *parent) :
    QObject(parent)
{
}

void CDownloads::add(CQueryHit *pHit)
{
	CDownload* pDownload = new CDownload(pHit, this);
	m_lDownloads.append(pDownload);
	systemLog.postLog(LogSeverity::Notice, qPrintable(tr("Queued download job for %s")), qPrintable(pDownload->m_sDisplayName));
	emit downloadAdded();
}
