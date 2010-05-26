#ifndef WIDGETSEARCHRESULTS_H
#define WIDGETSEARCHRESULTS_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
    class WidgetSearchResults;
}

class WidgetSearchResults : public QMainWindow {
    Q_OBJECT
public:
    WidgetSearchResults(QWidget *parent = 0);
    ~WidgetSearchResults();
	QLabel *labelFilter;
	QLineEdit *lineEditFilter;
	void saveWidget();

signals:
	void searchSidebarToggled(bool visible);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearchResults *ui;

public slots:
	void skinChangeEvent();
	void startSearch(QString searchString);
	void startNewSearch(QString *searchString);
	void addSearchTab();

private slots:
	void on_actionSearchToggle_triggered(bool checked);
	void on_actionFilterMore_triggered();
	void on_tabWidgetSearch_tabCloseRequested(int index);
};

#endif // WIDGETSEARCHRESULTS_H
