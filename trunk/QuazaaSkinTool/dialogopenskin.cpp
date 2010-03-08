#include "dialogopenskin.h"
#include "ui_dialogopenskin.h"
#include "../QSkinDialog/qskinsettings.h"
#include <QDir>
#include <QSettings>

DialogOpenSkin::DialogOpenSkin(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogOpenSkin)
{
	ui->setupUi(this);

	QDir dir(qApp->applicationDirPath() + "/Skin/");
	QFileInfoList skinDirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
	foreach(QFileInfo i, skinDirs)
	{
		QListWidgetItem *item = new QListWidgetItem(i.fileName());
		ui->listWidgetSkins->addItem(item);
	}
}

DialogOpenSkin::~DialogOpenSkin()
{
	delete ui;
}

void DialogOpenSkin::changeEvent(QEvent *e)
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

void DialogOpenSkin::on_pushButtonCancel_clicked()
{
	reject();
}

void DialogOpenSkin::on_pushButtonOK_clicked()
{
	QFile *m_skinFile = new QFile(skinFile);
	if (m_skinFile->exists(skinFile))
	{
		accept();
	} else {
		reject();
	}
}

void DialogOpenSkin::on_listWidgetSkins_itemClicked(QListWidgetItem* item)
{
	QSettings reader((qApp->applicationDirPath() + "/Skin/" + item->text() + "/" + item->text() + ".qsk"), QSettings::IniFormat);
	skinFile = (qApp->applicationDirPath() + "/Skin/" + item->text() + "/" + item->text() + ".qsk");
	ui->labelSkinAuthor->setText(reader.value("skinAuthor", "").toString());
	ui->labelSkinVersion->setText(reader.value("skinVersion", "").toString());
	ui->plainTextEditSkinDescription->setPlainText(reader.value("skinDescription", "").toString());
}
