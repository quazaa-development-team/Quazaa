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
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearch *ui;

public slots:
	void startNewSearch(QString *searchString);

private slots:
	void on_splitterSearch_customContextMenuRequested(QPoint pos);
 void on_toolButtonNewSearch_clicked();
	void on_toolButtonSearchClear_clicked();
	void on_toolButtonSearch_clicked();
	void skinChangeEvent();
};

#endif // WIDGETSEARCH_H
