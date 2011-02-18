/*
* Copyright (C) 2008-2009 J-P Nurmi jpnurmi@gmail.com
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*/

#ifndef IRC_UTIL_H
#define IRC_UTIL_H

#include <irc.h>
#include <QPair>

namespace Irc
{
	class IRC_EXPORT Util
    {
	public:
		static QString nickFromTarget(const QString& target);
		static QString hostFromTarget(const QString& target);
		static void replaceDecoration(QString& line, char decoration, char replacement);
		static QString messageToHtml(const QString& line, const QString& defaultColor = "#000000", bool parseURL = true, bool allowColors = true);
		static QString extractUrlData(QString text, bool doHyperlinks);
		static QString ircColorCode(int code);
	};
}

#endif // IRC_UTIL_H
