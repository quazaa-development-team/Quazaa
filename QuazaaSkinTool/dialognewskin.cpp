#include "dialognewskin.h"
#include "ui_dialognewskin.h"

DialogNewSkin::DialogNewSkin(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogNewSkin)
{
	ui->setupUi(this);
}

DialogNewSkin::~DialogNewSkin()
{
	delete ui;
}

void DialogNewSkin::changeEvent(QEvent *e)
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

void DialogNewSkin::on_pushButtonOK_clicked()
{
	name = ui->lineEditName->text();
	author = ui->lineEditAuthor->text();
	version = ui->lineEditVersion->text();
	description = ui->plainTextEditDescription->toPlainText();
	accept();
}

void DialogNewSkin::on_pushButtonCancel_clicked()
{
	reject();
}
