#include "dialogdownloadsimport.h"
#include "ui_dialogdownloadsimport.h"
#include "QSkinDialog/qskinsettings.h"

DialogDownloadsImport::DialogDownloadsImport(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogDownloadsImport)
{
	ui->setupUi(this);
	ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}

DialogDownloadsImport::~DialogDownloadsImport()
{
	delete ui;
	emit closed();
}

void DialogDownloadsImport::changeEvent(QEvent *e)
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
