/*
** commonfunctions.cpp
**
** Copyright  Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "commonfunctions.h"
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

CommonFunctions Functions;

CommonFunctions::CommonFunctions()
{
}

void CommonFunctions::FolderOpen(QString file)
{
	QDir completePath( file );

	if ( !completePath.exists() )
	{
		completePath.mkpath( file );
	}
	QDesktopServices::openUrl( QUrl( "file:///" + file, QUrl::TolerantMode ) );
}

QString CommonFunctions::FormatBytes(quint64 nBytesPerSec)
{
	const char* szUnit[4] = {"B", "KB", "MB", "GB"};

	double nBPS = nBytesPerSec;

	int nStep = 0;
	while ( nBPS > 1024 )
	{
		nBPS /= 1024;
		nStep++;
		if ( nStep == 3 )
		{
			break;
		}
	}

	if ( nStep )
	{
		return QString().sprintf( "%1.2f %s", nBPS, szUnit[nStep] );
	}
	else
	{
		return QString().sprintf( "%1.0f %s", nBPS, szUnit[nStep] );
	}
}

QString CommonFunctions::VendorCodeToName(QString vendorCode)
{
	if ( vendorCode == "RAZA" )
	{
		return "Shareaza";
	}
	if ( vendorCode == "RAZB" )
	{
		return "Shareaza Beta";
	}
	if ( vendorCode == "QAZA" )
	{
		return "Quazaa";
	}
	if (vendorCode == "QAZB" )
	{
		return "Quazaa Beta";
	}
	if (vendorCode == "SHLN" )
	{
		return "Sharelin";
	}
	if (vendorCode == "RZCB" )
	{
		return "Shareaza Plus";
	}
	if (vendorCode == "FSCP" )
	{
		return "FileScope";
	}
	if (vendorCode == "AGIO" )
	{
		return "Adiago";
	}
	if (vendorCode == "GNUC" )
	{
		return "Gnucleus";
	}
	if (vendorCode == "MLDK" )
	{
		return "MLDonkey";
	}
	if (vendorCode == "MMMM" )
	{
		return "Morpheus";
	}
	if (vendorCode == "PEER" )
	{
		return "PeerProject";
	}
	else
	{
		return vendorCode;
	}
}
