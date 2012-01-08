#ifndef DOWNLOADS_H
#define DOWNLOADS_H

#include <QObject>
#include <QMutex>

class CQueryHit;
class CDownload;

class CDownloads : public QObject
{
    Q_OBJECT
public:
	QMutex m_pSection;

	QList<CDownload*> m_lDownloads;
public:
	CDownloads(QObject *parent = 0);

	void start();
	void stop();

	void add(CQueryHit* pHit);

	bool exists(CDownload* pDownload);
signals:
	void downloadAdded(CDownload*);
	void downloadRemoved();
public slots:
	void emitDownloads();
};

extern CDownloads Downloads;

#endif // DOWNLOADS_H
