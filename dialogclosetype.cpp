#include "dialogclosetype.h"
#include "ui_dialogclosetype.h"
#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"

DialogCloseType::DialogCloseType(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogCloseType)
{
	m_ui->setupUi(this);
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogCloseType::~DialogCloseType()
{
	delete m_ui;
}

void DialogCloseType::changeEvent(QEvent *e)
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

void DialogCloseType::on_pushButtonOK_clicked()
{
	quazaaSettings.BasicCloseMode = m_ui->comboBoxCloseType->currentIndex() + 1;
	emit closed();
	close();
}

void DialogCloseType::skinChangeEvent()
{
	m_ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}
