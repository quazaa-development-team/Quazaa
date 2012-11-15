/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
* Copyright (C) 2010-2011 SmokeX <smokexjc@gmail.com>
* Copyright (C) 2012 Mark Johnson <marknotgeorge@googlemail.com>
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

#include "ircformatting.h"
#include <QHash>

/*!
	\file ircformatting.h
	\brief #include &lt;IrcFormatting&gt;
 */

/*!
	\class IrcFormatting ircformatting.h <IrcFormatting>
	\ingroup utility
	\brief The IrcFormatting class provides IRC formatting codes and conversion of color codes to strings

	\sa http://www.mirc.com/colors.html
 */

static QHash<uint, QString>& irc_colors()
{
	static QHash<uint, QString> x;
	if (x.isEmpty()) {
		x.insert(IrcFormatting::White, QLatin1String("white"));
		x.insert(IrcFormatting::Black, QLatin1String("black"));
		x.insert(IrcFormatting::DarkBlue, QLatin1String("navy"));
		x.insert(IrcFormatting::DarkGreen, QLatin1String("green"));
		x.insert(IrcFormatting::Red, QLatin1String("red"));
		x.insert(IrcFormatting::DarkRed, QLatin1String("maroon"));
		x.insert(IrcFormatting::DarkViolet, QLatin1String("purple"));
		x.insert(IrcFormatting::Orange, QLatin1String("olive"));
		x.insert(IrcFormatting::Yellow, QLatin1String("yellow"));
		x.insert(IrcFormatting::LightGreen, QLatin1String("lime"));
		x.insert(IrcFormatting::Cyan, QLatin1String("teal"));
		x.insert(IrcFormatting::LightCyan, QLatin1String("aqua"));
		x.insert(IrcFormatting::Blue, QLatin1String("royalblue"));
		x.insert(IrcFormatting::Violet, QLatin1String("fuchsia"));
		x.insert(IrcFormatting::DarkGray, QLatin1String("gray"));
		x.insert(IrcFormatting::LightGray, QLatin1String("lightgray"));
		x.insert(IrcFormatting::Transparent, QLatin1String("transparent"));
	}
	return x;
}

/*!
	Converts a color \a code to a color name. If the color \a code
	is unknown, the function returns \a defaultColor.

	\sa setColorName()
*/
QString IrcFormatting::colorName(uint code, const QString& defaultColor)
{
	return irc_colors().value(code, defaultColor);
}

/*!
	Assigns a \a color name for \a code.

	\sa colorName()
*/
void IrcFormatting::setColorName(uint code, const QString& color)
{
	if((code != 100) | (code != 101)) //These colors are protected and should not be changable
		irc_colors().insert(code, color);
}

/*!
	Scans the szData for a mIrc color code XX,XX
	and fills the color values in the two bytes
*/

uint IrcFormatting::getUnicodeColorBytes(const QString & szData, uint iChar, uchar * pcByte1, uchar * pcByte2)
{
	if(iChar >= (uint)szData.length())
	{
		(*pcByte1) = IrcFormatting::NoChange;
		(*pcByte2) = IrcFormatting::NoChange;
		return iChar;
	}

	ushort c = szData[(int)iChar].unicode();

	//First we can have a digit or a comma
	if(((c < '0') || (c > '9')))
	{
		// senseless : only a CTRL+K code
		(*pcByte1) = IrcFormatting::NoChange;
		(*pcByte2) = IrcFormatting::NoChange;
		return iChar;
	}

	//Something interesting ok.
	(*pcByte1) = c - '0'; //store the code
	iChar++;
	if(iChar >= (uint)szData.length())
	{
		(*pcByte2) = IrcFormatting::NoChange;
		return iChar;
	}

	c = szData[(int)iChar].unicode();

	if(((c < '0') || (c > '9')) && (c != ','))
	{
		(*pcByte2) = IrcFormatting::NoChange;
		return iChar;
	}

	if((c >= '0') && (c <= '9'))
	{
		(*pcByte1) = (((*pcByte1)*10)+(c-'0'))%16;
		iChar++;
		if(iChar >= (uint)szData.length())
		{
			(*pcByte2) = IrcFormatting::NoChange;
			return iChar;
		}
		c = szData[(int)iChar].unicode();
	}

	if(c == ',')
	{
		iChar++;
		if(iChar >= (uint)szData.length())
		{
			(*pcByte2) = IrcFormatting::NoChange;
			return iChar;
		}
		c = szData[(int)iChar].unicode();
	} else {
		(*pcByte2) = IrcFormatting::NoChange;
		return iChar;
	}

	if((c < '0') || (c > '9'))
	{
		(*pcByte2) = IrcFormatting::NoChange;
		if(szData[(int)(iChar-1)].unicode()==',')
			return iChar-1;
		else
			return iChar;
	}

	//Background, a color code
	(*pcByte2) = c-'0';
	iChar++;
	if(iChar >= (uint)szData.length())
		return iChar;
	c = szData[(int)iChar].unicode();

	if((c >= '0') && (c <='9'))
	{
		(*pcByte2) = (((*pcByte2)*10) + (c-'0')) % 16;
		iChar++;
	}

	return iChar;
}

/*!
	Strips the formatting codes out of a given string. Useful for links.
*/


QString IrcFormatting::stripFormatting(QString strip)
{
	bool bEndWithReset = false;
	QString processed = strip;

	uint uIndex = 0;

	while(uIndex < (uint)processed.length())
	{
		ushort character = processed[(int)uIndex].unicode();
		switch(character)
		{
			case IrcFormatting::Bold:
			{
				processed.remove((int)uIndex, 1);
				break;
			}
			case IrcFormatting::Color:
			{
				processed.remove((int)uIndex, 1);
				if (uIndex < (uint)processed.length())
				{
					uchar ucForeground;
					uchar ucBackground;

					ushort colorCharacters = IrcFormatting::getUnicodeColorBytes(processed,uIndex,&ucForeground,&ucBackground);

					processed.remove((int)uIndex, (int)colorCharacters);
				}
				break;
			}
			case IrcFormatting::Italic:
				processed.remove((int)uIndex, 1);
				break;
			case IrcFormatting::StrikeThrough:
				processed.remove((int)uIndex, 1);
				break;
			case IrcFormatting::Reset:
			{
				processed.remove((int)uIndex, 1);
				bEndWithReset = true;
				break;
			}
			case IrcFormatting::Underline:
			case IrcFormatting::Underline2:
			{
				processed.remove((int)uIndex, 1);
				break;
			}
			case IrcFormatting::Reverse:
			{
				processed.remove((int)uIndex, 1);
				break;
			}
			default:
			{
				++uIndex;
			}
		}
	}

	return processed;
}
