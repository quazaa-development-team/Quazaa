#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
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

void MainWindow::on_actionNavigation_2_triggered()
{
	ui->stackedWidget->setCurrentIndex(4);
}
