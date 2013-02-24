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

#include "completerlineedit.h"

CompleterLineEdit::CompleterLineEdit(const QStringList& completions, QWidget *parent) :
	QLineEdit(parent)
{
	m_pCompleter = new QCompleter(completions, this);
	setCaseSensitivity(Qt::CaseInsensitive);
	setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	setCompleter(m_pCompleter);
	m_bCompleteOnDoubleClick = true;
}

void CompleterLineEdit::mouseDoubleClickEvent(QMouseEvent *)
{
	m_pCompleter->complete();
}

void CompleterLineEdit::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
	m_pCompleter->setCaseSensitivity(caseSensitivity);
}

Qt::CaseSensitivity CompleterLineEdit::caseSensitivity() const
{
	return m_pCompleter->caseSensitivity();
}

void CompleterLineEdit::setCompletionMode(QCompleter::CompletionMode mode)
{
	m_pCompleter->setCompletionMode(mode);
}

QCompleter::CompletionMode CompleterLineEdit::completionMode() const
{
	return m_pCompleter->completionMode();
}

void CompleterLineEdit::setWrapAround(bool wrap)
{
	m_pCompleter->setWrapAround(wrap);
}

bool CompleterLineEdit::wrapAround() const
{
	return m_pCompleter->wrapAround();
}

bool CompleterLineEdit::completeOnDoubleClick() const
{
	return m_bCompleteOnDoubleClick;
}

void CompleterLineEdit::setCompleteOnDoubleClick(bool complete)
{
	m_bCompleteOnDoubleClick = complete;
}
