#ifndef DIALOGPRIVATEMESSAGECONNECT_H
#define DIALOGPRIVATEMESSAGECONNECT_H

#include <QDialog>

#include "network.h"

namespace PMNetwork {
	enum PMNetwork{G2 = 0, eDonkey, Ares, IRC};
}

namespace Ui {
    class DialogPrivateMessageConnect;
}

class DialogPrivateMessageConnect : public QDialog {
    Q_OBJECT
public:
    DialogPrivateMessageConnect(QWidget *parent = 0);
    ~DialogPrivateMessageConnect();

signals:
	void startG2PrivateMessage(CEndPoint ip);
	void startEDonkeyPrivateMessage(CEndPoint ip);
	void startAresPrivateMessage(CEndPoint ip);
	void startIRCPrivateMessage(QString nick);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DialogPrivateMessageConnect *ui;

private slots:
	void on_pushButtonCancel_clicked();
	void on_pushButtonConnect_clicked();
};

#endif // DIALOGPRIVATEMESSAGECONNECT_H
