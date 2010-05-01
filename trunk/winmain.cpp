#include "winmain.h"
#include "ui_winmain.h"

WinMain::WinMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WinMain)
{
    ui->setupUi(this);
    pageHome = new WidgetHome();
    ui->stackedWidgetMain->addWidget(pageHome);
    pageLibrary = new WidgetLibrary();
    ui->stackedWidgetMain->addWidget(pageLibrary);
    pageMedia = new WidgetMedia();
    ui->stackedWidgetMain->addWidget(pageMedia);
}

WinMain::~WinMain()
{
    delete ui;
}

void WinMain::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
