#include "dialogpreviewprepare.h"
#include "ui_dialogpreviewprepare.h"
#include "QSkinDialog/qskinsettings.h"

DialogPreviewPrepare::DialogPreviewPrepare(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogPreviewPrepare)
{
	m_ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogPreviewPrepare::~DialogPreviewPrepare()
{
	delete m_ui;
}

void DialogPreviewPrepare::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DialogPreviewPrepare::on_pushButton_clicked()
{
	emit closed();
	close();
}

void DialogPreviewPrepare::updateProgress(int percent, QString fileName)
{
	m_ui->progressBarStatus->setValue(percent);
	m_ui->labelFileName->setText(fileName);
}

void DialogPreviewPrepare::skinChangeEvent()
{
	m_ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}
