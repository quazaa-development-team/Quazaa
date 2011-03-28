#include "dialogprivatemessage.h"
#include "ui_dialogprivatemessage.h"

DialogPrivateMessage::DialogPrivateMessage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrivateMessage)
{
    ui->setupUi(this);
}

DialogPrivateMessage::~DialogPrivateMessage()
{
    delete ui;
}
