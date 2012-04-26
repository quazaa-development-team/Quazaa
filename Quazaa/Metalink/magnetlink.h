#ifndef MAGNETLINK_H
#define MAGNETLINK_H

#include <QList>
#include <QString>
#include <QUrl>

class CHash;

class CMagnetLink
{
private:

	// internal storage structure
	struct File
	{
		quint16 m_nID;				// We assume there are no more than 2^16 files in a magnet link.
		quint64 m_nFileSize;
		QString m_sFileName;
		QList<CHash> m_lHashes;		// Includes all hashes provided via <hash> tag.
		QList<QUrl> m_lURLs;		// Includes http, https, ftp, ftps, etc.
		QList<QUrl> m_lTrackers;	// BitTorrent Trachers for this file

		File();
		bool isValid();				// Returns true if file struct contains enough data to initialize a download.
	};

	QString		m_sMagnet;
	QList<File>	m_lFiles;			// Contains all files sorted by ID.

public:
	CMagnetLink();
	CMagnetLink(QString sMagnet);

	/**
	  * Returns true if Magnet link could be parsed without errors.
	  */
	bool parseMagnet(QString sMagnet);

	/**
	  * Returns true if magnet points to at least 1 file that is valid - e.g. which we can download.
	  */
	inline bool isValid();

private:
	void subsectionError(QString sParam, QString sSubsection);
};

bool CMagnetLink::isValid()
{
	return m_lFiles.size();
}

#endif // MAGNETLINK_H
