/*
** $Id$
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

#include <QShortcut>
#include <QPlastiqueStyle>

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetReturnEmitTextEdit::WidgetReturnEmitTextEdit(QWidget *parent)
{
	Q_UNUSED(parent);
	m_oCompleter = new Completer(this);
	m_oCompleter->setWidget(this);
	m_oCompleter->setTextEdit(this);

	emitReturn = true;
	connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

	setAttribute(Qt::WA_MacShowFocusRect, false);

	// a workaround for a bug in the Oxygen style
	if (style()->objectName() == "oxygen")
		setStyle(new QPlastiqueStyle);

	connect(this, SIGNAL(tabPressed()), m_oCompleter, SLOT(onTabPressed()));
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

Completer* WidgetReturnEmitTextEdit::completer() const
{
	return m_oCompleter;
}

void WidgetReturnEmitTextEdit::onTextChanged()
{
	emit textChanged(toPlainText());
}

void WidgetReturnEmitTextEdit::setSkin()
{

}

bool WidgetReturnEmitTextEdit::focusNextPrevChild(bool next)
{
	if (!tabChangesFocus() && this->textInteractionFlags() & Qt::TextEditable)
	{
		emit tabPressed();
		return true;
	} else {
		return QAbstractScrollArea::focusNextPrevChild(next);
	}
}
