#ifndef METALINK4HANDLER_H
#define METALINK4HANDLER_H

#include <QFile>
#include <QList>
#include <QtXml>

#include "download.h"
#include "metalinkhandler.h"

class CMetalink4Handler : public CMetalinkHandler
{
	Q_OBJECT

private:
	bool			m_bNull;
	unsigned int	m_nSize;
	QDomDocument	metalink;
	QDomNodeList	m_lFiles;

public:
	explicit CMetalink4Handler(QFile& meta4File, QObject *parent = 0);

	inline unsigned int size();
	CDownload* file(const unsigned int& ID);
	QList<CDownload*> files();

signals:

public slots:

};

unsigned int CMetalink4Handler::size()
{
	return m_nSize;
}

#endif // METALINK4HANDLER_H
