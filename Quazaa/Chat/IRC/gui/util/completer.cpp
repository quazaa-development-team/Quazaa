/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
* Copyright (C) 2010-2013 SmokeX <smokexjc@gmail.com>
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
#include "ircuserlistmodel.h"

Completer::Completer(QObject* parent) : QCompleter(parent)
{
	d.lineEdit = 0;
    d.textEdit = 0;
	d.defaultModel = 0;
	d.slashModel = 0;
	setCaseSensitivity(Qt::CaseInsensitive);
	setCompletionMode(InlineCompletion);
	connect(this, SIGNAL(highlighted(QString)), this, SLOT(insertCompletion(QString)));
}

HistoryLineEdit* Completer::lineEdit() const
{
	return d.lineEdit;
}

void Completer::setLineEdit(HistoryLineEdit* lineEdit)
{
	if (d.lineEdit != lineEdit) {
		if (d.lineEdit)
			disconnect(d.lineEdit, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited()));

		if (lineEdit)
			connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited()));

		d.lineEdit = lineEdit;
    }

    d.textEdit = 0;
}

WidgetReturnEmitTextEdit *Completer::textEdit() const
{
    return d.textEdit;
}

void Completer::setTextEdit(WidgetReturnEmitTextEdit *textEdit)
{
    if (d.textEdit != textEdit)
    {
        if (d.textEdit)
            disconnect(d.textEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextEdited()));

        if (textEdit)
            connect(textEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextEdited()));

        d.textEdit = textEdit;
    }

    d.lineEdit = 0;
}

IrcUserListModel* Completer::defaultModel() const
{
	return d.defaultModel;
}

void Completer::setDefaultModel(IrcUserListModel* model)
{
	d.defaultModel = model;
}

QAbstractItemModel* Completer::slashModel() const
{
	return d.slashModel;
}

void Completer::setSlashModel(QAbstractItemModel* model)
{
	d.slashModel = model;
}

void Completer::onTabPressed()
{
    if (!d.lineEdit && !d.textEdit)
		return;

    if(d.lineEdit)
    {
        // store selection
        int pos = d.lineEdit->cursorPosition();
        int start = d.lineEdit->selectionStart();
        QString selected = d.lineEdit->selectedText();

        // select current word
        d.lineEdit->cursorWordForward(false);
        d.lineEdit->cursorWordBackward(true);
        QString word = d.lineEdit->selectedText();

        // choose model
        if (word.startsWith('/')) {
            if (model() != d.slashModel)
                setModel(d.slashModel);
        } else {
            if (model() != d.defaultModel->toStringListModel())
                setModel(d.defaultModel->toStringListModel());
        }

        QString prefix = completionPrefix();
        if (prefix.isEmpty() || !word.startsWith(prefix, Qt::CaseInsensitive))
            setCompletionPrefix(word);

        // restore selection
        d.lineEdit->setCursorPosition(pos);
        if (start != -1)
            d.lineEdit->setSelection(start, selected.length());

        // complete
        if (!word.isEmpty()) {
            complete();

            int count = completionCount();
            if (count > 0) {
                int next = currentRow() + 1;
                setCurrentRow(next % count);
            }
        }
    } else if(d.textEdit) {
        QString word = d.textEdit->textUnderCursor();
        int startIndex = d.textEdit->currentWordStartIndex();
        bool bIsCommand = false;

        // choose model
        if (word.startsWith('/'))
        {
            if (model() != d.slashModel)
                setModel(d.slashModel);
            bIsCommand = true;
        }
        else
        {
            if (model() != d.defaultModel->toStringListModel())
                setModel(d.defaultModel->toStringListModel());
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

                if ( !bIsCommand && (startIndex == 0) )
                    d.textEdit->insertPlainText(": ");
            }
        }
    }
}

void Completer::onTextEdited()
{
	setCompletionPrefix(QString());
}

void Completer::insertCompletion(const QString& completion)
{
    if (!d.lineEdit && !d.textEdit)
		return;

    if (d.lineEdit)
    {
        int pos = d.lineEdit->cursorPosition();
        QString text = d.lineEdit->text();
        if (pos > 0 && pos < text.length() && !text.at(pos - 1).isSpace())
            d.lineEdit->cursorWordForward(false);
        d.lineEdit->cursorWordBackward(true);
        pos = d.lineEdit->cursorPosition();

        QString tmp = completion;
        if (pos == 0 && !completion.startsWith("/"))
            tmp.append(":");
        d.lineEdit->insert(tmp);

        text = d.lineEdit->text();
        int cursor = d.lineEdit->cursorPosition();
        if (!text.at(cursor - 1).isSpace())
            d.lineEdit->insert(" ");
    } else if(d.textEdit) {
        QTextCursor tc = d.textEdit->textCursor();
        int extra = completion.length() - completionPrefix().length();
        tc.movePosition(QTextCursor::Left);
        tc.movePosition(QTextCursor::EndOfWord);
        tc.insertText(completion.right(extra));
        d.textEdit->setTextCursor(tc);
    }
}
