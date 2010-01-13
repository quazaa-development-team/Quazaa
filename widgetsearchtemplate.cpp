#include "widgetsearchtemplate.h"
#include "ui_widgetsearchtemplate.h"

WidgetSearchTemplate::WidgetSearchTemplate(QWidget *parent) :
	QWidget(parent),
    m_ui(new Ui::WidgetSearchTemplate)
{
	m_ui->setupUi(this);
}

WidgetSearchTemplate::~WidgetSearchTemplate()
{
    delete m_ui;
}

void WidgetSearchTemplate::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
