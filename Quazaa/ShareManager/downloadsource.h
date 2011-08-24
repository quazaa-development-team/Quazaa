#ifndef CDOWNLOADSOURCE_H
#define CDOWNLOADSOURCE_H

#include <QtGlobal>
#include <QObject>
#include <QUuid>

#include "download.h"

class CDownloadSource : public QObject
{
    Q_OBJECT
private:
	quint32		m_tNextAccess;	// Time (in seconds since 1970...) of next source access.
	CDownload*	m_pOwner;		// Download owning this source.
	QUuid		m_oUUID;		// Unique source identifier.

public:
	explicit CDownloadSource(CDownload* download = NULL, QObject *parent = NULL);

	void setOwningDownload(CDownload* download);

	quint32 secToNextAccess(const quint32& tNow);
	bool schdeuleNextAccess(const quint32& tAccess);

	// Returns the unique source identifier.
	inline QUuid UUID() const;

signals:

public slots:
	// Queries the source. This needs to be implemented by inheriting source classes.
	virtual void query() = 0;

};

QUuid CDownloadSource::UUID() const
{
	return m_oUUID;
}

#endif // CDOWNLOADSOURCE_H
