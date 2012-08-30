/*
** $Id: widgetreturnemittextedit.cpp 931 2012-02-18 00:32:03Z smokexyz $
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

#include "widgetreturnemittextedit.h"
#include "skinsettings.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetReturnEmitTextEdit::WidgetReturnEmitTextEdit(QWidget *parent)
{
	Q_UNUSED(parent);
	emitReturn = true;
	setSkin();
}

void WidgetReturnEmitTextEdit::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Return)
	{
		if (emitReturn)
		{
			emit returnPressed();
		} else {
			QTextEdit::keyPressEvent(event);
		}
	} else {
		QTextEdit::keyPressEvent(event);
	}
}

void WidgetReturnEmitTextEdit::setEmitsReturn(bool shouldEmit)
{
	emitReturn = shouldEmit;
}

bool WidgetReturnEmitTextEdit::emitsReturn()
{
	return emitReturn;
}

void WidgetReturnEmitTextEdit::setSkin()
{

}
