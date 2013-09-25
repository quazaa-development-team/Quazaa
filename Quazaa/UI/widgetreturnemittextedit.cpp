/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
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
#include <QDebug>
#include <QApplication>

#if QT_VERSION >= 0x050000
#include <QTextDocument> //For Qt::escape()
#endif

#include "debug_new.h"

WidgetReturnEmitTextEdit::WidgetReturnEmitTextEdit(QWidget *parent)
{
	Q_UNUSED(parent);
	resetHistoryIndex();
	m_oCompleter = new Completer(this);
	m_oCompleter->setWidget(this);
    m_oCompleter->setTextEdit(this);

	emitReturn = true;
	connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

	setAttribute(Qt::WA_MacShowFocusRect, false);

	connect(this, SIGNAL(tabPressed()), m_oCompleter, SLOT(onTabPressed()));
	setSkin();
}

bool WidgetReturnEmitTextEdit::event(QEvent* event)
{
    if (event->type() == QEvent::ShortcutOverride) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        if (ke == QKeySequence::MoveToStartOfDocument) {
            emit scrollToTop();
            event->accept();
            return true;
        } else if (ke == QKeySequence::MoveToEndOfDocument) {
            emit scrollToBottom();
            event->accept();
            return true;
        } else if (ke == QKeySequence::MoveToNextPage) {
            emit scrollToNextPage();
            event->accept();
            return true;
        } else if (ke == QKeySequence::MoveToPreviousPage) {
            emit scrollToPreviousPage();
            event->accept();
            return true;
        }
    }
    return QTextEdit::event(event);
}

void WidgetReturnEmitTextEdit::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Return)
	{
        if (event->modifiers() & Qt::ShiftModifier) {
            QTextEdit::keyPressEvent(event);
		} else {
            emit returnPressed();
		}
	} else if (event->key() == Qt::Key_Up) {
		if (!m_lHistory.isEmpty())
		{
			if(m_iHistoryIndex < 1)
				m_iHistoryIndex = (m_lHistory.size() - 1);
			else
				--m_iHistoryIndex;
			event->ignore();
			setHtml(m_lHistory.at(m_iHistoryIndex));
		} else {
			event->ignore();
			QTextEdit::keyPressEvent(event);
		}
	} else if (event->key() == Qt::Key_Down) {
		if (!m_lHistory.isEmpty())
		{
			if(m_iHistoryIndex == -1 || m_iHistoryIndex == (m_lHistory.size() - 1))
				m_iHistoryIndex = 0;
			else
				++m_iHistoryIndex;
			event->ignore();
			setHtml(m_lHistory.at(m_iHistoryIndex));
		} else {
			event->ignore();
			QTextEdit::keyPressEvent(event);
		}
	} else {
		resetHistoryIndex();
		QTextEdit::keyPressEvent(event);
	}
}

Completer* WidgetReturnEmitTextEdit::completer() const
{
	return m_oCompleter;
}

QString WidgetReturnEmitTextEdit::textUnderCursor() const
{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);

	// Save selected positions of current word.
	int selectionStart = tc.selectionStart();
	int selectionEnd = tc.selectionEnd();

	// If selection is at beginning of text edit then there can't be a slash to check for
	if (selectionStart == 0)
		return tc.selectedText();

	// Modify selection to include previous character
	tc.setPosition(selectionStart - 1, QTextCursor::MoveAnchor);
	tc.setPosition(selectionEnd, QTextCursor::KeepAnchor);

	// If previous character was / return current selection for command completion
	if(tc.selectedText().startsWith('/'))
		return tc.selectedText();
	else {
		// Else restore original selection and return for nick completion
		tc.setPosition(selectionStart, QTextCursor::MoveAnchor);
		tc.setPosition(selectionEnd, QTextCursor::KeepAnchor);
		return tc.selectedText();
    }
}

int WidgetReturnEmitTextEdit::currentWordStartIndex()
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);

    // Return start position of current word.
    return tc.selectionStart();
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
    if (!tabChangesFocus() && (textInteractionFlags() & Qt::TextEditable))
	{
		emit tabPressed();
		return true;
	} else {
		return QAbstractScrollArea::focusNextPrevChild(next);
    }
}

void WidgetReturnEmitTextEdit::insertFromMimeData(const QMimeData *source)
{
    insertPlainText( source->text() );
}

void WidgetReturnEmitTextEdit::addHistory(QTextDocument* document)
{
	if(m_lHistory.count() > 49 && !m_lHistory.isEmpty())
	{
		m_lHistory.removeAt(0);
		m_lPlainTextHistory.removeAt(0);
	}
	if(!m_lPlainTextHistory.contains(document->toPlainText()))
	{
		m_lHistory.append(document->toHtml());
		m_lPlainTextHistory.append(document->toPlainText());
	}
	resetHistoryIndex();
}

void WidgetReturnEmitTextEdit::addHistory(QString* text)
{
	QTextDocument* document = new QTextDocument();
#if QT_VERSION >= 0x050000
	document->setHtml(text->toHtmlEscaped());
#else
	QString escapeText = text->toLocal8Bit();
	document->setHtml(Qt::escape(escapeText));
#endif
	addHistory(document);
}

void WidgetReturnEmitTextEdit::resetHistoryIndex()
{
	m_iHistoryIndex = -1;
}
