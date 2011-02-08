#include "wizardquickstart.h"
#include "ui_wizardquickstart.h"

WizardQuickStart::WizardQuickStart(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::WizardQuickStart)
{
    ui->setupUi(this);
}

WizardQuickStart::~WizardQuickStart()
{
    delete ui;
}
