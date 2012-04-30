#ifndef METALINK4HANDLER_H
#define METALINK4HANDLER_H

#include <QFile>

#include "download.h"
#include "metalinkhandler.h"

class CMetalink4Handler : public CMetalinkHandler
{
	Q_OBJECT

private:
	quint16 nID;

public:
	explicit CMetalink4Handler(QFile& oFile = QFile(), QObject *parent = 0);

	CDownload* file(const unsigned int& ID);

private:
	bool parseFile(quint16 ID);

signals:

public slots:

};

#endif // METALINK4HANDLER_H
