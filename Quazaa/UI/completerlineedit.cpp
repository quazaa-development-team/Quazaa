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
