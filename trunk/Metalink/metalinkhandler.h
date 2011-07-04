#ifndef CMETALINKHANDLER_H
#define CMETALINKHANDLER_H

#include <QObject>

#include "download.h"

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

#endif // CMETALINKHANDLER_H
