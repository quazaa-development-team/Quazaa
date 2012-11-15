/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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

#ifndef IRCFORMATTING_H
#define IRCFORMATTING_H

#include <IrcGlobal>
#include <QString>

#define IRC_DEFAULT_BACKGROUND 100
#define IRC_DEFAULT_FOREGROUND 101

class COMMUNI_EXPORT IrcFormatting
{
public:
	enum ColorCode {
		White			=   0,   /**< White */
		Black			=   1,   /**< Black */
		DarkBlue		=   2,   /**< Dark blue */
		DarkGreen		=   3,   /**< Dark green */
		Red				=   4,   /**< Red */
		DarkRed			=   5,   /**< Dark red */
		DarkViolet		=   6,   /**< Dark violet */
		Orange			=   7,   /**< Orange */
		Yellow			=   8,   /**< Yellow */
		LightGreen		=   9,   /**< Light green */
		Cyan			=  10,   /**< Cornflower blue */
		LightCyan		=  11,   /**< Light blue */
		Blue			=  12,   /**< Blue */
		Violet			=  13,   /**< Violet */
		DarkGray	    =  14,   /**< Dark gray */
		LightGray		=  15,   /**< Light gray */
		Transparent		= 100,   /**< Transparent, non standard color code for IrcViews */
		NoChange		= 101    /**< No change, non standard color code for IrcViews */
	};

	/**
	* \enum ControlCode
	* \brief Contains mIRC control codes
	*/
	enum ControlCode {
		Bold			= 0x02,		/**< Bold */
		Color			= 0x03,		/**< Color */
		Italic			= 0x09,		/**< Italic */
		StrikeThrough	= 0x13,		/**< Strike-Through */
		Reset			= 0x0f,		/**< Reset */
		Underline		= 0x15,		/**< Underline */
		Underline2		= 0x1f,		/**< Underline */
		Reverse			= 0x16		/**< Reverse */
	};

	static QString colorName(uint code, const QString& defaultColor = QLatin1String("black"));
	static void setColorName(uint code, const QString& color);
	static uint getUnicodeColorBytes(const QString & szData, unsigned int iChar, unsigned char * pcByte1, unsigned char * pcByte2);
	static QString stripFormatting(QString strip);
};

#endif // IRCFORMATTING_H
