#include "dialoglibrarysearch.h"
#include "ui_dialoglibrarysearch.h"

DialogLibrarySearch::DialogLibrarySearch(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogLibrarySearch)
{
	ui->setupUi(this);
}

DialogLibrarySearch::~DialogLibrarySearch()
{
	delete ui;
}

void DialogLibrarySearch::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogLibrarySearch::on_pushButtonSearch_clicked()
{
	emit closed();
	close();
}

void DialogLibrarySearch::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}
