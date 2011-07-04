#ifndef DOWNLOADINGFILE_H
#define DOWNLOADINGFILE_H

#include "file.h"
#include "downloadsource.h"

class CDownload : public CFile
{
private:
	QList<QString>	m_lFileNames;			// All names of the file as returned by the different search sources, including eventual user input.
	quint8			m_nSeclectedFileName;	// The file name currently selected to be used when migrating the file out of the incompletes directory.
	QString			m_sTargetDirectory;		// The directory the file will be moved to uppon completion.

	QList<CDownloadSource*> m_lSources;

public:
	explicit CDownload(QObject* parent = NULL);
	explicit CDownload(const QString& file, QObject* parent = NULL);
	explicit CDownload(const QFile& file, QObject* parent = NULL);
	explicit CDownload(const QDir& dir, const QString& file, QObject* parent = NULL);
	explicit CDownload(const QFileInfo& fileinfo, QObject* parent = NULL);

	// This method is queried by sources to schedule the time of their initial access.
	// The goal of this method is to allow the program to query a certain number of sources/s on
	// restart instead of having all sources trying to query at the same time.
	// TODO: Implement
	quint32 requestInitialSourceAccessTime();

	inline void setFutureFileNames( const QList<QString>& lNames );
	inline void addFutureFileName( const QString& sName );
	inline QList<QString> getFutureFileNames() const;

	inline void setSelectedFileNameID( const quint8& ID );
	inline quint8 getSelectedFileNameID() const;
	inline QString getSelectedFileName() const;

	// Note that the caller must assure the validity of the passed directory name.
	inline void setTargetDirectory( const QString& sDirectory );
	inline QString getTargetDirectory() const;

	// Moves completed download to target directory.
	inline bool finalize();
};

void CDownload::setFutureFileNames( const QList<QString>& lNames )
{
	m_lFileNames = lNames;
	m_nSeclectedFileName = m_nSeclectedFileName < m_lFileNames.size() ? m_nSeclectedFileName : 0;
}

void CDownload::addFutureFileName( const QString& sName )
{
	m_lFileNames.append( sName );
}

QList<QString> CDownload::getFutureFileNames() const
{
	return m_lFileNames;
}

void CDownload::setSelectedFileNameID( const quint8& ID )
{
	m_nSeclectedFileName = ID < m_lFileNames.size() ? ID : m_nSeclectedFileName;
}

quint8 CDownload::getSelectedFileNameID() const
{
	return m_nSeclectedFileName;
}

QString CDownload::getSelectedFileName() const
{
	// Set functions perform the necessary checks to make sure m_nSeclectedFileName is always valid.
	return m_lFileNames.at( m_nSeclectedFileName );
}

void CDownload::setTargetDirectory( const QString& sDirectory )
{
	m_sTargetDirectory = sDirectory;
}

QString CDownload::getTargetDirectory() const
{
	return m_sTargetDirectory;
}

// Moves completed download to target directory.
bool CDownload::finalize()
{
	// TODO: create kind of a buffer between Quazaa and HDD writes to prevent our
	// program to hang (partially) when big amounts of data havve to be written on disk.
	// This buffer would take all data to be written and store it temporarly to execute
	// the file writes on a delayed basis one by one - respectively execute them in
	// parallel if we are trying to write on multiple physical HDDs at the same time.

	return rename( m_sTargetDirectory +  "/" + getSelectedFileName() );
}

#endif // DOWNLOADINGFILE_H
