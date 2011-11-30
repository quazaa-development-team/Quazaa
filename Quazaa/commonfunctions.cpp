/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2011.
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
#include <QSysInfo>

#ifdef _DEBUG
#include "debug_new.h"
#endif

void common::folderOpen(QString file)
{
	QDir completePath( file );

	if ( !completePath.exists() )
	{
		completePath.mkpath( file );
	}
	QDesktopServices::openUrl( QUrl::fromLocalFile(file) );
}

QString common::formatBytes(quint64 nBytesPerSec)
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

QString common::vendorCodeToName(QString vendorCode)
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

OSVersion::OSVersion common::osVersion()
{
#ifdef Q_OS_LINUX
	return OSVersion::Linux;
#endif
#ifdef Q_OS_FREEBSD
	return OSVersion::BSD;
#endif
#ifdef Q_OS_NETBSD
	return OSVersion::BSD;
#endif
#ifdef Q_OS_OPENBSD
	return OSVersion::BSD;
#endif
#ifdef Q_OS_UNIX
	return OSVersion::Unix;
#endif
#ifdef Q_OS_MAC
	switch ( QSysInfo::MacintoshVersion )
	{
	case QSysInfo::MV_CHEETAH:
		return OSVersion::MacCheetah;
	case QSysInfo::MV_PUMA:
		return OSVersion::MacPuma;
	case QSysInfo::MV_JAGUAR:
		return OSVersion::MacJaguar;
	case QSysInfo::MV_PANTHER:
		return OSVersion::MacPanther;
	case QSysInfo::MV_TIGER:
		return OSVersion::MacTiger;
	case QSysInfo::MV_LOEPARD:
		return OSVersion::MacLeopard;
	case QSysInfo::MV_SNOWLEOPARD:
		return OSVersion::MacSnowLeopard;
	default:
		return OSVersion::MacSnowLeopard;
	}
#endif
#ifdef Q_OS_WIN
	switch ( QSysInfo::windowsVersion() )
	{
	case QSysInfo::WV_2000:
		return OSVersion::Win2000;
	case QSysInfo::WV_XP:
		return OSVersion::WinXP;
	case QSysInfo::WV_2003:
		return OSVersion::Win2003;
	case QSysInfo::WV_VISTA:
		return OSVersion::WinVista;
	case QSysInfo::WV_WINDOWS7:
		return OSVersion::Win7;
	default:
		return OSVersion::Win7;
	}
#endif
}

