//
// widgetneighbours.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef WIDGETNEIGHBORS_H
#define WIDGETNEIGHBORS_H

#include <QtGui>
#include <QAbstractItemModel>

class CNeighboursTableModel;

namespace Ui
{
	class WidgetNeighbours;
}

class WidgetNeighbours : public QMainWindow
{
	Q_OBJECT
public:
	WidgetNeighbours(QWidget* parent = 0);
	~WidgetNeighbours();
	void setModel(QAbstractItemModel* model);
	QWidget* treeView();
	void saveWidget();
	QLabel* labelG2StatsIcon;
	QLabel* labelG2Stats;
	CNeighboursTableModel* neighboursList;

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetNeighbours* ui;
		QMenu *neighboursMenu;

private slots:
	void on_tableViewNeighbours_customContextMenuRequested(QPoint pos);
 void on_actionNeighbourDisconnect_triggered();
	void on_actionNeighbourConnectTo_triggered();
	void on_actionSettings_triggered();

	void onTimer();

	void updateG2();
	void updateAres();
	void updateEDonkey();
};

#endif // WIDGETNEIGHBORS_H
