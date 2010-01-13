#include "dialogclosetype.h"
#include "ui_dialogclosetype.h"
#include "quazaasettings.h"

DialogCloseType::DialogCloseType(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::DialogCloseType)
{
	m_ui->setupUi(this);
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
	close();
}
