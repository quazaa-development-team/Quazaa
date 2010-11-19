#ifndef DIALOGPRIVATEMESSAGE_H
#define DIALOGPRIVATEMESSAGE_H

#include <QDialog>

namespace Ui {
    class DialogPrivateMessage;
}

class DialogPrivateMessage : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrivateMessage(QWidget *parent = 0);
    ~DialogPrivateMessage();

private:
    Ui::DialogPrivateMessage *ui;
};

#endif // DIALOGPRIVATEMESSAGE_H
