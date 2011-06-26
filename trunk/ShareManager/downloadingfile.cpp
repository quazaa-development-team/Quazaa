#include "downloadingfile.h"

CDownloadingFile::CDownloadingFile(QObject* parent) :
	CFile( parent ),
	m_nSeclectedFileName( 0 )
{
}

CDownloadingFile::CDownloadingFile(const QString& file, QObject* parent) :
	CFile( file, parent ),
	m_nSeclectedFileName( 0 )
{
}

CDownloadingFile::CDownloadingFile(const QFile& file, QObject* parent) :
	CFile( file, parent ),
	m_nSeclectedFileName( 0 )
{
}

CDownloadingFile::CDownloadingFile(const QDir& dir, const QString& file, QObject* parent) :
	CFile( dir, file, parent ),
	m_nSeclectedFileName( 0 )
{
}

CDownloadingFile::CDownloadingFile(const QFileInfo& fileinfo, QObject* parent) :
	CFile( fileinfo, parent ),
	m_nSeclectedFileName( 0 )
{
}
