/*
** chatconverter.h
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

#ifndef CHATCONVERTER_H
#define CHATCONVERTER_H

class QTextDocument;
class QTextBlock;
class QTextFragment;

#include <QString>
#include <QStack>
#include <QColor>

class CChatConverter
{

	enum OutputFormats
	{
		HTML,
		BBCODE,
		IRCCODE
	};

	QString			m_pResult;
	QTextDocument*	m_pDocument;

	QStack<QString> m_pStack;

	OutputFormats	m_nOutputFormat;

public:
	CChatConverter(QTextDocument* pDoc);
	virtual ~CChatConverter();

	QString toHtml();
	QString toBBCode();
	QString toIRC();
private:
	void processBlock(QTextBlock* pBlock);
	void processFragment(QTextFragment* pFrag);
	void processFragmentBB(QTextFragment* pFrag);
	void processFragmentIRC(QTextFragment* pFrag);
};

#endif // CHATCONVERTER_H
