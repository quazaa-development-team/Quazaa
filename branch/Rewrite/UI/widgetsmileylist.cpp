#include "widgetsmileylist.h"
#include "ui_widgetsmileylist.h"

WidgetSmileyList::WidgetSmileyList(QWidget *parent) :
    ui(new Ui::WidgetSmileyList)
{
    ui->setupUi(this);
	setWindowFlags(windowFlags() |= Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	connect(this, SIGNAL(aboutToShow()), ui->listWidget, SLOT(clearSelection()));
}

WidgetSmileyList::~WidgetSmileyList()
{
    delete ui;
}

void WidgetSmileyList::changeEvent(QEvent *e)
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

void WidgetSmileyList::on_listWidget_itemClicked(QListWidgetItem* item)
{
	emit smileyClicked(item->toolTip());
	close();
}
