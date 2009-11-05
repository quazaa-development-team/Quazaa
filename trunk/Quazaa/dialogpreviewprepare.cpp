#include "dialogpreviewprepare.h"
#include "ui_dialogpreviewprepare.h"

DialogPreviewPrepare::DialogPreviewPrepare(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DialogPreviewPrepare)
{
    m_ui->setupUi(this);
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
	this->close();
}

void DialogPreviewPrepare::updateProgress(int percent, QString fileName)
{
	m_ui->progressBarStatus->setValue(percent);
	m_ui->labelFileName->setText(fileName);
}
