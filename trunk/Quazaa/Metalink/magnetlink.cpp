#include <QMultiMap>
#include <QObject>

#include "magnetlink.h"
#include "systemlog.h"

#include "NetworkCore/Hashes/hash.h"

CMagnetLink::File::File(quint16 ID) :
	m_nID( ID ),
	m_nFileSize( 0 )
{}

bool CMagnetLink::File::isValid()
{

	// todo: implement

	return true;
}

CMagnetLink::CMagnetLink()
{
}

CMagnetLink::CMagnetLink(QString sMagnet)
{
	parseMagnet( sMagnet );
}

bool CMagnetLink::parseMagnet(QString sMagnet)
{
	if ( !sMagnet.startsWith( "magnet:?" ) )
		return false;

	m_sMagnet = sMagnet;
	m_lFiles.clear();

	sMagnet.remove( 0, 8 );

	int pos, pos2;
	QString sTmp, sParam;

	QMap<quint16, File> mFiles;
	QList<QPair<QString, QString>> lGlobal;

	while ( sMagnet.length() )
	{
		pos = sMagnet.indexOf( "&" );
		pos = ( pos == -1 ) ? sMagnet.length() : pos;

		sTmp = sMagnet.left( pos );
		sMagnet.remove( 0, ++pos );

		pos2 = sTmp.indexOf( "=" );

		if ( pos2 == -1 )
		{
			QString sub = QObject::tr( "Subsection: %1" ).arg( sTmp );
			systemLog.postLog( LogSeverity::Error, QObject::tr("Error while parsing subsection of Magnet Link:") );
			systemLog.postLog( LogSeverity::Error, m_sMagnet );
			systemLog.postLog( LogSeverity::Error, sub );

			continue;
		}

		sParam = sTmp.left( pos2 );
		sTmp.remove( 0, sParam.length() + 1 );

		if ( sParam.startsWith( "dn" ) )
		{

		}
		else if ( sParam.startsWith( "xl" ) )
		{

		}
		else if ( sParam.startsWith( "xt" ) )
		{

		}
		else if ( sParam.startsWith( "as" ) )
		{

		}
		else if ( sParam.startsWith( "xs" ) )
		{

		}
		else if ( sParam.startsWith( "kt" ) )
		{

		}
		else if ( sParam.startsWith( "mt" ) )
		{

		}
		else if ( sParam.startsWith( "tr" ) )
		{

		}
		else if ( sParam.startsWith( "x." ) ) // Experimental stuff
		{
			QString sub = QObject::tr( "Experimental Subsection: %1=%2" ).arg( sParam, sTmp );
			systemLog.postLog( LogSeverity::Error, QObject::tr("Detected unsupported experimental subsection in Magnet Link:") );
			systemLog.postLog( LogSeverity::Error, m_sMagnet );
			systemLog.postLog( LogSeverity::Error, sub );
		}
		else
		{
			QString sub = QObject::tr( "Subsection: %1=%2" ).arg( sParam, sTmp );
			systemLog.postLog( LogSeverity::Error, QObject::tr("Detected unknown type of subsection within Magnet Link:") );
			systemLog.postLog( LogSeverity::Error, m_sMagnet );
			systemLog.postLog( LogSeverity::Error, sub );
		}

		// TODO: continue here
	}

	// TODO: continue here

	quint16 nID = 0;
	foreach ( File oFile, mFiles )
	{
		if ( oFile.isValid() )
		{
			oFile.m_nID = nID++;
			m_lFiles.push_back( oFile );
		}
	}

	return m_lFiles.size();
}


