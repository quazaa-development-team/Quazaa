#include <QMultiMap>
#include <QObject>

#include "magnetlink.h"
#include "systemlog.h"

#include "NetworkCore/Hashes/hash.h"

CMagnetLink::File::File() :
	m_nID( 0 ),
	m_nFileSize( 0 )
{}

CMagnetLink::File::~File()
{
	foreach( CHash* pHash, m_lHashes )
	{
		delete pHash;
	}
}

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
	m_lSearches.clear();

	sMagnet.remove( 0, 8 );

	int pos, pos2, nFileNo;
	QString sSubsection, sParam;

	QMap<quint16, File> mFiles;
	QMap<quint16, QString> mSearches;

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
			// Determine ID the current file.
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
				CHash* pHash = CHash::FromURN( sSubsection );

				if ( pHash )
				{
					mFiles[nFileNo].m_lHashes.append( pHash );
				}
				else
				{
					QString sub = QObject::tr( "Hash URN: %1" ).arg( sSubsection );
					systemLog.postLog( LogSeverity::Error, QObject::tr("Detected unknown hash type within Magnet Link:") );
					systemLog.postLog( LogSeverity::Error, m_sMagnet );
					systemLog.postLog( LogSeverity::Error, sub );

					continue;
				}
			}
			else if ( sParam.startsWith( "as" ) || sParam.startsWith( "xs" ) )
			{
				MediaURL url;
				url.m_oURL		= QUrl( sSubsection );

				if ( url.m_oURL.isValid() )
				{
					url.m_nPriority = sParam.startsWith( "xs" ) ? 128 : 0;
					mFiles[nFileNo].m_lURLs.append( url );
				}
				else
				{
					subsectionError( sParam, sSubsection );
					continue;
				}
			}
			else if ( sParam.startsWith( "kt" ) )
			{
				mSearches[nFileNo] = sSubsection;
			}
			else if ( sParam.startsWith( "mt" ) )
			{
				QString sub = QObject::tr( "Manifest Topic: %1=%2" ).arg( sParam, sSubsection );
				systemLog.postLog( LogSeverity::Error, QObject::tr("Detected unsupported \"Manifest Topic\" section in Magnet Link:") );
				systemLog.postLog( LogSeverity::Error, m_sMagnet );
				systemLog.postLog( LogSeverity::Error, sub );
			}
			else if ( sParam.startsWith( "tr" ) )
			{
				QUrl url = QUrl( sSubsection );

				if ( url.isValid() )
				{
					mFiles[nFileNo].m_lTrackers.append( url );
				}
				else
				{
					subsectionError( sParam, sSubsection );
					continue;
				}
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
	}

	quint16 nID = 0;
	foreach ( File oFile, mFiles )
	{
		if ( oFile.isValid() )
		{
			oFile.m_nID = nID++;
			m_lFiles.push_back( oFile );
		}
	}

	foreach ( QString sSearch, mSearches )
	{
		if ( !sSearch.trimmed().isEmpty() )
		{
			m_lSearches.push_back( sSearch );
		}
	}

	return m_lFiles.size() + m_lSearches.size();
}

void CMagnetLink::subsectionError(QString sParam, QString sSubsection)
{
	QString sub = QObject::tr( "Subsection: %1=%2" ).arg( sParam, sSubsection );
	systemLog.postLog( LogSeverity::Error, QObject::tr("Error while parsing subsection of Magnet Link:") );
	systemLog.postLog( LogSeverity::Error, m_sMagnet );
	systemLog.postLog( LogSeverity::Error, sub );
}
