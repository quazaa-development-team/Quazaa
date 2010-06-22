#ifndef DIALOGCONNECTTO_H
#define DIALOGCONNECTTO_H

#include <QDialog>
#include "types.h"

namespace Ui {
    class DialogConnectTo;
}

class DialogConnectTo : public QDialog {
    Q_OBJECT
public:
    DialogConnectTo(QWidget *parent = 0);
    ~DialogConnectTo();

signals:
	void closed();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DialogConnectTo *ui;

private slots:
	void on_pushButtonConnect_clicked();
	void on_pushButtonCancel_clicked();
	void skinChangeEvent();
};

#endif // DIALOGCONNECTTO_H
