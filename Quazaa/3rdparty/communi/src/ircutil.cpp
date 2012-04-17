/*
* Copyright (C) 2008-2011 J-P Nurmi <jpnurmi@gmail.com>
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

/*
  Parts of this code come from KvIrc and are copyrighted to:
  Copyright (C) 2010 Fabio Bas < ctrlaltca at gmail dot com >
*/

#include "ircutil.h"
#include <QStringList>
#include <QRegExp>
#include <QTextDocument> // for Qt::escape
#include <QDebug>

#define IRC_DEFAULT_BACKGROUND 100
#define IRC_DEFAULT_FOREGROUND 101

/*!
    \file ircutil.h
    \brief #include &lt;IrcUtil&gt;
 */

/*!
    \class IrcUtil ircutil.h <IrcUtil>
    \ingroup utility
    \brief The IrcUtil class provides miscellaneous utility functions.
 */

static QRegExp URL_PATTERN(QLatin1String("((www\\.(?!\\.)|(ssh|fish|irc|amarok|(f|sf|ht)tp(|s))://)(\\.?[\\d\\w/,\\':~\\^\\?=;#@\\-\\+\\%\\*\\{\\}\\!\\(\\)\\[\\]]|&)+)|""([-.\\d\\w]+@[-.\\d\\w]{2,}\\.[\\w]{2,})"), Qt::CaseInsensitive);

/*!
    Converts \a message to HTML. This function parses the
    message and replaces IRC-style formatting like colors,
    bold and underline to the corresponding HTML formatting.
    Furthermore, this function detects URLs and replaces
    them with appropriate HTML hyperlinks.
*/
QString IrcUtil::messageToHtml(const QString& message, bool bEscape, bool bShowIcons)
{
	QString result;
	bool bCurrentBold      = false;
	bool bCurrentItalic    = false;
	bool bCurrentStrikeThrough = false;
	bool bCurrentUnderline = false;
	bool bIgnoreIcons  = false;

	unsigned char ucCurrentForeground = IRC_DEFAULT_FOREGROUND; //default foreground
	unsigned char ucCurrentBackground = IRC_DEFAULT_BACKGROUND; //default background

	unsigned int uIndex = 0;

	QString processed = bEscape ? Qt::escape(message) : message;

	while(uIndex < (unsigned int)processed.length())
	{
		unsigned short character = processed[(int)uIndex].unicode();
		unsigned int uStart = uIndex;

		while(
			(character != IrcControlCodes::Bold) &&
			(character != IrcControlCodes::Color) &&
			(character != IrcControlCodes::Italic) &&
			(character != IrcControlCodes::StrikeThrough) &&
			(character != IrcControlCodes::Reset) &&
			(character != IrcControlCodes::Underline) &&
			(character != IrcControlCodes::Underline2) &&
			(character != IrcControlCodes::Reverse) &&
			((character != ':') || bIgnoreIcons) &&
			((character != ';') || bIgnoreIcons) &&
			((character != '=') || bIgnoreIcons))
		{
			bIgnoreIcons = false;
			if(character == '&')
			{
				//look for an html entity
				QString szEntity = processed.mid((int)uIndex,6);
				if(szEntity == "&quot;")
				{
					uIndex += 5;
				} else {
					szEntity.truncate(5);
					if(szEntity == "&amp;")
					{
						 uIndex += 4;
					} else {
						szEntity.truncate(4);
						if(szEntity == "&lt;" || szEntity == "&gt;")
							uIndex += 3;
					}
				}
			}

			uIndex++;
			if(uIndex >= (unsigned int)processed.length())
				break;

			character = processed[(int)uIndex].unicode();
		}

		bIgnoreIcons = false;
		int iLength = uIndex - uStart;

		if(iLength > 0)
		{
			bool bOpened = false;

			if(ucCurrentForeground != IRC_DEFAULT_FOREGROUND)
			{
				result.append("<span style=\"color:");
				result.append(colorCodeToName(ucCurrentForeground));
				bOpened = true;
			}

			if(ucCurrentBackground != IRC_DEFAULT_BACKGROUND)
			{
				if(!bOpened)
				{
					result.append("<span style=\"background-color:");
					bOpened = true;
				} else {
					result.append(";background-color:");
				}
				result.append(colorCodeToName(ucCurrentBackground));
			}

			if(bCurrentBold)
			{
				if(!bOpened)
				{
					result.append("<span style=\"font-weight:bold");
					bOpened = true;
				} else {
					result.append(";font-weight:bold");
				}
			}

			if(bCurrentItalic)
			{
				if(!bOpened)
				{
					result.append("<span style=\"font-style:italic");
					bOpened = true;
				} else {
					result.append(";font-style:italic");
				}
			}

			if(bCurrentStrikeThrough)
			{
				if(!bOpened)
				{
					result.append("<span style=\"text-decoration:line-through");
					bOpened = true;
				} else {
					result.append(";text-decoration:line-through");
				}
			}

			if(bCurrentUnderline)
			{
				if(!bOpened)
				{
					result.append("<span style=\"text-decoration:underline");
					bOpened = true;
				} else {
					result.append(";text-decoration:underline");
				}
			}

			if(bOpened)
				result.append(";\">");

			result.append(processed.mid(uStart,iLength));

			if(bOpened)
				result.append("</span>");
		}

		switch(character)
		{
			case IrcControlCodes::Bold:
			{
				bCurrentBold = !bCurrentBold;
				++uIndex;
				break;
			}
			case IrcControlCodes::Color:
			{
				++uIndex;
				unsigned char ucForeground;
				unsigned char ucBackground;
				uIndex = getUnicodeColorBytes(processed,uIndex,&ucForeground,&ucBackground);
				if(ucForeground != IrcControlCodes::NoChange)
					ucCurrentForeground = ucForeground;
				else
					ucCurrentForeground = IRC_DEFAULT_FOREGROUND; // only a CTRL+K
				if(ucBackground != IrcControlCodes::NoChange)
					ucCurrentBackground = ucBackground;
				else
					ucCurrentBackground = IRC_DEFAULT_BACKGROUND;
				break;
			}
			case IrcControlCodes::Italic:
				bCurrentItalic = !bCurrentItalic;
				++uIndex;
				break;
			case IrcControlCodes::StrikeThrough:
				bCurrentStrikeThrough = !bCurrentStrikeThrough;
				++uIndex;
				break;
			case IrcControlCodes::Reset:
			{
				ucCurrentForeground = IRC_DEFAULT_FOREGROUND;
				ucCurrentBackground = IRC_DEFAULT_BACKGROUND;
				bCurrentBold = false;
				bCurrentItalic = false;
				bCurrentStrikeThrough = false;
				bCurrentUnderline = false;
				++uIndex;
				break;
			}
			case IrcControlCodes::Underline:
			case IrcControlCodes::Underline2:
			{
				bCurrentUnderline = !bCurrentUnderline;
				++uIndex;
				break;
			}
			case IrcControlCodes::Reverse:
			{
				char cAuxBack = ucCurrentBackground;
				ucCurrentBackground = ucCurrentForeground;
				ucCurrentForeground = cAuxBack;
				++uIndex;
				break;
			}
			case ':':
			case ';':
			case '=':
			{
				qDebug() << "Possible emoticon detected.";
				if(bShowIcons)
				{
					//potential emoticon, got eyes
					++uIndex;
					QString szLookup;
					szLookup.append(QChar(character));
					unsigned short uIsEmoticon = 0;
					unsigned int uIcoStart = uIndex;

					if(uIndex < (unsigned int)processed.length())
					{
						//look up for a nose
						if(processed[(int)uIndex] == '-')
						{
							szLookup.append('-');
							uIndex++;
						}
					}

					if(uIndex < (unsigned int)processed.length())
					{
						//look up for a mouth
						unsigned short uMouth = processed[(int)uIndex].unicode();
						switch(uMouth)
						{
							case ')':
							case '(':
							case '/':
							case 'D':
							case 'P':
							case 'S':
							case 'O':
							case '*':
							case '|':
								szLookup += QChar(uMouth);
								uIsEmoticon++;
								uIndex++;
								break;
							default:
								break;
						}
					}

					if(uIndex < (unsigned int)processed.length())
					{
						//look up for a space
						if(processed[(int)uIndex]== ' ')
						{
							uIsEmoticon++;
						}
					} else {
						//got a smile at the end of the text
						uIsEmoticon++;
					}

					if(uIsEmoticon > 1)
					{
						qDebug() << "Emoticon is " << szLookup;
						bIgnoreIcons = true;
						/*
						KviTextIcon * pIcon  = g_pTextIconManager->lookupTextIcon(szLookup);
						// do we have that emoticon-icon association ?
						if(pIcon)
						{
							result.append("<img src=\"");
							result.append(g_pIconManager->getSmallIconResourceName(pIcon->id()));
							if(ucCurrentBackground != IRC_DEFAULT_BACKGROUND)
							{
								result.append("\" style=\"background-color:");
								result.append(KVI_OPTION_MIRCCOLOR(ucCurrentBackground).name());
							}
							result.append("\" />");
						} else {
							bIgnoreIcons = true;
							uIndex = uIcoStart-1;
						}
						*/
					} else {
						bIgnoreIcons = true;
						uIndex = uIcoStart-1;
					}

					break;
					qDebug() << "Exiting emoticon detected. ";
				} else {
					bIgnoreIcons = true;
				}
			}
		}
	}

	int pos = 0;
	while ((pos = URL_PATTERN.indexIn(result, pos)) >= 0)
	{
		int len = URL_PATTERN.matchedLength();
		QString href = result.mid(pos, len);

		// Don't consider trailing &gt; as part of the link.
		QString append;
		if (href.endsWith(QLatin1String("&gt;")))
		{
			append.append(href.right(4));
			href.chop(4);
		}

		// Don't consider trailing comma or semi-colon as part of the link.
		if (href.endsWith(QLatin1Char(',')) || href.endsWith(QLatin1Char(';')))
		{
			append.append(href.right(1));
			href.chop(1);
		}

		// Don't consider trailing closing parenthesis part of the link when
		// there's an opening parenthesis preceding in the beginning of the
		// URL or there is no opening parenthesis in the URL at all.
		if (pos > 0 && href.endsWith(QLatin1Char(')'))
			&& (result.at(pos-1) == QLatin1Char('(')
			|| !href.contains(QLatin1Char('('))))
		{
			append.prepend(href.right(1));
			href.chop(1);
		}

		// Qt doesn't support (?<=pattern) so we do it here
		if (pos > 0 && result.at(pos-1).isLetterOrNumber())
		{
			pos++;
			continue;
		}

		QString protocol;
		if (URL_PATTERN.cap(1).startsWith(QLatin1String("www."), Qt::CaseInsensitive))
			protocol = QLatin1String("http://");
		else if (URL_PATTERN.cap(1).isEmpty())
			protocol = QLatin1String("mailto:");

		QString source = href;
		source.replace(QLatin1String("&amp;"), QLatin1String("&"));

		QString link = QString(QLatin1String("<a href='%1%2'>%3</a>")).arg(protocol, source, href) + append;
		result.replace(pos, len, link);
		pos += link.length();
	}

	//qDebug("Results %s",result.toUtf8().data());
	return result;
}

/*!
	Scans the szData for a mIrc color code XX,XX
	and fills the color values in the two bytes
*/

unsigned int IrcUtil::getUnicodeColorBytes(const QString & szData, unsigned int iChar, unsigned char * pcByte1, unsigned char * pcByte2)
{
	if(iChar >= (unsigned int)szData.length())
	{
		(*pcByte1) = IrcControlCodes::NoChange;
		(*pcByte2) = IrcControlCodes::NoChange;
		return iChar;
	}

	unsigned short c = szData[(int)iChar].unicode();

	//First we can have a digit or a comma
	if(((c < '0') || (c > '9')))
	{
		// senseless : only a CTRL+K code
		(*pcByte1) = IrcControlCodes::NoChange;
		(*pcByte2) = IrcControlCodes::NoChange;
		return iChar;
	}

	//Something interesting ok.
	(*pcByte1) = c - '0'; //store the code
	iChar++;
	if(iChar >= (unsigned int)szData.length())
	{
		(*pcByte2) = IrcControlCodes::NoChange;
		return iChar;
	}

	c = szData[(int)iChar].unicode();

	if(((c < '0') || (c > '9')) && (c != ','))
	{
		(*pcByte2) = IrcControlCodes::NoChange;
		return iChar;
	}

	if((c >= '0') && (c <= '9'))
	{
		(*pcByte1) = (((*pcByte1)*10)+(c-'0'))%16;
		iChar++;
		if(iChar >= (unsigned int)szData.length())
		{
			(*pcByte2) = IrcControlCodes::NoChange;
			return iChar;
		}
		c = szData[(int)iChar].unicode();
	}

	if(c == ',')
	{
		iChar++;
		if(iChar >= (unsigned int)szData.length())
		{
			(*pcByte2) = IrcControlCodes::NoChange;
			return iChar;
		}
		c = szData[(int)iChar].unicode();
	} else {
		(*pcByte2) = IrcControlCodes::NoChange;
		return iChar;
	}

	if((c < '0') || (c > '9'))
	{
		(*pcByte2) = IrcControlCodes::NoChange;
		if(szData[(int)(iChar-1)].unicode()==',')
			return iChar-1;
		else
			return iChar;
	}

	//Background, a color code
	(*pcByte2) = c-'0';
	iChar++;
	if(iChar >= (unsigned int)szData.length())
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
    Converts a color \a code to a color name. If the color \a code
    is unknown, the function returns \a defaultColor.
*/
QString IrcUtil::colorCodeToName(int code, const QString& defaultColor)
{
    switch (code)
    {
	case IrcControlCodes::White:  return QLatin1String("white");
	case IrcControlCodes::Black:  return QLatin1String("black");
	case IrcControlCodes::DarkBlue:  return QLatin1String("darkblue");
	case IrcControlCodes::DarkGreen:  return QLatin1String("darkgreen");
	case IrcControlCodes::Red:  return QLatin1String("red");
	case IrcControlCodes::DarkRed:  return QLatin1String("darkred");
	case IrcControlCodes::DarkViolet:  return QLatin1String("darkviolet");
	case IrcControlCodes::Orange:  return QLatin1String("orange");
	case IrcControlCodes::Yellow:  return QLatin1String("yellow");
	case IrcControlCodes::LightGreen:  return QLatin1String("lightgreen");
	case IrcControlCodes::BlueMarine: return QLatin1String("bluemarine");
	case IrcControlCodes::LightBlue: return QLatin1String("lightblue");
	case IrcControlCodes::Blue: return QLatin1String("blue");
	case IrcControlCodes::LightViolet: return QLatin1String("lightviolet");
	case IrcControlCodes::DarkGray: return QLatin1String("darkgray");
	case IrcControlCodes::LightGray: return QLatin1String("lightgray");
    default: return defaultColor;
    }
}

