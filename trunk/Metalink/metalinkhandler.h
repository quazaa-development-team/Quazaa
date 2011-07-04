#ifndef METALINKHANDLER_H
#define METALINKHANDLER_H

#include <QObject>

class QFile;
class CDownload;

// Pure virtual class for all Metalink handlers to inherit from.
class CMetalinkHandler : public QObject
{
	Q_OBJECT

public:
	explicit CMetalinkHandler(QFile& metalinkFile, QObject *parent = 0);

	virtual unsigned int size() = 0;
	virtual CDownload* file(const unsigned int& ID) = 0;
	virtual QList<CDownload*> files() = 0;

signals:

public slots:

};

#endif // METALINKHANDLER_H
