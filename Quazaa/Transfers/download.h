#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "types.h"
#include "filefragments.h"
#include "Hashes/hash.h"

class CDownloadSource;
class CQueryHit;

class CDownload : public QObject
{
    Q_OBJECT

public:
	struct FileListItem
	{
		QString sFileName;
		quint64 nStartOffset;
		quint64 nEndOffset;
		QList<CHash> lHashes;
	};
	enum DownloadState
	{
		dsQueued,
		dsPaused,
		dsSearching,
		dsPending,
		dsDownloading,
		dsVerifying,
		dsMoving,
		dsFileError,
		dsCompleted
	};

	enum Priority
	{
		HIGH = -1,
		NORMAL = 0,
		LOW = 1
	};

	QString					m_sDisplayName;
	QString					m_sTempName;
	quint64					m_nSize;
	quint64					m_nCompletedSize;
	DownloadState			m_nState;
	QList<CDownloadSource*> m_lSources;
	bool					m_bMultifile;
	QList<FileListItem>		m_lFiles;	// for multifile downloads
	CFileFragments			m_lCompleted;
	CFileFragments			m_lVerified;
	CFileFragments			m_lActive;
	QList<CHash>			m_lHashes; // hashes for whole download

	bool					m_bSignalSources;
	int						m_nPriority;
public:
	CDownload()
		:m_bSignalSources(false)
	{}
	CDownload(CQueryHit* pHit, QObject *parent = 0);
	~CDownload();

	void start();
	void pause();
	void cancelDownload();
	bool addSource(CDownloadSource* pSource);
	int  addSource(CQueryHit* pHit);
	bool sourceExists(CDownloadSource* pSource);
signals:
	void sourceAdded(CDownloadSource*);
public slots:
	void emitSources();
};

QDataStream& operator<<(QDataStream& s, const CDownload& rhs);
QDataStream& operator>>(QDataStream& s, CDownload& rhs);

#endif // DOWNLOAD_H
