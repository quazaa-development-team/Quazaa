#include "dialogirccolordialog.h"
#include "ui_dialogirccolordialog.h"

DialogIRCColorDialog::DialogIRCColorDialog(QColor initialColor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogIRCColorDialog)
{
    ui->setupUi(this);
	m_oIRCColor = initialColor;
}

DialogIRCColorDialog::~DialogIRCColorDialog()
{
    delete ui;
}

void DialogIRCColorDialog::on_toolButtonWhite_clicked()
{
	m_oIRCColor.setNamedColor("white");
	accept();
}

void DialogIRCColorDialog::on_toolButtonBlack_clicked()
{
	m_oIRCColor.setNamedColor("black");
	accept();
}

void DialogIRCColorDialog::on_toolButtonNavy_clicked()
{
	m_oIRCColor.setNamedColor("navy");
	accept();
}

void DialogIRCColorDialog::on_toolButtonGreen_clicked()
{
	m_oIRCColor.setNamedColor("green");
	accept();
}

void DialogIRCColorDialog::on_toolButtonGray_clicked()
{
	m_oIRCColor.setNamedColor("gray");
	accept();
}

void DialogIRCColorDialog::on_toolButtonLightGray_clicked()
{
	m_oIRCColor.setNamedColor("lightGray");
	accept();
}

void DialogIRCColorDialog::on_toolButtonBlue_clicked()
{
	m_oIRCColor.setNamedColor("blue");
	accept();
}

void DialogIRCColorDialog::on_toolButtonDarkCyan_clicked()
{
	m_oIRCColor.setNamedColor("darkCyan");
	accept();
}

void DialogIRCColorDialog::on_toolButtonCyan_clicked()
{
	m_oIRCColor.setNamedColor("cyan");
	accept();
}

void DialogIRCColorDialog::on_toolButtonPurple_clicked()
{
	m_oIRCColor.setNamedColor("purple");
	accept();
}

void DialogIRCColorDialog::on_toolButtonMagenta_clicked()
{
	m_oIRCColor.setNamedColor("magenta");
	accept();
}

void DialogIRCColorDialog::on_toolButtonLime_clicked()
{
	m_oIRCColor.setNamedColor("lime");
	accept();
}

void DialogIRCColorDialog::on_toolButtonMaroon_clicked()
{
	m_oIRCColor.setNamedColor("maroon");
	accept();
}

void DialogIRCColorDialog::on_toolButtonRed_clicked()
{
	m_oIRCColor.setNamedColor("red");
	accept();
}

void DialogIRCColorDialog::on_toolButtonOrange_clicked()
{
	m_oIRCColor.setNamedColor("orange");
	accept();
}

void DialogIRCColorDialog::on_toolButtonYellow_clicked()
{
	m_oIRCColor.setNamedColor("yellow");
	accept();
}

void DialogIRCColorDialog::on_toolButton_2_clicked()
{
	reject();
}
