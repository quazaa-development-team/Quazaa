/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "completer.h"
#include "historylineedit.h"

#include <QDebug>
#include <QtWidgets/QAbstractItemView>

Completer::Completer(QObject* parent) : QCompleter(parent)
{
	d.textEdit = 0;
	d.tabModel = 0;
	d.slashModel = 0;
	setCaseSensitivity(Qt::CaseInsensitive);
	setCompletionMode(InlineCompletion);
	connect(this, SIGNAL(highlighted(QString)), this, SLOT(insertCompletion(QString)));
}

WidgetReturnEmitTextEdit* Completer::textEdit() const
{
	return d.textEdit;
}

void Completer::setTextEdit(WidgetReturnEmitTextEdit* textEdit)
{
	if (d.textEdit != textEdit)
	{
		if (d.textEdit)
			disconnect(d.textEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextEdited()));

		if (textEdit)
			connect(textEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextEdited()));

		d.textEdit = textEdit;
	}
}

IrcUserListModel* Completer::tabModel() const
{
	return d.tabModel;
}

void Completer::setTabModel(IrcUserListModel* model)
{
	d.tabModel = model;
}

QStringListModel *Completer::slashModel() const
{
	return d.slashModel;
}

void Completer::setSlashModel(QStringListModel* model)
{
	d.slashModel = model;
}

void Completer::onTabPressed()
{
	if (!d.textEdit)
		return;

	QString word = d.textEdit->textUnderCursor();

	// choose model
	if (word.startsWith('/'))
	{
		if (model() != d.slashModel)
			setModel(d.slashModel);
	}
	else
	{
		if (model() != d.tabModel->toStringListModel())
			setModel(d.tabModel->toStringListModel());
	}

	if (word != completionPrefix()) {
		setCompletionPrefix(word);
	}

	QString prefix = completionPrefix();
	if (prefix.isEmpty() || !word.startsWith(prefix, Qt::CaseInsensitive))
		setCompletionPrefix(word);

	// complete
	if (!word.isEmpty())
	{
		complete();

		int count = completionCount();
		if (count > 0)
		{
			int next = currentRow() + 1;
			setCurrentRow(next % count);
		}
	}
}

void Completer::onTextEdited()
{
	setCompletionPrefix(QString());
}

void Completer::insertCompletion(const QString& completion)
{
	if (!d.textEdit)
		return;
	QTextCursor tc = d.textEdit->textCursor();
	int extra = completion.length() - completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	d.textEdit->setTextCursor(tc);
}
