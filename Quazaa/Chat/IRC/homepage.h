/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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

#ifndef WIDGETIRCHOMEPAGE_H
#define WIDGETIRCHOMEPAGE_H

#include <QWidget>
class QLabel;

class WidgetIrcHomePage : public QWidget
{
	Q_OBJECT

public:
	WidgetIrcHomePage(QWidget* parent = 0);

signals:
	void connectRequested();

protected:
	void paintEvent(QPaintEvent* event);

private:
	QWidget* createBody(QWidget* parent = 0) const;

	QLabel* header;
	QLabel* slogan;
	QLabel* footer;
	QPixmap bg;

private slots:
	void showSettings();
};

#endif // WIDGETIRCHOMEPAGE_H
