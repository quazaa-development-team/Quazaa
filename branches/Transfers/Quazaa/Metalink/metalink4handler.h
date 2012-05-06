#ifndef METALINK4HANDLER_H
#define METALINK4HANDLER_H

#include <QFile>

#include "download.h"
#include "metalinkhandler.h"

class CMetalink4Handler : public CMetalinkHandler
{

private:
	quint16 nID;

public:
	explicit CMetalink4Handler(QFile& oFile = QFile());

	CDownload* file(const unsigned int& ID) const;

private:
	bool parseFile(quint16 ID);

};

#endif // METALINK4HANDLER_H
