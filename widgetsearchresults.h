#ifndef WIDGETSEARCHRESULTS_H
#define WIDGETSEARCHRESULTS_H

#include <QMainWindow>

namespace Ui {
    class WidgetSearchResults;
}

class WidgetSearchResults : public QMainWindow {
    Q_OBJECT
public:
    WidgetSearchResults(QWidget *parent = 0);
    ~WidgetSearchResults();
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearchResults *ui;

public slots:
	void skinChangeEvent();
	void startNewSearch(QString searchString);
};

#endif // WIDGETSEARCHRESULTS_H
