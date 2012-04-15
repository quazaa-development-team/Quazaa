#ifndef METALINK4HANDLER_H
#define METALINK4HANDLER_H

#include <QFile>
#include <QXmlStreamReader>

#include "download.h"
#include "metalinkhandler.h"

class CMetalink4Handler : public CMetalinkHandler
{
	Q_OBJECT

private:

public:
	explicit CMetalink4Handler(QFile& meta4File = QFile(), QObject *parent = 0);

	CDownload* file(const unsigned int& ID);

signals:

public slots:

};

#endif // METALINK4HANDLER_H
