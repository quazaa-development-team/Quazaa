/*
** widgetdiscovery.h
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

#ifndef WIDGETDISCOVERY_H
#define WIDGETDISCOVERY_H

#include <QtGui>

#include "tableview.h"

class CDiscoveryTableModel;

namespace Ui
{
	class WidgetDiscovery;
}

class WidgetDiscovery : public QMainWindow
{
	Q_OBJECT

public:
	CDiscoveryTableModel* m_pDiscoveryList;

private:
	Ui::WidgetDiscovery* ui;
	QMenu* m_pDiscoveryMenu;
	CTableView* tableViewDiscovery;

public:
	WidgetDiscovery(QWidget* parent = 0);
	~WidgetDiscovery();

	void		setModel(QAbstractItemModel* model);
	QWidget*	tableView();
	void		saveWidget();

protected:
	virtual void changeEvent(QEvent* e);
	virtual void keyPressEvent(QKeyEvent *event);

public slots:
	void update();

private slots:
	void on_tableViewDiscovery_customContextMenuRequested(const QPoint &pos);
	void on_tableViewDiscovery_doubleClicked(const QModelIndex &index);
	void on_tableViewDiscovery_clicked(const QModelIndex &index);

	void setSkin();
};

#endif // WIDGETDISCOVERY_H
