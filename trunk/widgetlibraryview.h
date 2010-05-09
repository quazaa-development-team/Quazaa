#ifndef WIDGETLIBRARYVIEW_H
#define WIDGETLIBRARYVIEW_H

#include <QMainWindow>

namespace Ui {
    class WidgetLibraryView;
}

class WidgetLibraryView : public QMainWindow {
    Q_OBJECT
public:
    WidgetLibraryView(QWidget *parent = 0);
    ~WidgetLibraryView();
	void saveState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetLibraryView *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETLIBRARYVIEW_H
