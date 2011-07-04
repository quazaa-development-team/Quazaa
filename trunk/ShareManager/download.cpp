#include "download.h"

CDownload::CDownload(QObject* parent) :
	CFile( parent ),
	m_nSeclectedFileName( 0 )
{
}

CDownload::CDownload(const QString& file, QObject* parent) :
	CFile( file, parent ),
	m_nSeclectedFileName( 0 )
{
}

CDownload::CDownload(const QFile& file, QObject* parent) :
	CFile( file, parent ),
	m_nSeclectedFileName( 0 )
{
}

CDownload::CDownload(const QDir& dir, const QString& file, QObject* parent) :
	CFile( dir, file, parent ),
	m_nSeclectedFileName( 0 )
{
}

CDownload::CDownload(const QFileInfo& fileinfo, QObject* parent) :
	CFile( fileinfo, parent ),
	m_nSeclectedFileName( 0 )
{
}

quint32 CDownload::requestInitialSourceAccessTime()
{
	quint32 tNow = static_cast< quint32 >( time( NULL ) );
	return ++tNow;
}
