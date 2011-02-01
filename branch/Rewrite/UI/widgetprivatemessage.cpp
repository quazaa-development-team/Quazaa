#include "widgetprivatemessage.h"
#include "ui_widgetprivatemessage.h"

WidgetPrivateMessage::WidgetPrivateMessage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetPrivateMessage)
{
	ui->setupUi(this);
}

WidgetPrivateMessage::~WidgetPrivateMessage()
{
    delete ui;
}

void WidgetPrivateMessage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


