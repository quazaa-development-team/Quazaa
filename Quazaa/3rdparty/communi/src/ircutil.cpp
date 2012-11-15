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

/*
  Parts of this code come from Konversation and are copyrighted to:
  Copyright (C) 2002 Dario Abatianni <eisfuchs@tigress.com>
  Copyright (C) 2004 Peter Simonsson <psn@linux.se>
  Copyright (C) 2006-2008 Eike Hein <hein@kde.org>
  Copyright (C) 2004-2009 Eli Mackenzie <argonel@gmail.com>
*/

#include "ircutil.h"
#include "ircformatting.h"
#include <QStringList>
#include <QRegExp>
#include <QHash>
#include <QDebug>

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
	them with appropriate HTML hyperlinks and detects smileys
	and replaces them with the appropriate images.
*/
QString IrcUtil::messageToHtml(const QString& message, QHash<QString, QString> emoticons)
{
	QString result;
	bool bCurrentBold      = false;
	bool bCurrentItalic    = false;
	bool bCurrentStrikeThrough = false;
	bool bCurrentUnderline = false;
	bool bIgnoreSmileys  = false;

	uchar ucCurrentForeground = IRC_DEFAULT_FOREGROUND; //default foreground
	uchar ucCurrentBackground = IRC_DEFAULT_BACKGROUND; //default background

	uint uIndex = 0;

	QString processed = message.toHtmlEscaped();

	//Begin url detection
	int pos = 0;
	while ((pos = URL_PATTERN.indexIn(processed, pos)) >= 0)
	{
		int len = URL_PATTERN.matchedLength();

		QString href = IrcFormatting::stripFormatting(processed.mid(pos, len));

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
			&& (processed.at(pos-1) == QLatin1Char('(')
			|| !href.contains(QLatin1Char('('))))
		{
			append.prepend(href.right(1));
			href.chop(1);
		}

		// Qt 4 doesn't support (?<=pattern) so we do it here
		if (pos > 0 && processed.at(pos-1).isLetterOrNumber())
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

		processed.replace(pos, len, link);
		pos += link.length();
	}

	while(uIndex < (uint)processed.length())
	{
		ushort character = processed[(int)uIndex].unicode();
		uint uStart = uIndex;

		while(
			(character != IrcFormatting::Bold) &&
			(character != IrcFormatting::Color) &&
			(character != IrcFormatting::Italic) &&
			(character != IrcFormatting::StrikeThrough) &&
			(character != IrcFormatting::Reset) &&
			(character != IrcFormatting::Underline) &&
			(character != IrcFormatting::Underline2) &&
			(character != IrcFormatting::Reverse) &&
			((character != ':') || bIgnoreSmileys) &&
			((character != ';') || bIgnoreSmileys))
		{
			bIgnoreSmileys = false;
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
			if(uIndex >= (uint)processed.length())
				break;

			character = processed[(int)uIndex].unicode();
		}

		bIgnoreSmileys = false;
		int iLength = uIndex - uStart;

		if(iLength > 0)
		{
			bool bOpened = false;

			if(ucCurrentForeground != IRC_DEFAULT_FOREGROUND && !result.endsWith("http"))
			{
				result.append("<span style=\"color:");
				result.append(IrcFormatting::colorName(ucCurrentForeground));
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
				result.append(IrcFormatting::colorName(ucCurrentBackground, QLatin1String("transparent")));
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

			if(bOpened && !result.endsWith("http"))
				result.append("</span>");
		}

		switch(character)
		{
			case IrcFormatting::Bold:
			{
				bCurrentBold = !bCurrentBold;
				++uIndex;
				break;
			}
			case IrcFormatting::Color:
			{
				++uIndex;
				uchar ucForeground;
				uchar ucBackground;
				uIndex = IrcFormatting::getUnicodeColorBytes(processed,uIndex,&ucForeground,&ucBackground);
				if(ucForeground != IrcFormatting::NoChange)
					ucCurrentForeground = ucForeground;
				else
					ucCurrentForeground = IRC_DEFAULT_FOREGROUND; // only a CTRL+K
				if(ucBackground != IrcFormatting::NoChange)
					ucCurrentBackground = ucBackground;
				else
					ucCurrentBackground = IRC_DEFAULT_BACKGROUND;
				break;
			}
			case IrcFormatting::Italic:
				bCurrentItalic = !bCurrentItalic;
				++uIndex;
				break;
			case IrcFormatting::StrikeThrough:
				bCurrentStrikeThrough = !bCurrentStrikeThrough;
				++uIndex;
				break;
			case IrcFormatting::Reset:
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
			case IrcFormatting::Underline:
			case IrcFormatting::Underline2:
			{
				bCurrentUnderline = !bCurrentUnderline;
				++uIndex;
				break;
			}
			case IrcFormatting::Reverse:
			{
				char cAuxBack = ucCurrentBackground;
				ucCurrentBackground = ucCurrentForeground;
				ucCurrentForeground = cAuxBack;
				++uIndex;
				break;
			}
			case ':':
			case ';':
			{
				if(!emoticons.isEmpty())
				{
					//potential emoticon, got eyes
					++uIndex;
					QString szLookup;
					szLookup.append(QChar(character));
					ushort uIsEmoticon = 0;
					uint uIcoStart = uIndex;

					if(uIndex < (uint)processed.length())
					{
						//look up for a nose
						if(processed[(int)uIndex] == '-')
						{
							szLookup.append('-');
							uIndex++;
						}
					}

					if(uIndex < (uint)processed.length())
					{
						//look up for a mouth
						ushort uMouth = processed[(int)uIndex].toUpper().unicode();
						switch(uMouth)
						{
							case ')':
							case '(':
							case '/':
							case '\\':
							case 'D':
							case 'P':
							case 'S':
							case 'O':
							case 'X':
							case '*':
							case '|':
							case '@':
								szLookup += QChar(uMouth);
								uIsEmoticon++;
								uIndex++;
								break;
							default:
								break;
						}
					}

					if(uIndex < (uint)processed.length())
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
						// do we have that emoticon-icon association ?
						if(emoticons.contains(szLookup))
						{
							result.append("<img src=\"");
							result.append(emoticons.value(szLookup));
							result.append("\" />");
						} else {
							bIgnoreSmileys = true;
							uIndex = uIcoStart-1;
						}
					} else {
						bIgnoreSmileys = true;
						uIndex = uIcoStart-1;
					}
					break;
				} else {
					bIgnoreSmileys = true;
				}
			}
		}
	}

	return result;
}

/*!
	\deprecated Use IrcFormatting::colorName() instead.
*/
QString IrcUtil::colorCodeToName(int code, const QString& defaultColor)
{
	return IrcFormatting::colorName(code, defaultColor);
}
