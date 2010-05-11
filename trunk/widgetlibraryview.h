#ifndef WIDGETLIBRARYVIEW_H
#define WIDGETLIBRARYVIEW_H

#include <QMainWindow>
#include <QLineEdit>

namespace Ui {
    class WidgetLibraryView;
}

class WidgetLibraryView : public QMainWindow {
    Q_OBJECT
public:
    WidgetLibraryView(QWidget *parent = 0);
    ~WidgetLibraryView();
	QLineEdit *lineEditFind;
	void saveWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetLibraryView *ui;

private slots:
	void on_actionFind_triggered();
	void skinChangeEvent();
};

#endif // WIDGETLIBRARYVIEW_H
