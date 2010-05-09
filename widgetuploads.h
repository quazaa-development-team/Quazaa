#ifndef WIDGETUPLOADS_H
#define WIDGETUPLOADS_H

#include <QMainWindow>

namespace Ui {
    class WidgetUploads;
}

class WidgetUploads : public QMainWindow {
    Q_OBJECT
public:
    WidgetUploads(QWidget *parent = 0);
    ~WidgetUploads();
	void saveState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetUploads *ui;

private slots:
	void skinChangeEvent();
};

#endif // WIDGETUPLOADS_H
