/*
** widgetsecurity.h
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

#ifndef WIDGETSECURITY_H
#define WIDGETSECURITY_H

#include <QtGui>
#include <QAbstractItemModel>

class CSecurityTableModel;

namespace Ui
{
	class WidgetSecurity;
}

class WidgetSecurity : public QMainWindow
{
	Q_OBJECT
public:
	CSecurityTableModel* m_pSecurityList;

public:
	WidgetSecurity(QWidget* parent = 0);
	~WidgetSecurity();

	void		setModel(QAbstractItemModel* model);
	QWidget*	tableView();
	void		saveWidget();

protected:
	void changeEvent(QEvent* e);
	void keyPressEvent(QKeyEvent *event);

private:
	Ui::WidgetSecurity* ui;
	QMenu* securityMenu;

signals:

public slots:
	void update();

private slots:
	void on_actionSecurityAddRule_triggered();
	void on_actionSecurityRemoveRule_triggered();
	void on_actionSecurityModifyRule_triggered();
	void on_actionSecurityImportRules_triggered();
	void on_actionSecurityExportRules_triggered();
	void on_actionSubscribeSecurityList_triggered();
	void on_tableViewSecurity_customContextMenuRequested(const QPoint &pos);
	void on_tableViewSecurity_doubleClicked(const QModelIndex &index);
	void on_tableViewSecurity_clicked(const QModelIndex &index);
	void setSkin();
};

#endif // WIDGETSECURITY_H
