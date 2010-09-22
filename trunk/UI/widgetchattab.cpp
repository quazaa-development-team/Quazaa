#include "widgetchattab.h"
#include "ui_widgetchattab.h"
#include "quazaasettings.h"
#include "QSkinDialog/qskinsettings.h"
#include "systemlog.h"

WidgetChatTab::WidgetChatTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetChatTab)
{
	ui->setupUi(this);
	//connect(lineEditTextInput, SIGNAL(returnPressed()), this, SLOT(on_actionSend_triggered()));
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	//connect(quazaaIrc, SIGNAL(appendMessage(QString)), this, SLOT(append(QString)));
	skinChangeEvent();
	ui->listViewChatUsers->setModel(new QStringListModel());

	/*QSortFilterProxyModel *userSortModel = new QSortFilterProxyModel(this);
	userSortModel->setSourceModel(userList);
	// ToDo: subclass QSortFilterProxyModel to make sorting actually work
	ui->listViewChatUsers->setModel(userSortModel);
	userSortModel->setDynamicSortFilter(true);*/
}

void WidgetChatTab::setName(QString str)
{
	name = str;
}

WidgetChatTab::~WidgetChatTab()
{
	delete ui;
}

void WidgetChatTab::skinChangeEvent()
{
}

void WidgetChatTab::saveWidget()
{
}

void WidgetChatTab::append(QString text) {
	ui->textBrowser->append(text);
}

void WidgetChatTab::channelNames(QStringList names) {
	qDebug() << "My name is >> "+name;
	for (int i = 0; i < names.size(); ++i) {
		qDebug() << "CName: "+names.at(i);
		//ui->listWidgetChatUsers->addItem(names.at(i));
		//new QListWidgetItem(names.at(i), ui->listWidgetChatUsers);
	}
	QStringListModel *model = qobject_cast<QStringListModel*>(ui->listViewChatUsers->model());
	model->setStringList(names);
	//userList->setStringList(names);
	//ui->listWidgetChatUsers->addItem()
}
