#include <QMultiMap>
#include <QObject>

#include "magnetlink.h"
#include "systemlog.h"

#include "NetworkCore/Hashes/hash.h"

CMagnetLink::File::File() :
	m_nID( 0 ),
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

	int pos, pos2, nFileNo;
	QString sSubsection, sParam;

	QMap<quint16, File> mFiles;
	QList<QPair<QString, QString>> lGlobal;

	while ( sMagnet.length() )
	{
		pos = sMagnet.indexOf( "&" );
		pos = ( pos == -1 ) ? sMagnet.length() : pos;	// determine the end of the current subsection

		sSubsection = sMagnet.left( pos );	// isolate current subsection
		sMagnet.remove( 0, ++pos );		// remove current subsection from string

		pos2 = sSubsection.indexOf( "=" );	// get parameter type string length

		if ( pos2 == -1 )
		{
			QString sub = QObject::tr( "Subsection: %1" ).arg( sSubsection );
			systemLog.postLog( LogSeverity::Error, QObject::tr("Error while parsing subsection of Magnet Link:") );
			systemLog.postLog( LogSeverity::Error, m_sMagnet );
			systemLog.postLog( LogSeverity::Error, sub );
			continue;
		}

		sParam = sSubsection.left( pos2 );			// get parameter type string
		sSubsection.remove( 0, sParam.length() + 1 );	// isolate current subsection content

		if ( sParam.length() == 2 || sParam.length() > 3 && sParam[2] == '.' )
		{
			if ( sParam.length() == 2 )
			{
				nFileNo = 65535; // = 2^16 - 1;
			}
			else if ( sParam.length() > 3 && sParam[2] == '.' )
			{
				bool bOK;
				nFileNo = sParam.mid( 3 ).toInt( &bOK );

				if ( !bOK || nFileNo < 0 )
				{
					subsectionError( sParam, sSubsection );
					continue;
				}
			}
			else
			{
				subsectionError( sParam, sSubsection );
				continue;
			}

			// Make sure a file with the ID we've just isolated exists within the map.
			mFiles[nFileNo].m_nID = nFileNo;

			if ( sParam.startsWith( "dn" ) )		// Display Name
			{
				mFiles[nFileNo].m_sFileName = sSubsection;
			}
			else if ( sParam.startsWith( "xl" ) )	// EXact Length
			{
				bool bOK;
				qulonglong nFileSize = sSubsection.toULongLong( &bOK );

				if ( bOK )
				{
					mFiles[nFileNo].m_nFileSize = nFileSize;
				}
				else
				{
					subsectionError( sParam, sSubsection );
					continue;
				}
			}
			else if ( sParam.startsWith( "xt" ) )	// EXect Topic
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
				QString sub = QObject::tr( "Experimental Subsection: %1=%2" ).arg( sParam, sSubsection );
				systemLog.postLog( LogSeverity::Error, QObject::tr("Detected unsupported experimental subsection in Magnet Link:") );
				systemLog.postLog( LogSeverity::Error, m_sMagnet );
				systemLog.postLog( LogSeverity::Error, sub );
			}
			else
			{
				QString sub = QObject::tr( "Subsection: %1=%2" ).arg( sParam, sSubsection );
				systemLog.postLog( LogSeverity::Error, QObject::tr("Detected unknown type of subsection within Magnet Link:") );
				systemLog.postLog( LogSeverity::Error, m_sMagnet );
				systemLog.postLog( LogSeverity::Error, sub );
			}
		}
		else
		{
			subsectionError( sParam, sSubsection );
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

void CMagnetLink::subsectionError(QString sParam, QString sSubsection)
{
	QString sub = QObject::tr( "Subsection: %1=%2" ).arg( sParam, sSubsection );
	systemLog.postLog( LogSeverity::Error, QObject::tr("Error while parsing subsection of Magnet Link:") );
	systemLog.postLog( LogSeverity::Error, m_sMagnet );
	systemLog.postLog( LogSeverity::Error, sub );
}
