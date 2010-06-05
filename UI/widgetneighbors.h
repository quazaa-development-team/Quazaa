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
