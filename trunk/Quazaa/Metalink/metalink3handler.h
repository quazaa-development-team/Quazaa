#ifndef METALINK3HANDLER_H
#define METALINK3HANDLER_H

#include <QFile>
#include <QList>
#include <QXmlStreamReader>

#include "download.h"
#include "metalinkhandler.h"

class CMetalink3Handler : public CMetalinkHandler
{
	Q_OBJECT

private:

public:
	explicit CMetalink3Handler(QFile& meta4File = QFile(), QObject *parent = 0);

	CDownload* file(const unsigned int& ID);

signals:

public slots:

};

#endif // METALINK3HANDLER_H
