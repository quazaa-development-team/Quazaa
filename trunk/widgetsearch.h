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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearch *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETSEARCH_H
