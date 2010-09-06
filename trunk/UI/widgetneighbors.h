//
// widgetneighbors.h
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

namespace Ui {
	class WidgetNeighbors;
}

class WidgetNeighbors : public QMainWindow {
	Q_OBJECT
public:
	WidgetNeighbors(QWidget *parent = 0);
	~WidgetNeighbors();
	void setModel(QAbstractItemModel *model);
	QWidget *treeView();
	void saveWidget();
	QLabel *labelG2StatsIcon;
	QLabel *labelG2Stats;

protected:
	void changeEvent(QEvent *e);

private:
	Ui::WidgetNeighbors *ui;

private slots:
	void on_actionNeighborDisconnect_triggered();
 void on_actionNeighborConnectTo_triggered();
	void on_actionSettings_triggered();
	void skinChangeEvent();
	void updateG2();
	void updateAres();
	void updateEDonkey();
};

#endif // WIDGETNEIGHBORS_H
