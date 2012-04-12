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

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include "ui_messageview.h"
#include <ircsession.h>
#include <ircmessage.h>
#include <irccommand.h>
#include "commandparser.h"
#include "messageformatter.h"

#include <QToolButton>

class QStringListModel;

class MessageView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString receiver READ receiver WRITE setReceiver)

public:
    MessageView(IrcSession* session, QWidget* parent = 0);
    ~MessageView();

	QToolButton* closeButton;

    QString receiver() const;
	void setReceiver(const QString& receiver);
	void setStatusChannel(bool statusChannel);

    bool isChannelView() const;
	bool isStatusChannel() const;

public slots:
    void showHelp(const QString& text, bool error = false);
    void appendMessage(const QString& message);
	void onSend(const QString& text);

signals:
    void highlight(MessageView* view, bool on);
    void alert(MessageView* view, bool on);
	void query(const QString& user);
	void appendQueryMessage(const QString& receiver, const QString& message);
	void appendRawMessage(const QString& message);
	void closeQuery(MessageView *view);
    void aboutToQuit();
	void partView(MessageView *view);
	void quitSession();
	void join(QString channel);

protected:
    bool eventFilter(QObject* receiver, QEvent* event);

protected slots:
    void receiveMessage(IrcMessage* message);
    void addUser(const QString& user);
	void removeUser(const QString& user);
	void part();

private slots:
	void onEscPressed();
	void applySettings();
    void onCustomCommand(const QString& command, const QStringList& params);
	void followLink(const QString& link);

private:static
	QString prettyNames(const QStringList& names, int columns);

	struct MessageViewData : public Ui::MessageView
    {
        QString receiver;
        IrcSession* session;
        CommandParser parser;
        MessageFormatter formatter;
        QStringListModel* userModel;
		static QStringListModel* commandModel;
		bool m_bIsStatusChannel;
		QSet<IrcCommand::Type> sentCommands;
    } d;
};

#endif // MESSAGEVIEW_H
