#ifndef DIALOGPRIVATEMESSAGE_H
#define DIALOGPRIVATEMESSAGE_H

#include <QDialog>
#include <QKeyEvent>
#include <QTextDocument>

namespace Ui {
    class DialogPrivateMessage;
}

class DialogPrivateMessage : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrivateMessage(QWidget *parent = 0);
    ~DialogPrivateMessage();

protected:
	void keyPressEvent(QKeyEvent* event);

private:
    Ui::DialogPrivateMessage *ui;

signals:
	void sendMessage(QTextDocument *text);

private slots:
	void on_toolButtonSend_clicked();
	void onMessage(QString);
	void onSystemMessage(QString);
};

#endif // DIALOGPRIVATEMESSAGE_H
