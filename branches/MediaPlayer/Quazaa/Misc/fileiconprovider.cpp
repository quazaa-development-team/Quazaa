/*
** $Id: fileiconprovider.cpp 929 2012-02-16 14:32:59Z brov $
**
** Copyright © Quazaa Development Team, 2009-2012.
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

#include "fileiconprovider.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

#if defined(Q_WS_WIN)
#  define _WIN32_IE 0x0500
#  include <qt_windows.h>
#  include <commctrl.h>
#  include <objbase.h>
#  include <qpixmapcache.h>
#  include <qdir.h>
#endif

#ifdef Q_OS_WIN

#ifndef SHGFI_ADDOVERLAYS
#define SHGFI_ADDOVERLAYS 0x000000020
#define SHGFI_OVERLAYINDEX 0x000000040
#endif

#ifndef SHGFI_USEFILEATTRIBUTES
#define SHGFI_USEFILEATTRIBUTES 0x000000010
#endif

// QFileIconProviderPrivate::getWinIcon copy modified to get icons for non-existing files
QIcon getWinIcon(const QFileInfo &fileInfo)
{
	QIcon retIcon;
	const QString fileExtension = QLatin1Char('.') + fileInfo.suffix().toUpper();

	QString key;
	if (fileInfo.isFile() && !fileInfo.isExecutable() && !fileInfo.isSymLink() && fileExtension != QLatin1String(".ICO"))
		key = QLatin1String("qt_") + fileExtension;

	QPixmap pixmap;
	if (!key.isEmpty()) {
		QPixmapCache::find(key, pixmap);
	}

	if (!pixmap.isNull()) {
		retIcon.addPixmap(pixmap);
		if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
			retIcon.addPixmap(pixmap);
		return retIcon;
	}

	/* We don't use the variable, but by storing it statically, we
	 * ensure CoInitialize is only called once. */
	static HRESULT comInit = CoInitialize(NULL);
	Q_UNUSED(comInit);

	SHFILEINFO info;
	unsigned long val = 0;

	//Get the small icon
#ifndef Q_OS_WINCE
	val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
						sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS|SHGFI_OVERLAYINDEX|SHGFI_USEFILEATTRIBUTES);
#else
	val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
						sizeof(SHFILEINFO), SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES);
#endif

	// Even if GetFileInfo returns a valid result, hIcon can be empty in some cases
	if (val && info.hIcon) {
		if (fileInfo.isDir() && !fileInfo.isRoot()) {
			//using the unique icon index provided by windows save us from duplicate keys
			key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
			QPixmapCache::find(key, pixmap);
			if (!pixmap.isNull()) {
				retIcon.addPixmap(pixmap);
				if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
					retIcon.addPixmap(pixmap);
				DestroyIcon(info.hIcon);
				return retIcon;
			}
		}
		if (pixmap.isNull()) {
#ifndef Q_OS_WINCE
			pixmap = QPixmap::fromWinHICON(info.hIcon);
#else
			pixmap = QPixmap::fromWinHICON(ImageList_GetIcon((HIMAGELIST) val, info.iIcon, ILD_NORMAL));
#endif
			if (!pixmap.isNull()) {
				retIcon.addPixmap(pixmap);
				if (!key.isEmpty())
					QPixmapCache::insert(key, pixmap);
			}
			else {
			  qWarning("QFileIconProviderPrivate::getWinIcon() no small icon found");
			}
		}
		DestroyIcon(info.hIcon);
	}

	//Get the big icon
#ifndef Q_OS_WINCE
	val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
						sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS|SHGFI_OVERLAYINDEX|SHGFI_USEFILEATTRIBUTES);
#else
	val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
						sizeof(SHFILEINFO), SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES);
#endif
	if (val && info.hIcon) {
		if (fileInfo.isDir() && !fileInfo.isRoot()) {
			//using the unique icon index provided by windows save us from duplicate keys
			key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
		}
#ifndef Q_OS_WINCE
		pixmap = QPixmap::fromWinHICON(info.hIcon);
#else
		pixmap = QPixmap::fromWinHICON(ImageList_GetIcon((HIMAGELIST) val, info.iIcon, ILD_NORMAL));
#endif
		if (!pixmap.isNull()) {
			retIcon.addPixmap(pixmap);
			if (!key.isEmpty())
				QPixmapCache::insert(key + QLatin1Char('l'), pixmap);
		}
		else {
			qWarning("::getWinIcon() no large icon found");
		}
		DestroyIcon(info.hIcon);
	}
	return retIcon;

}
#endif

CFileIconProvider::CFileIconProvider()
	:QFileIconProvider()
{
}

QIcon CFileIconProvider::icon(const QFileInfo &info) const
{
#ifdef Q_OS_WIN
	QIcon icon = getWinIcon(info);
	if( !icon.isNull() )
		return icon;
#endif

	return QFileIconProvider::icon(info);
}

QIcon CFileIconProvider::icon(const QString &file) const
{
	QFileInfo info(file);
	return icon(info);
}
