#ifndef METALINKHANDLER_H
#define METALINKHANDLER_H

#include <QObject>

#include <QList>
#include <QFile>
#include <QXmlStreamReader>

#include "download.h"

class CMetalinkHandler : public QObject
{
	Q_OBJECT
protected:
	bool					m_bNull; // true if invalid
	quint32					m_nSize; // number of files in metalink
	QXmlStreamReader		m_oMetaLink;
	QXmlStreamReader::Error m_nParsingState;

public:
	explicit CMetalinkHandler(QFile& oFile = QFile(), QObject *parent = 0);
	virtual ~CMetalinkHandler();

	virtual CDownload* file(const unsigned int& ID) = 0;
	QList<CDownload*> files();

signals:

public slots:

};

#endif // METALINKHANDLER_H
