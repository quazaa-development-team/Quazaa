#include "winmain.h"
#include "ui_winmain.h"

WinMain::WinMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WinMain)
{
    ui->setupUi(this);
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
