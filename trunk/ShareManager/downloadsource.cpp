#include "downloadsource.h"

CDownloadSource::CDownloadSource(CDownload* download, QObject *parent) :
	QObject( parent ),
	m_pOwner( download )
{
	if ( m_pOwner )
		schdeuleNextAccess( m_pOwner->requestInitialSourceAccessTime() );
}

bool CDownloadSource::setOwningDownload(const CDownload* download)
{
	m_pOwner = download;

	if ( m_pOwner )
		schdeuleNextAccess( m_pOwner->requestInitialSourceAccessTime() );
}

void CDownloadSource::query()
{
	// TODO: implement
}

quint32 CDownloadSource::getSecToNextAccess(const quint32& tNow)
{
	return m_tNextAccess - tNow;
}

bool CDownloadSource::schdeuleNextAccess(const quint32& tAccess)
{
	m_tNextAccess = tAccess;

	// TODO: implement (push into signal queue)
	return true;
}
