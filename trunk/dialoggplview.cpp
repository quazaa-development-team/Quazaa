#include "dialoggplview.h"
#include "ui_dialoggplview.h"
#include <QDesktopServices>

DialogGPLView::DialogGPLView(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DialogGPLView)
{
	m_ui->setupUi(this);
}

DialogGPLView::~DialogGPLView()
{
    delete m_ui;
}

void DialogGPLView::changeEvent(QEvent *e)
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

void DialogGPLView::on_pushButtonOK_clicked()
{
	close();
}

void DialogGPLView::on_textBrowserGPL_anchorClicked(QUrl link)
{
	QDesktopServices::openUrl(link);
}
