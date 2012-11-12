#ifndef COMPLETERLINEEDIT_H
#define COMPLETERLINEEDIT_H

#include <QLineEdit>
#include <QCompleter>
#include <QStringList>

class CompleterLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit CompleterLineEdit(const QStringList& completions, QWidget *parent = 0);
	void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);
	Qt::CaseSensitivity caseSensitivity() const;
	void setCompletionMode(QCompleter::CompletionMode mode);
	QCompleter::CompletionMode completionMode() const;
	bool wrapAround() const;
	bool completeOnDoubleClick() const;

private:
	QCompleter* m_pCompleter;
	bool m_bCompleteOnDoubleClick;

protected:
	void mouseDoubleClickEvent(QMouseEvent *);

public Q_SLOTS:
	void setWrapAround(bool wrap);
	void setCompleteOnDoubleClick(bool complete);
};

#endif // COMPLETERLINEEDIT_H
