#include "metalink3handler.h"

CMetalink3Handler::CMetalink3Handler(QFile& meta4File, QObject *parent) :
	CMetalinkHandler( meta4File, parent )
{
	bool bError = true;



	m_bNull = bError;
}

CDownload* CMetalink3Handler::file(const unsigned int& ID)
{
	return new CDownload();
}
