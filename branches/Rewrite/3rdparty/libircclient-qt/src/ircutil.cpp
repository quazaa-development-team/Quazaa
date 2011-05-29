/*
* Copyright (C) 2008-2009 J-P Nurmi jpnurmi@gmail.com
* Copyright (C) 2010-2011 Joseph Crowell joseph.w.crowell@gmail.com
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at your
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
#include <QString>
#include <QRegExp>

/*!
    \class Irc::Util ircutil.h
    \brief The Irc::Util class provides IRC related utility functions.
 */

static QRegExp URL_PATTERN(QLatin1String("((www\\.(?!\\.)|(ssh|fish|irc|amarok|(f|sf|ht)tp(|s))://)(\\.?[\\d\\w/,\\':~\\^\\?=;#@\\-\\+\\%\\*\\{\\}\\!\\(\\)\\[\\]]|&)+)|""([-.\\d\\w]+@[-.\\d\\w]{2,}\\.[\\w]{2,})"), Qt::CaseInsensitive);

namespace Irc
{
	/*!
	    Parses and returns the nick part from \a target.
	*/
	QString Util::nickFromTarget(const QString& target)
	{
		int index = target.indexOf(QLatin1Char('!'));
		return target.left(index);
	}

	/*!
	    Parses and returns the host part from \a target.
	*/
	QString Util::hostFromTarget(const QString& target)
	{
		int index = target.indexOf(QLatin1Char('!'));
		return target.mid(index + 1);
	}

	/*!
	   Replaces IRC decoration characters with html code
	*/
	void Util::replaceDecoration(QString& line, QChar decoration, QChar replacement)
	{
		int pos;
		bool decorated = false;

		while((pos = line.indexOf(decoration)) != -1)
		{
			line.replace(pos, 1, (decorated) ? QString("</%1>").arg(replacement) : QString("<%1>").arg(replacement));
			decorated = !decorated;
		}
	}

	/*!
	    Converts \a message to HTML. This function parses the
	    message and replaces IRC-style formatting like colors,
	    bold and underline to the corresponding HTML formatting.
	    Furthermore, this function detects URLs and replaces
	    them with appropriate HTML hyperlinks.
	*/

	QString Util::messageToHtml(const QString& line, const QString& defaultColor, bool parseURL, bool allowColors)
	{
		QString filteredLine(line);

		//Since we can't turn off whitespace simplification withouteliminating text wrapping,
		//if the line starts with a space turn it into a non-breaking space.
		//(which magically turns back into a space on copy)

		if(filteredLine[0] == ' ')
		{
			filteredLine[0] = '\xA0';
		}

		// TODO: Use QStyleSheet::escape() here
		// Replace all < with &lt;
		filteredLine.replace('<', "\x0blt;");
		// Replace all > with &gt;
		filteredLine.replace('>', "\x0bgt;");

		// replace \003 and \017 codes with rich text color codes
		// captures          1    2                   23 4                   4 3     1
		QRegExp colorRegExp("(\003([0-9]|0[0-9]|1[0-5]|)(,([0-9]|0[0-9]|1[0-5])|,|)|\017)");

		int pos;
		bool firstColor = true;
		QString colorString;

		while((pos = colorRegExp.indexIn(filteredLine)) != -1)
		{
			if(!allowColors)
			{
				colorString.clear();
			}
			else
			{
				colorString = (firstColor) ? QString() : QString("</font>");

				// reset colors on \017 to default value
				if(colorRegExp.cap(1) == "\017")
				{
					colorString += "<font color=\"" + defaultColor + "\">";
				}
				else
				{
					if(!colorRegExp.cap(2).isEmpty())
					{
						int foregroundColor = colorRegExp.cap(2).toInt();
						colorString += "<font color=\"" + rgbFromCode(foregroundColor) + "\">";
					}
					else
					{
						colorString += "<font color=\"" + defaultColor + "\">";
					}
				}

				firstColor = false;
			}
			filteredLine.replace(pos, colorRegExp.cap(0).length(), colorString);
		}

		if(!firstColor)
		{
			filteredLine += "</font>";
		}

		// Replace all text decorations
		// TODO: \017 should reset all text decorations to plain text
		replaceDecoration(filteredLine, '\x02', 'b');
		replaceDecoration(filteredLine, '\x1d', 'i');
		replaceDecoration(filteredLine, '\x09', 'i');
		replaceDecoration(filteredLine, '\x13', 's');
		replaceDecoration(filteredLine, '\x15', 'u');
		replaceDecoration(filteredLine, '\x16', 'b'); // should be inverse
		replaceDecoration(filteredLine, '\x1f', 'u');

		if(parseURL)
		{
			filteredLine = extractUrlData(filteredLine, true);
		}
		else
		{
			// Change & to &amp; to prevent html entities to do strange things to the text
			filteredLine.replace('&', "&amp;");
			filteredLine.replace("\x0b", "&");
		}

		//filteredLine = Konversation::Emoticons::parseEmoticons(filteredLine);

		// Replace pairs of spaces with "<space>&nbsp;" to preserve some semblance of text wrapping
		filteredLine.replace("  ", " \xA0");
		return filteredLine;
	}

	/*!
	    Extracts URL data and converts to hyperlink if doHyperlinks is true.
	*/

	QString Util::extractUrlData(const QString text, bool doHyperlinks)
	{
		// QTime timer;
		// timer.start();

		int pos = 0;
		int urlLen = 0;

		QString link;
		QString extractedText = text;
		QString insertText;
		QString protocol;
		QString href;
		QString append;

		URL_PATTERN.setCaseSensitivity(Qt::CaseInsensitive);

		if(doHyperlinks)
		{
			link = "<a href=\"#%1\">%2</a>";

			if(extractedText.contains("#"))
			{
				QRegExp chanExp("(^|\\s|^\"|\\s\"|,|'|\\(|\\:|!|@|%|\\+)(#[^,\\s;\\)\\:\\/\\(\\<\\>]*[^.,\\s;\\)\\:\\/\\(\"\''\\<\\>])");

				while((pos = chanExp.indexIn(extractedText, pos)) >= 0)
				{
					href = chanExp.cap(2);
					urlLen = href.length();
					pos += chanExp.cap(1).length();

					insertText = link.arg(href, href);
					extractedText.replace(pos, urlLen, insertText);
					pos += insertText.length();
				}
			}

			link = "<a href=\"%1%2\">%3</a>";

			pos = 0;
			urlLen = 0;
		}

		while((pos = URL_PATTERN.indexIn(extractedText, pos)) >= 0)
		{
			urlLen = URL_PATTERN.matchedLength();

			// check if the matched text is already replaced as a channel
			if(doHyperlinks && extractedText.lastIndexOf("<a", pos) > extractedText.lastIndexOf("</a>", pos))
			{
				++pos;
				continue;
			}

			protocol.clear();
			href = extractedText.mid(pos, urlLen);
			append.clear();

			// Don't consider trailing comma part of link.
			if(href.right(1) == ",")
			{
				href.truncate(href.length() - 1);
				append = ',';
			}

			// Don't consider trailing semicolon part of link.
			if(href.right(1) == ";")
			{
				href.truncate(href.length() - 1);
				append = ';';
			}

			// Don't consider trailing closing parenthesis part of link when
			// there's an opening parenthesis preceding the beginning of the
			// URL or there is no opening parenthesis in the URL at all.
			if(href.right(1) == ")" && (extractedText.mid(pos - 1, 1) == "(" || !href.contains("(")))
			{
				href.truncate(href.length() - 1);
				append.prepend(")");
			}

			if(doHyperlinks)
			{
				// Qt doesn't support (?<=pattern) so we do it here
				if((pos > 0) && extractedText[pos-1].isLetterOrNumber())
				{
					pos++;
					continue;
				}

				if(URL_PATTERN.cap(1).startsWith(QLatin1String("www."), Qt::CaseInsensitive))
				{
					protocol = "http://";
				}
				else if(URL_PATTERN.cap(1).isEmpty())
				{
					protocol = "mailto:";
				}

				// Use \x0b as a placeholder for & so we can read them after changing all & in the normal text to &amp;
				insertText = link.arg(protocol, QString(href).replace('&', "\x0b"), href) + append;

				extractedText.replace(pos, urlLen, insertText);
			}
			else
			{
				insertText = href + append;
			}

			pos += insertText.length();
		}

		if(doHyperlinks)
		{
			// Change & to &amp; to prevent html entities to do strange things to the text
			extractedText.replace('&', "&amp;");
			extractedText.replace("\x0b", "&");
		}

		// kDebug() << "Took (msecs) : " << timer.elapsed() << " for " << extractedText;

		return extractedText;
	}

	/*!
	    Converts \a code to #rrggbb html color code.
	*/
	QString Util::rgbFromCode(int code)
	{
		switch(code)
		{
			case 0:
				return "#ffffff"; //white
			case 1:
				return "#000000"; //black
			case 2:
				return "#000080"; //navy
			case 3:
				return "#008000"; //green
			case 4:
				return "#ff0000"; //red
			case 5:
				return "#a52a2a"; //brown
			case 6:
				return "#800080"; //purple
			case 7:
				return "#808000"; //olive
			case 8:
				return "#ffff00"; //yellow
			case 9:
				return "#00ff00"; //lime
			case 10:
				return "#008b8b"; //dark cyan
			case 11:
				return "#00ffff"; //cyan
			case 12:
				return "#0000ff"; //blue
			case 13:
				return "#ff00ff"; //magenta
			case 14:
				return "#808080"; //gray
			case 15:
				return "#d3d3d3"; //light gray
			default:
				return "#000000"; //black
		}
	}

	/*!
	    Converts \a code to a color name.
	*/
	QString Util::colorNameFromCode(int code)
	{
		switch(code)
		{
			case 0:
				return QLatin1String("white");
			case 1:
				return QLatin1String("black");
			case 2:
				return QLatin1String("navy");
			case 3:
				return QLatin1String("green");
			case 4:
				return QLatin1String("red");
			case 5:
				return QLatin1String("maroon");
			case 6:
				return QLatin1String("purple");
			case 7:
				return QLatin1String("orange");
			case 8:
				return QLatin1String("yellow");
			case 9:
				return QLatin1String("lime");
			case 10:
				return QLatin1String("darkcyan");
			case 11:
				return QLatin1String("cyan");
			case 12:
				return QLatin1String("blue");
			case 13:
				return QLatin1String("magenta");
			case 14:
				return QLatin1String("gray");
			case 15:
				return QLatin1String("lightgray");
			default:
				return QLatin1String("black");
		}
	}
}
