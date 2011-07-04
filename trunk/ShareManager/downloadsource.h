#ifndef CDOWNLOADSOURCE_H
#define CDOWNLOADSOURCE_H

#include <QtGlobal>
#include <QObject>

#include "download.h"

class CDownloadSource : public QObject
{
    Q_OBJECT
private:
	quint32		m_tNextAccess;
	CDownload*	m_pOwner;

public:
	explicit CDownloadSource(CDownload* download = NULL, QObject *parent = NULL);

	bool setOwningDownload(const CDownload* download);
	void query();
	quint32 getSecToNextAccess(const quint32& tNow);
	bool schdeuleNextAccess(const quint32& tAccess);

signals:
	void requestQuery();

public slots:

};

#endif // CDOWNLOADSOURCE_H
