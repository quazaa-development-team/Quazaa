#include "downloadsource.h"
#include "timedsignalqueue.h"

CDownloadSource::CDownloadSource(CDownload* download, QObject *parent) :
	QObject( parent ),
	m_pOwner( download )
{
	if ( m_pOwner )
		schdeuleNextAccess( m_pOwner->requestInitialSourceAccessTime() );
}

void CDownloadSource::setOwningDownload(CDownload* download)
{
	m_pOwner = download;

	if ( m_pOwner )
	{
		schdeuleNextAccess( m_pOwner->requestInitialSourceAccessTime() );
	}
}

quint32 CDownloadSource::secToNextAccess(const quint32& tNow)
{
	return m_tNextAccess - tNow;
}

bool CDownloadSource::schdeuleNextAccess(const quint32& tAccess)
{
	m_tNextAccess = tAccess;
	signalQueue.push( this, SLOT( query() ), tAccess );
	return true;
}
