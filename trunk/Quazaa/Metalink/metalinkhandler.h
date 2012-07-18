#ifndef METALINKHANDLER_H
#define METALINKHANDLER_H

#include <QObject>

#include <QList>
#include <QFile>
#include <QString>
#include <QXmlStreamReader>

#include "download.h"
#include "magnetlink.h"

struct MediaURI
{
	QString m_sType;
	union
	{
        CMagnet*    m_pMagnet;	// Magnet link
        QUrl*       m_pURL;		// http, https, ftp, ftps, etc.
        QUrl*       m_pTorrent;	// link to .torrent file
	};
	QString m_sLocation;
	quint8 m_nPriority;		// 255: highest priority; 1: lowest priority; 0: temporary disabled
};

struct MetaFile
{
	quint16 m_nID;				// We assume there are no more than 2^16 files in a metalink XML.
	quint64 m_nFileSize;
	QString m_sFileName;
	QString m_sIdentity;
	QString m_sVersion;
	QString m_sLanguage;
	QString m_sDescription;
	QList<CHash*> m_lHashes;	// Includes all hashes provided via <hash> tag.
	QList<MediaURI> m_sURIs;	// Includes web links, links to .torrent files, as well as Magnets.

	MetaFile(quint16 ID);
	bool isValid() const;				// Returns true if file struct contains enough data to initialize a download.
};

class CMetalinkHandler : public QObject
{
protected:
	bool					m_bNull;		// true if invalid
	quint32					m_nSize;		// number of files in metalink
	QXmlStreamReader		m_oMetaLink;
	QXmlStreamReader::Error m_nParsingState;
	QList<MetaFile>			m_lFiles;		// Files are inerted in the order of their ID and always stay sorted.

public:
	explicit CMetalinkHandler(QFile& oFile = QFile());
	virtual ~CMetalinkHandler();

	virtual CDownload* file(const quint16& ID) const = 0;
	QList<CDownload*> files() const;

	inline int size() const;
	inline bool isNull() const;

protected:
	void postParsingError( const int line, const QString sError ) const;
	void postParsingInfo( const int line, const QString sInfo ) const;
};

int CMetalinkHandler::size() const
{
	return m_lFiles.size();
}

bool CMetalinkHandler::isNull() const
{
	return m_bNull;
}

#endif // METALINKHANDLER_H
