#ifndef MAGNETLINK_H
#define MAGNETLINK_H

#include <QList>
#include <QString>
#include <QUrl>

#include "download.h"
#include "managedsearch.h"

class CHash;

class CMagnetLink
{
private:

	// internal storage structure
	struct MediaURL
	{
		QUrl	m_oURL;
		quint8	m_nPriority;		// 255: highest priority; 1: lowest priority; 0: temporary disabled
	};

	// internal storage structure
	struct File
	{
		quint16			m_nID;			// We assume there are no more than 2^16 files in a magnet link.
		quint64			m_nFileSize;
		QString			m_sFileName;
		QList<CHash*>	m_lHashes;		// Includes all hashes provided via <hash> tag.
		QList<MediaURL>	m_lURLs;		// Includes http, https, ftp, ftps, etc.
		QList<QUrl>		m_lTrackers;	// BitTorrent Trachers for this file

		File();
		~File();
		bool isValid();		// Returns true if file struct contains enough data to initialize a download.
	};

	QString			m_sMagnet;
	QList<File>		m_lFiles;			// Contains all files sorted by ID.
	QList<QString>	m_lSearches;		// Contains all files sorted by ID.

public:
	CMagnetLink();
	CMagnetLink(QString sMagnet);

	/**
	  * Returns true if Magnet link could be parsed without errors.
	  */
	bool parseMagnet(QString sMagnet);

	/**
	  * Inserts the information for ID into the download passed as parameter. If NULL is passed, a
	  * new download is created.
	  */
	bool file(const quint16 nID, CDownload* pDownload);

	/**
	  * Creates search from ID.
	  */
	CManagedSearch* search(const quint16 nID);

	/**
	  * Returns true if magnet points to at least 1 file that is valid - e.g. which we can download.
	  */
	inline bool isValid();

	/**
	  * Returns the number of valid files within this Magnet.
	  */
	inline int fileCount();

	/**
	  * Returns the number of searches within this Magnet.
	  */
	inline int searchCount();

private:
	void subsectionError(QString sParam, QString sSubsection);
};

bool CMagnetLink::isValid()
{
	return m_lFiles.size() || m_lSearches.size();
}

int CMagnetLink::fileCount()
{
	return m_lFiles.size();
}

int CMagnetLink::searchCount()
{
	return m_lSearches.size();
}

#endif // MAGNETLINK_H
