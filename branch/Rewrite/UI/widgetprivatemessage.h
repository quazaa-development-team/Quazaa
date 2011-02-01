#ifndef WIDGETPRIVATEMESSAGE_H
#define WIDGETPRIVATEMESSAGE_H

#include <QWidget>

namespace Ui {
    class WidgetPrivateMessage;
}

class WidgetPrivateMessage : public QWidget {
    Q_OBJECT
public:
    WidgetPrivateMessage(QWidget *parent = 0);
    ~WidgetPrivateMessage();
	QString inputMessage;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetPrivateMessage *ui;

public slots:
	//void addMessage(QString message);
};

#endif // WIDGETPRIVATEMESSAGE_H
