#include "dialogircsettings.h"
#include "ui_dialogircsettings.h"

#include "quazaasettings.h"

#include <QStyledItemDelegate>
#include <QComboBox>

// SVG color keyword names provided by the World Wide Web Consortium
static const QStringList COLORS = QStringList()
		<< "aliceblue" << "antiquewhite" << "aqua" << "aquamarine" << "azure" << "beige" << "bisque"
		<< "black" << "blanchedalmond" << "blue" << "blueviolet" << "brown" << "burlywood" << "cadetblue"
		<< "chartreuse" << "chocolate" << "coral" << "cornflowerblue" << "cornsilk" << "crimson" << "cyan"
		<< "darkblue" << "darkcyan" << "darkgoldenrod" << "darkgray" << "darkgreen" << "darkgrey"
		<< "darkkhaki" << "darkmagenta" << "darkolivegreen" << "darkorange" << "darkorchid" << "darkred"
		<< "darksalmon" << "darkseagreen" << "darkslateblue" << "darkslategray" << "darkslategrey"
		<< "darkturquoise" << "darkviolet" << "deeppink" << "deepskyblue" << "dimgray" << "dimgrey"
		<< "dodgerblue" << "firebrick" << "floralwhite" << "forestgreen" << "fuchsia" << "gainsboro"
		<< "ghostwhite" << "gold" << "goldenrod" << "gray" << "grey" << "green" << "greenyellow"
		<< "honeydew" << "hotpink" << "indianred" << "indigo" << "ivory" << "khaki" << "lavender"
		<< "lavenderblush" << "lawngreen" << "lemonchiffon" << "lightblue" << "lightcoral" << "lightcyan"
		<< "lightgoldenrodyellow" << "lightgray" << "lightgreen" << "lightgrey" << "lightpink"
		<< "lightsalmon" << "lightseagreen" << "lightskyblue" << "lightslategray" << "lightslategrey"
		<< "lightsteelblue" << "lightyellow" << "lime" << "limegreen" << "linen" << "magenta"
		<< "maroon" << "mediumaquamarine" << "mediumblue" << "mediumorchid" << "mediumpurple"
		<< "mediumseagreen" << "mediumslateblue" << "mediumspringgreen" << "mediumturquoise"
		<< "mediumvioletred" << "midnightblue" << "mintcream" << "mistyrose" << "moccasin"
		<< "navajowhite" << "navy" << "oldlace" << "olive" << "olivedrab" << "orange" << "orangered"
		<< "orchid" << "palegoldenrod" << "palegreen" << "paleturquoise" << "palevioletred"
		<< "papayawhip" << "peachpuff" << "peru" << "pink" << "plum" << "powderblue" << "purple" << "red"
		<< "rosybrown" << "royalblue" << "saddlebrown" << "salmon" << "sandybrown" << "seagreen"
		<< "seashell" << "sienna" << "silver" << "skyblue" << "slateblue" << "slategray" << "slategrey"
		<< "snow" << "springgreen" << "steelblue" << "tan" << "teal" << "thistle" << "tomato"
		<< "turquoise" << "violet" << "wheat" << "white" << "whitesmoke" << "yellow" << "yellowgreen";


class ColorItemDelegate : public QStyledItemDelegate
{
public:
	ColorItemDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {
	}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
		Q_UNUSED(option);
		if (index.column() == ColorColumns::Color) {
			QComboBox* comboBox = new QComboBox(parent);
			comboBox->addItems(COLORS);
			int i = 0;
			foreach(const QString & color, COLORS)
			{
				comboBox->setItemData(i++, QColor(color), Qt::DecorationRole);
			}
			return comboBox;
		}
		return 0;
	}

	void setEditorData(QWidget* editor, const QModelIndex& index) const {
		if (index.column() == ColorColumns::Color) {
			QComboBox* comboBox = static_cast<QComboBox*>(editor);
			int i = comboBox->findText(index.data().toString());
			comboBox->setCurrentIndex(i);
		}
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
		if (index.column() == ColorColumns::Color) {
			QComboBox* comboBox = static_cast<QComboBox*>(editor);
			model->setData(index, comboBox->currentText());
			model->setData(index, QColor(comboBox->currentText()), Qt::DecorationRole);
		}
	}
};

class ShortcutItemDelegate : public QStyledItemDelegate
{
public:
	ShortcutItemDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) { }
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& opt, const QModelIndex& index) const {
		QWidget* editor = 0;
		if (index.column() == ShortcutColumns::Shortcut)
			editor = QStyledItemDelegate::createEditor(parent, opt, index);
		return editor;
	}
};

DialogIrcSettings::DialogIrcSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogIrcSettings)
{
	ui->setupUi(this);

	setWindowFilePath(tr("Settings"));

	ui->treeWidgetColors->setItemDelegate(new ColorItemDelegate(ui->treeWidgetColors));
	ui->treeWidgetShortcuts->setItemDelegate(new ShortcutItemDelegate(ui->treeWidgetShortcuts));
#if QT_VERSION >= 0x050000
	ui->treeWidgetColors->header()->setSectionResizeMode(ColorColumns::Message, QHeaderView::ResizeToContents);
	ui->treeWidgetShortcuts->header()->setSectionResizeMode(ShortcutColumns::Description, QHeaderView::ResizeToContents);
#else
    ui->treeWidgetColors->header()->setResizeMode(ColorColumns::Message, QHeaderView::ResizeToContents);
    ui->treeWidgetShortcuts->header()->setResizeMode(ShortcutColumns::Description, QHeaderView::ResizeToContents);
#endif
	ui->treeWidgetShortcuts->expandItem(ui->treeWidgetShortcuts->topLevelItem(0));
	ui->treeWidgetShortcuts->expandItem(ui->treeWidgetShortcuts->topLevelItem(1));

	loadSettings();

	connect(ui->checkBoxConnectOnStartup, SIGNAL(toggled(bool)), this, SLOT(enableApply()));
	connect(ui->checkBoxStripNicks, SIGNAL(toggled(bool)), this, SLOT(enableApply()));
	connect(ui->checkBoxTimeStamp, SIGNAL(toggled(bool)), this, SLOT(enableApply()));
	connect(ui->comboBoxLayout, SIGNAL(currentIndexChanged(int)), this, SLOT(enableApply()));
	connect(ui->spinBoxBlockCount, SIGNAL(valueChanged(int)), this, SLOT(enableApply()));
	connect(ui->treeWidgetColors, SIGNAL(clicked(QModelIndex)), this, SLOT(enableApply()));
	connect(ui->treeWidgetShortcuts, SIGNAL(clicked(QModelIndex)), this, SLOT(enableApply()));

	connect(ui->pushButtonApply, SIGNAL(clicked()), this, SLOT(saveSettings()));
	ui->pushButtonApply->setEnabled(false);
}

DialogIrcSettings::~DialogIrcSettings()
{
	delete ui;
}

void DialogIrcSettings::saveSettings()
{
	quazaaSettings.Chat.ConnectOnStartup = ui->checkBoxConnectOnStartup->isChecked();
	quazaaSettings.Chat.MaxBlockCount = ui->spinBoxBlockCount->value();
	quazaaSettings.Chat.ShowTimestamp = ui->checkBoxTimeStamp->isChecked();

	QHash<int, QString> colors;
	for (int i = IrcColorType::Background; i <= IrcColorType::Link; ++i)
		colors[i] = ui->treeWidgetColors->topLevelItem(i)->data(ColorColumns::Color, Qt::DisplayRole).toString();
	quazaaSettings.Chat.Colors = colors;
	quazaaSettings.Chat.Layout = ui->comboBoxLayout->currentText().toLower();

	QTreeWidgetItem* navigate = ui->treeWidgetShortcuts->topLevelItem(0);
	for (int i = IrcShortcutType::NavigateUp; i <= IrcShortcutType::NavigateRight; ++i) {
		QTreeWidgetItem* item = navigate->child(i);
		quazaaSettings.Chat.Shortcuts.insert(i, item->text(ShortcutColumns::Shortcut));
	}

	QTreeWidgetItem* unread = ui->treeWidgetShortcuts->topLevelItem(1);
	for (int i = IrcShortcutType::NextUnreadUp; i <= IrcShortcutType::NextUnreadRight; ++i) {
		QTreeWidgetItem* item = unread->child(i - IrcShortcutType::NextUnreadUp);
		quazaaSettings.Chat.Shortcuts.insert(i, item->text(ShortcutColumns::Shortcut));
	}

	quazaaSettings.saveChat();

	ui->pushButtonApply->setEnabled(false);
}

void DialogIrcSettings::loadSettings()
{
	ui->checkBoxConnectOnStartup->setChecked(quazaaSettings.Chat.ConnectOnStartup);
	ui->spinBoxBlockCount->setValue(quazaaSettings.Chat.MaxBlockCount);
	ui->checkBoxTimeStamp->setChecked(quazaaSettings.Chat.ShowTimestamp);
	ui->comboBoxLayout->setCurrentIndex((quazaaSettings.Chat.Layout == "Tabs") ? 0 : 1);

	QHashIterator<int, QString> it(quazaaSettings.Chat.Colors);
	while (it.hasNext()) {
		it.next();
		ui->treeWidgetColors->topLevelItem(it.key())->setData(ColorColumns::Color, Qt::DisplayRole, it.value());
		ui->treeWidgetColors->topLevelItem(it.key())->setData(ColorColumns::Color, Qt::DecorationRole, QColor(it.value()));
	}

	QTreeWidgetItem* navigate = ui->treeWidgetShortcuts->topLevelItem(0);
	for (int i = IrcShortcutType::NavigateUp; i <= IrcShortcutType::NavigateRight; ++i) {
		QTreeWidgetItem* item = navigate->child(i);
		item->setText(ShortcutColumns::Shortcut, quazaaSettings.Chat.Shortcuts.value(i));
	}

	QTreeWidgetItem* unread = ui->treeWidgetShortcuts->topLevelItem(1);
	for (int i = IrcShortcutType::NextUnreadUp; i <= IrcShortcutType::NextUnreadRight; ++i) {
		QTreeWidgetItem* item = unread->child(i - IrcShortcutType::NextUnreadUp);
		item->setText(ShortcutColumns::Shortcut, quazaaSettings.Chat.Shortcuts.value(i));
	}
}

void DialogIrcSettings::on_pushButtonOK_clicked()
{
	if(ui->pushButtonApply->isEnabled())
		saveSettings();
	accept();
}

void DialogIrcSettings::enableApply()
{
	ui->pushButtonApply->setEnabled(true);
}
