/*
** widgetchatmiddle.h
**
** Copyright © Quazaa Development Team, 2009-2011.
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

#ifndef WIDGETCHATCENTER_H
#define WIDGETCHATCENTER_H

#include <QMainWindow>
#include <QLineEdit>
#include <QToolButton>
#include <QTextDocument>

#include "maintabwidget.h"
#include "widgetchatinput.h"

struct Connection;

namespace Ui
{
	class WidgetIRCMain;
}

class WidgetIRCMain : public QMainWindow
{
	Q_OBJECT
public:
	WidgetIRCMain(QWidget* parent = 0);
	~WidgetIRCMain();
	WidgetChatInput *widgetChatInput;

protected:
	void changeEvent(QEvent* e);
	void closeEvent(QCloseEvent* event);

private:
	Ui::WidgetIRCMain* ui;
	MainTabWidget* tabWidgetMain;

public slots:
	void saveWidget();
	void connectTo(const QString& host = QString(), quint16 port = 6667,
				   const QString& nick = QString(), const QString& password = QString());
	void connectTo(const Connection& connection);
	void connectToImpl(const Connection& connection);

private slots:
	void on_actionChatSettings_triggered();
	void on_actionEditMyProfile_triggered();
	void initialize();
	void tabActivated(int index);
	void createWelcomeView();
	void createTabbedView();
	void onNewTabRequested();
};

#endif // WIDGETCHATCENTER_H
