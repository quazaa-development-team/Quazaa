#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QSkinDialog/qskinsettings.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	actionGroupMainNavigation = new QActionGroup(this);
	actionGroupMainNavigation->addAction(ui->actionHome);
	actionGroupMainNavigation->addAction(ui->actionLibrary);
	actionGroupMainNavigation->addAction(ui->actionMedia);
	actionGroupMainNavigation->addAction(ui->actionSearch);
	actionGroupMainNavigation->addAction(ui->actionTransfers);
	actionGroupMainNavigation->addAction(ui->actionSecurity);
	actionGroupMainNavigation->addAction(ui->actionNetwork);
	actionGroupMainNavigation->addAction(ui->actionChat);
	actionGroupMainNavigation->addAction(ui->actionGeneric);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
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

void MainWindow::on_actionSplash_Screen_triggered()
{
	ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_actionWindow_Frame_triggered()
{
	ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_actionStandard_Items_triggered()
{
	ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_actionExtended_Items_triggered()
{
	ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_actionNavigation_triggered()
{
	ui->stackedWidget->setCurrentIndex(4);
}
