#ifndef WIDGETSEARCH_H
#define WIDGETSEARCH_H

#include <QWidget>
#include "widgetsearchresults.h"

namespace Ui {
    class WidgetSearch;
}

class WidgetSearch : public QWidget {
    Q_OBJECT
public:
    WidgetSearch(QWidget *parent = 0);
    ~WidgetSearch();
	WidgetSearchResults *panelSearchResults;
	void saveState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearch *ui;

private slots:
	void on_toolButtonNewSearch_triggered(QAction* );
	void skinChangeEvent();
};

#endif // WIDGETSEARCH_H
