#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../QSkinDialog/qskinsettings.h"
#include "dialognewskin.h"
#include "dialogopenskin.h"
#include "private/qcssparser_p.h"
#include "csshighlighter.h"
#include <QDesktopServices>
#include "qtgradientmanager.h"
#include "qtgradientviewdialog.h"
#include "qtgradientutils.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QSignalMapper>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->stackedWidget->setCurrentIndex(0);
	isMainWindow = true;
	actionGroupMainNavigation = new QActionGroup(this);
	actionGroupMainNavigation->addAction(ui->actionHome);
	actionGroupMainNavigation->addAction(ui->actionLibrary);
	actionGroupMainNavigation->addAction(ui->actionMedia);
	actionGroupMainNavigation->addAction(ui->actionSearch);
	actionGroupMainNavigation->addAction(ui->actionTransfers);
	actionGroupMainNavigation->addAction(ui->actionSecurity);
	actionGroupMainNavigation->addAction(ui->actionNetwork);
	actionGroupMainNavigation->addAction(ui->actionChat);
	actionGroupMainNavigation->addAction(ui->actionGeneric);

	isMainWindow = true;

	ui->plainTextEditStyleSheet->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->plainTextEditStyleSheet, SIGNAL(customContextMenuRequested(QPoint)),
				this, SLOT(slotContextMenuRequested()));

	m_addImageAction = new QAction(tr("Add Image..."), this);
	m_addGradientAction = new QAction(tr("Add Gradient..."), this);
	m_addColorAction = new QAction(tr("Add Color..."), this);
	m_addFontAction = new QAction(tr("Add Font..."), this);
	m_addImageAction->setEnabled(false);
	m_addGradientAction->setEnabled(false);
	m_addColorAction->setEnabled(false);
	m_addFontAction->setEnabled(false);

	QSignalMapper *imageActionMapper = new QSignalMapper(this);
	QSignalMapper *gradientActionMapper = new QSignalMapper(this);
	QSignalMapper *colorActionMapper = new QSignalMapper(this);

	imageActionMapper->setMapping(m_addImageAction, QString());
	gradientActionMapper->setMapping(m_addGradientAction, QString());
	colorActionMapper->setMapping(m_addColorAction, QString());

	connect(m_addImageAction, SIGNAL(triggered()), imageActionMapper, SLOT(map()));
	connect(m_addGradientAction, SIGNAL(triggered()), gradientActionMapper, SLOT(map()));
	connect(m_addColorAction, SIGNAL(triggered()), colorActionMapper, SLOT(map()));
	connect(m_addFontAction, SIGNAL(triggered()), this, SLOT(slotAddFont()));

	const char * const imageProperties[] = {
		"background-image",
		"border-image",
		"image",
		0
	};

	const char * const colorProperties[] = {
		"color",
		"background-color",
		"alternate-background-color",
		"border-color",
		"border-top-color",
		"border-right-color",
		"border-bottom-color",
		"border-left-color",
		"gridline-color",
		"selection-color",
		"selection-background-color",
		0
	};

	QMenu *imageActionMenu = new QMenu(this);
	QMenu *gradientActionMenu = new QMenu(this);
	QMenu *colorActionMenu = new QMenu(this);

	ui->statusBarPreview->showMessage("Status Bar");
	ui->statusBarPreview->addPermanentWidget(new QLabel("Status Bar Item", this));

	for (int imageProperty = 0; imageProperties[imageProperty]; ++imageProperty) {
		QAction *action = imageActionMenu->addAction(QLatin1String(imageProperties[imageProperty]));
		connect(action, SIGNAL(triggered()), imageActionMapper, SLOT(map()));
		imageActionMapper->setMapping(action, QLatin1String(imageProperties[imageProperty]));
	}

	for (int colorProperty = 0; colorProperties[colorProperty]; ++colorProperty) {
		QAction *gradientAction = gradientActionMenu->addAction(QLatin1String(colorProperties[colorProperty]));
		QAction *colorAction = colorActionMenu->addAction(QLatin1String(colorProperties[colorProperty]));
		connect(gradientAction, SIGNAL(triggered()), gradientActionMapper, SLOT(map()));
		connect(colorAction, SIGNAL(triggered()), colorActionMapper, SLOT(map()));
		gradientActionMapper->setMapping(gradientAction, QLatin1String(colorProperties[colorProperty]));
		colorActionMapper->setMapping(colorAction, QLatin1String(colorProperties[colorProperty]));
	}

	connect(imageActionMapper, SIGNAL(mapped(QString)), this, SLOT(slotAddImage(QString)));
	connect(gradientActionMapper, SIGNAL(mapped(QString)), this, SLOT(slotAddGradient(QString)));
	connect(colorActionMapper, SIGNAL(mapped(QString)), this, SLOT(slotAddColor(QString)));

	m_addImageAction->setMenu(imageActionMenu);
	m_addGradientAction->setMenu(gradientActionMenu);
	m_addColorAction->setMenu(colorActionMenu);

	ui->toolBarStyleSheetEditor->addAction(m_addImageAction);
	ui->toolBarStyleSheetEditor->addAction(m_addGradientAction);
	ui->toolBarStyleSheetEditor->addAction(m_addColorAction);
	ui->toolBarStyleSheetEditor->addAction(m_addFontAction);

	ui->plainTextEditStyleSheet->setFocus();
	ui->plainTextEditStyleSheet->setTabStopWidth(fontMetrics().width(QLatin1Char(' '))*4);
	new CssHighlighter(ui->plainTextEditStyleSheet->document());
	connect(ui->plainTextEditStyleSheet, SIGNAL(textChanged()), this, SLOT(validateStyleSheet()));

	gradientManager = new QtGradientManager(this);
	saved = true;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	if (!saved && !skinSettings.skinName.isEmpty())
	{
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, "Skin Not Saved", "The skin has not been saved. Would you like to save it now?", QMessageBox::Ok|QMessageBox::Cancel, this);
		bool ok = msgBox->exec();
		if (ok)
			on_actionSave_triggered();
	}
}

void MainWindow::on_actionNew_triggered()
{
	DialogNewSkin *dlgNewSkin = new DialogNewSkin(this);
	bool ok = dlgNewSkin->exec();

	if (ok && !(dlgNewSkin->name.isEmpty() && dlgNewSkin->author.isEmpty()))
	{
		this->enableEditing(true);
		ui->lineEditName->setText(dlgNewSkin->name);
		skinSettings.skinName = dlgNewSkin->name;
		QDir skinPath = QString(qApp->applicationDirPath() + "/Skin/" + ui->lineEditName->text() + "/");
		if (!skinPath.exists())
		{
			skinPath.mkpath(QString(qApp->applicationDirPath() + "/Skin/" + ui->lineEditName->text() + "/"));
		}
		this->setWindowTitle(dlgNewSkin->name + ".qsf" + " - Quazaa Skin Tool");
		ui->lineEditAuthor->setText(dlgNewSkin->author);
		skinSettings.skinAuthor = dlgNewSkin->author;
		ui->lineEditVersion->setText(dlgNewSkin->version);
		skinSettings.skinVersion = dlgNewSkin->version;
		ui->plainTextEditDescription->setPlainText(dlgNewSkin->description);
		skinSettings.skinDescription = dlgNewSkin->description;
		skinSettings.windowFrameTopLeftStyleSheet = "border-image: url(:/Resource/frameTopLeft.png);";
		skinSettings.windowFrameLeftStyleSheet = "border-image: url(:/Resource/frameLeft.png); border-left: 1; border-top: 10;";
		skinSettings.windowFrameBottomLeftStyleSheet = "border-image: url(:/Resource/frameBottomLeft.png);";
		skinSettings.windowFrameTopStyleSheet = "";
		skinSettings.windowFrameBottomStyleSheet = "border-image: url(:/Resource/frameBottom.png); border-bottom: 1;";
		skinSettings.windowFrameTopRightStyleSheet = "border-image: url(:/Resource/frameTopRight.png);";
		skinSettings.windowFrameRightStyleSheet = "QFrame { border-image: url(:/Resource/frameRight.png); border-right: 1; border-top: 10; }";
		skinSettings.windowFrameBottomRightStyleSheet = "border-image: url(:/Resource/frameBottomRight.png);";
		skinSettings.titlebarButtonsFrameStyleSheet = "/*These move the buttons up so they aren't displayed in the center of the titlebar\n   Remove these to center your buttons on the titlebar.*/\nQFrame#titlebarButtonsFrame {\n	padding-top: -1;\n	padding-bottom: 10;\n	border-image: url(:/Resource/titlebarButtonsFrame.png);\n}";
		skinSettings.minimizeButtonStyleSheet = "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/minButton.png); } QToolButton:hover { border-image: url(:/Resource/minButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/minButtonD.png); }";
		skinSettings.maximizeButtonStyleSheet = "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/maxButton.png); } QToolButton:hover { border-image: url(:/Resource/maxButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/maxButtonD.png); } QToolButton:checked { border-image: url(:/Resource/restoreButton.png); } QToolButton:checked:hover { border-image: url(:/Resource/restoreButtonH.png); } QToolButton:checked:disabled { border-image: url(:/Resource/restoreButtonD.png); }";
		skinSettings.closeButtonStyleSheet = "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/quitButton.png); } QToolButton:hover { border-image: url(:/Resource/quitButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/quitButtonD.png); }";
		skinSettings.windowFrameTopSpacerStyleSheet = "QFrame#windowFrameTopSpacer {\n	border-image: url(:/Resource/frameTop.png);\n}";
		skinSettings.windowTextStyleSheet = "border-image: url(:/Resource/windowTextBackground.png);\npadding-left: -2px;\npadding-right: -2px;\npadding-bottom: 2px;\nfont-weight: bold;\nfont-size: 16px;\ncolor: rgb(255, 255, 255);";
		skinSettings.windowIconFrameStyleSheet = "QFrame#windowIconFrame {\n	border-image: url(:/Resource/windowIconFrame.png);\n}";
		skinSettings.windowIconVisible = true;
		skinSettings.windowIconSize = QSize(20, 20);

		// Child Window Frame
		skinSettings.childWindowFrameTopLeftStyleSheet = "border-image: url(:/Resource/frameTopLeft.png);";
		skinSettings.childWindowFrameLeftStyleSheet = "border-image: url(:/Resource/frameLeft.png); border-left: 1; border-top: 10;";
		skinSettings.childWindowFrameBottomLeftStyleSheet = "border-image: url(:/Resource/frameBottomLeft.png);";
		skinSettings.childWindowFrameTopStyleSheet = "";
		skinSettings.childWindowFrameBottomStyleSheet = "border-image: url(:/Resource/frameBottom.png); border-bottom: 1;";
		skinSettings.childWindowFrameTopRightStyleSheet = "border-image: url(:/Resource/frameTopRight.png);";
		skinSettings.childWindowFrameRightStyleSheet = "QFrame { border-image: url(:/Resource/frameRight.png); border-right: 1; border-top: 10; }";
		skinSettings.childWindowFrameBottomRightStyleSheet = "border-image: url(:/Resource/frameBottomRight.png);";
		skinSettings.childTitlebarButtonsFrameStyleSheet = "/*These move the buttons up so they aren't displayed in the center of the titlebar\n   Remove these to center your buttons on the titlebar.*/\nQFrame#titlebarButtonsFrame {\n	padding-top: -1;\n	padding-bottom: 10;\n	border-image: url(:/Resource/titlebarButtonsFrame.png);\n}";
		skinSettings.childMinimizeButtonStyleSheet = "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/minButton.png); } QToolButton:hover { border-image: url(:/Resource/minButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/minButtonD.png); }";
		skinSettings.childMaximizeButtonStyleSheet = "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/maxButton.png); } QToolButton:hover { border-image: url(:/Resource/maxButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/maxButtonD.png); } QToolButton:checked { border-image: url(:/Resource/restoreButton.png); } QToolButton:checked:hover { border-image: url(:/Resource/restoreButtonH.png); } QToolButton:checked:disabled { border-image: url(:/Resource/restoreButtonD.png); }";
		skinSettings.childCloseButtonStyleSheet = "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/quitButton.png); } QToolButton:hover { border-image: url(:/Resource/quitButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/quitButtonD.png); }";
		skinSettings.childWindowFrameTopSpacerStyleSheet = "QFrame#windowFrameTopSpacer {\n	border-image: url(:/Resource/frameTop.png);\n}";
		skinSettings.childWindowTextStyleSheet = "border-image: url(:/Resource/windowTextBackground.png);\npadding-left: -2px;\npadding-right: -2px;\npadding-bottom: 2px;\nfont-weight: bold;\nfont-size: 16px;\ncolor: rgb(255, 255, 255);";
		skinSettings.childWindowIconFrameStyleSheet = "QFrame#windowIconFrame {\n	border-image: url(:/Resource/windowIconFrame.png);\n}";
		skinSettings.childWindowIconVisible = true;
		skinSettings.childWindowIconSize = QSize(20, 20);

		// Splash Screen
		skinSettings.splashBackground = "QFrame {\n	border-image: url(:/Resource/Splash.png) repeat;\n}";
		skinSettings.splashLogo = "background-color: transparent;\nmin-height: 172px;\nmax-height: 172px;\nmin-width: 605px;\nmax-width: 605px;\nborder-image: url(:/Resource/QuazaaLogo.png);";
		skinSettings.splashFooter = "QFrame {\n	border-image: url(:/Resource/HeaderBackground.png) repeat;\n	min-height: 28;\n	max-height: 28;\n	min-width: 605px;\n	max-width: 605px;\n}";
		skinSettings.splashProgress = "font-weight: bold;\nmax-height: 10px;\nmin-height: 10px;\nmax-width: 300px;\nmin-width: 300px;";
		skinSettings.splashStatus = "font-weight: bold;\ncolor: white;\nbackground-color: transparent;\npadding-left: 5px;";

		// Standard Items
		skinSettings.standardItems = "";

		// Sidebar
		skinSettings.sidebarBackground = "QFrame {\n	 background-color: rgb(199, 202, 255);\n}";
		skinSettings.sidebarTaskBackground = "QFrame {\n	background-color: rgb(161, 178, 231);\n}";
		skinSettings.sidebarTaskHeader = "QToolButton {\n	background-color: rgb(78, 124, 179);\n	color: rgb(255, 255, 255);\n	border: none;\n	font-size: 16px;\n	font-weight: bold;\n}\n\nQToolButton:hover {\n	background-color: rgb(56, 90, 129);\n}";
		skinSettings.sidebarUnclickableTaskHeader = "QToolButton {\n	background-color: rgb(78, 124, 179);\n	color: rgb(255, 255, 255);\n	border: none;\n	font-size: 16px;\n	font-weight: bold;\n}";

		// Toolbars
		skinSettings.toolbars = "";
		skinSettings.navigationToolbar = "";

		// Headers
		skinSettings.genericHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.homeHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.libraryHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.mediaHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.searchHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.transfersHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.securityHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.networkHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.chatHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";
		skinSettings.dialogHeader = "font-size: 15px;\nfont-weight: bold;\ncolor: rgb(255, 255, 255);\nbackground-image: url(:/Resource/HeaderBackground.png);";

		// Media
		skinSettings.seekSlider = "QSlider::groove:horizontal {	\n	border: 1px solid rgb(82, 111, 174); \n	height: 22px; \n	background: black; \n	margin: 3px 0; \n}	\n\nQSlider::handle:horizontal { \n	background: qlineargradient(spread:pad, x1:0.510526, y1:0, x2:0.511, y2:1, stop:0 rgba(206, 215, 255, 255), stop:0.184211 rgba(82, 107, 192, 255), stop:0.342105 rgba(55, 80, 167, 255), stop:0.484211 rgba(17, 26, 148, 255), stop:0.636842 rgba(0, 0, 0, 255), stop:0.8 rgba(24, 46, 171, 255), stop:0.984211 rgba(142, 142, 255, 255)); \n	border: 1px solid rgb(82, 111, 174); \n	border-radius: 0px; \n	width: 4px; \n	margin: 1px 0; /* handle is placed by default on the contents rect of the groove. Expand outside the groove */ \n}";
		skinSettings.volumeSlider = "QSlider::groove:horizontal { \n	border: 1px solid rgb(0, 61, 89); \n	height: 3px;  \n	background: black; \n	margin: 2px 0; \n} \n\nQSlider::handle:horizontal { \n	background: qlineargradient(spread:pad, x1:0.510526, y1:0, x2:0.511, y2:1, stop:0 rgba(206, 215, 255, 255), stop:0.184211 rgba(82, 107, 192, 255), stop:0.342105 rgba(55, 80, 167, 255), stop:0.484211 rgba(17, 26, 148, 255), stop:0.636842 rgba(0, 0, 0, 255), stop:0.8 rgba(24, 46, 171, 255), stop:0.984211 rgba(142, 142, 255, 255)); \n	border: 1px solid rgb(82, 111, 174); \n	width: 6px; \n	margin: -4px 0; \n} ";
		skinSettings.mediaToolbar = "";

		// Chat
		skinSettings.chatWelcome = "QFrame {\n	background-color: rgb(78, 124, 179);\n	color: rgb(255, 255, 255);\n}";
		skinSettings.chatToolbar = "QFrame {\n	background-color: rgb(199, 202, 255);\n}";

		// Specialised Tab Widgets
		skinSettings.libraryNavigator = "QTabWidget::pane { /* The tab widget frame */\n     border-top: 2px solid transparent;\n }\n\n QTabWidget::tab-bar {\n     left: 5px;  /* move to the right by 5px */\n }\n\n /* Style the tab using the tab sub-control. Note that\n     it reads QTabBar _not_ QTabWidget */\n QTabBar::tab {\n     background:transparent;\n     border: 1px solid transparent;\n     padding: 4px;\n }\n\n QTabBar::tab:selected, QTabBar::tab:hover {\n     border: 1px solid rgb(78, 96, 255);\n     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);\n }";
		skinSettings.tabSearches = "QTabWidget::pane { /* The tab widget frame */\n     border-top: 2px solid #C2C7CB;\n }\n\nQTabWidget::pane { /* The tab widget frame */\n     border-top: 2px solid transparent;\n }\n\n QTabWidget::tab-bar {\n     left: 5px;  /* move to the right by 5px */\n }\n\n /* Style the tab using the tab sub-control. Note that\n     it reads QTabBar _not_ QTabWidget */\n QTabBar::tab {\n     background:transparent;\n     border: 1px solid transparent;\n     padding: 4px;\n }\n\n QTabBar::tab:selected, QTabBar::tab:hover {\n     border: 1px solid rgb(78, 96, 255);\n     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);\n }";

		skinChangeEvent();
		saved = false;
	}
}

void MainWindow::slotContextMenuRequested()
{
	QMenu *menu = ui->plainTextEditStyleSheet->createStandardContextMenu();
	menu->addSeparator();
	menu->addAction(m_addImageAction);
	menu->addAction(m_addGradientAction);
	menu->exec(QCursor::pos());
	delete menu;
}

void MainWindow::validateStyleSheet()
{
	const bool valid = isStyleSheetValid(ui->plainTextEditStyleSheet->toPlainText());
	if (valid) {
		ui->labelValidStyleSheet->setText(tr("Valid Style Sheet"));
		ui->labelValidStyleSheet->setStyleSheet(QLatin1String("margin-left: 2px; color: green;"));
	} else {
		ui->labelValidStyleSheet->setText(tr("Invalid Style Sheet"));
		ui->labelValidStyleSheet->setStyleSheet(QLatin1String("margin-left: 2px; color: red;"));
	}
}

bool MainWindow::isStyleSheetValid(const QString &styleSheet)
{
	QCss::Parser parser(styleSheet);
	QCss::StyleSheet sheet;
	if (parser.parse(&sheet))
		return true;
	QString fullSheet = QLatin1String("* { ");
	fullSheet += styleSheet;
	fullSheet += QLatin1Char('}');
	QCss::Parser parser2(fullSheet);
	return parser2.parse(&sheet);
}

void MainWindow::slotAddImage(const QString &property)
{
	QString path = "";
	QString copyPath = QFileDialog::getOpenFileName(this, tr("Open Image"),
													QString(qApp->applicationDirPath() + "/Skin/" + skinSettings.skinName + "/"),
														   tr("Images (*.png *.xpm *.jpg *.gif *.bmp)"));

	if (!copyPath.isEmpty())
	{
		QString destinationPath = "";
		QDir skinPath = QString(qApp->applicationDirPath() + "/Skin/" + skinSettings.skinName + "/");
		if (!skinPath.exists())
		{
			skinPath.mkpath(QString(qApp->applicationDirPath() + "/Skin/" + skinSettings.skinName + "/"));
		}

		if (!copyPath.contains(QString(qApp->applicationDirPath()) + "/Skin/" + skinSettings.skinName + "/"))
		{
			QFileInfo fileInfo(copyPath);
			destinationPath = qApp->applicationDirPath() + "/Skin/" + ui->lineEditName->text() + "/" + fileInfo.fileName();
			destinationPath = destinationPath.replace("//", "/", Qt::CaseInsensitive);
			QFile::copy(copyPath, destinationPath);
		} else {
			destinationPath = copyPath;
			destinationPath = destinationPath.replace("//", "/", Qt::CaseInsensitive);
		}

		path = destinationPath.remove(QApplication::applicationDirPath() + "/");

		if (!path.isEmpty())
			insertCssProperty(property, QString(QLatin1String("url(%1)")).arg(path));
	}
}

void MainWindow::slotAddGradient(const QString &property)
{
	bool ok;
	const QGradient grad = QtGradientViewDialog::getGradient(&ok, gradientManager, this);
	if (ok)
		insertCssProperty(property, QtGradientUtils::styleSheetCode(grad));
}

void MainWindow::slotAddColor(const QString &property)
{
	const QColor color = QColorDialog::getColor(0xffffffff, this, QString(), QColorDialog::ShowAlphaChannel);
	if (!color.isValid())
		return;

	QString colorStr;

	if (color.alpha() == 255) {
		colorStr = QString(QLatin1String("rgb(%1, %2, %3)")).arg(
				color.red()).arg(color.green()).arg(color.blue());
	} else {
		colorStr = QString(QLatin1String("rgba(%1, %2, %3, %4)")).arg(
				color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
	}

	insertCssProperty(property, colorStr);
}

void MainWindow::slotAddFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, this);
	if (ok) {
		QString fontStr;
		if (font.bold()) {
			fontStr += "bold";
			fontStr += QLatin1Char(' ');
		}

		switch (font.style()) {
		case QFont::StyleItalic:
			fontStr += QLatin1String("italic ");
			break;
		case QFont::StyleOblique:
			fontStr += QLatin1String("oblique ");
			break;
		default:
			break;
		}
		fontStr += QString::number(font.pointSize());
		fontStr += QLatin1String("pt \"");
		fontStr += font.family();
		fontStr += QLatin1Char('"');

		insertCssProperty(QLatin1String("font"), fontStr);
		QString decoration;
		if (font.underline())
			decoration += QLatin1String("underline");
		if (font.strikeOut()) {
			if (!decoration.isEmpty())
				decoration += QLatin1Char(' ');
			decoration += QLatin1String("line-through");
		}
		insertCssProperty(QLatin1String("text-decoration"), decoration);
	}
}

void MainWindow::insertCssProperty(const QString &name, const QString &value)
{
	if (!value.isEmpty()) {
		QTextCursor cursor = ui->plainTextEditStyleSheet->textCursor();
		if (!name.isEmpty()) {
			cursor.beginEditBlock();
			cursor.removeSelectedText();
			cursor.movePosition(QTextCursor::EndOfLine);

			// Simple check to see if we're in a selector scope
			const QTextDocument *doc = ui->plainTextEditStyleSheet->document();
			const QTextCursor closing = doc->find(QLatin1String("}"), cursor, QTextDocument::FindBackward);
			const QTextCursor opening = doc->find(QLatin1String("{"), cursor, QTextDocument::FindBackward);
			const bool inSelector = !opening.isNull() && (closing.isNull() ||
														  closing.position() < opening.position());
			QString insertion;
			if (ui->plainTextEditStyleSheet->textCursor().block().length() != 1)
				insertion += QLatin1Char('\n');
			if (inSelector)
				insertion += QLatin1Char('\t');
			insertion += name;
			insertion += QLatin1String(": ");
			insertion += value;
			insertion += QLatin1Char(';');
			cursor.insertText(insertion);
			cursor.endEditBlock();
		} else {
			cursor.insertText(value);
		}
	}
}

void MainWindow::enableEditing(bool enable)
{
	ui->plainTextEditStyleSheet->setEnabled(enable);
	m_addImageAction->setEnabled(enable);
	m_addGradientAction->setEnabled(enable);
	m_addColorAction->setEnabled(enable);
	m_addFontAction->setEnabled(enable);
	ui->lineEditVersion->setEnabled(enable);
	ui->plainTextEditDescription->setEnabled(enable);
	ui->checkBoxMainIconVisible->setEnabled(enable);
	ui->spinBoxMainIconSize->setEnabled(enable);
}


void MainWindow::on_treeWidgetSelector_itemClicked(QTreeWidgetItem* item, int column)
{
	if (item->isSelected())
	{
		bool tempSaved = saved;
		currentSelectionText = item->text(column);
		if (item->text(column) == "Skin Properties")
		{
			ui->stackedWidget->setCurrentIndex(0);
			ui->plainTextEditStyleSheet->setPlainText("");
			ui->plainTextEditStyleSheet->setEnabled(false);
		} else if (!ui->lineEditName->text().isEmpty() && !ui->lineEditAuthor->text().isEmpty()) {
			ui->plainTextEditStyleSheet->setEnabled(true);
		}

		if (item->text(column) == "Splash Screen Background")
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashBackground);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == "Logo")
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashLogo);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == "Footer")
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashFooter);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == "Status Text")
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashStatus);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == "Progress")
		{
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.splashProgress);
			ui->stackedWidget->setCurrentIndex(1);
		}

		if (item->text(column) == "Top Left")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameTopLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Left")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Bottom Left")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameBottomLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Top")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameTopStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Icon Frame")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowIconFrameStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Window Text")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowTextStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Spacer Frame")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameTopSpacerStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Buttons Frame")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.titlebarButtonsFrameStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Minimize Button")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.minimizeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Maximize Button")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.maximizeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Close Button")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.closeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Bottom")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameBottomStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Top Right")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameTopRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Right")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Bottom Right")
		{
			ui->windowText->setText("Main Window");
			this->isMainWindow = true;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.windowFrameBottomRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Top Left")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameTopLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Left")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Bottom Left")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameBottomLeftStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Top")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameTopStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Icon Frame")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowIconFrameStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Window Text")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowTextStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Spacer Frame")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameTopSpacerStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Buttons Frame")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childTitlebarButtonsFrameStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Minimize Button")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childMinimizeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Maximize Button")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childMaximizeButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Close Button")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childCloseButtonStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Bottom")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameBottomStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Top Right")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameTopRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Right")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Child Bottom Right")
		{
			ui->windowText->setText("Child Window");
			this->isMainWindow = false;
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.childWindowFrameBottomRightStyleSheet);
			ui->stackedWidget->setCurrentIndex(2);
		}

		if (item->text(column) == "Standard Items")
		{
			ui->stackedWidget->setCurrentIndex(3);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.standardItems);
		}

		if (item->text(column) == "Sidebar Background")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.sidebarBackground);
		}

		if (item->text(column) == "Sidebar Task Header")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.sidebarTaskHeader);
		}

		if (item->text(column) == "Sidebar Task Body")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.sidebarTaskBackground);
		}

		if (item->text(column) == "Un-Clickable Sidebar Task Header")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.sidebarUnclickableTaskHeader);
		}

		if (item->text(column) == "Add Search Button")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.addSearchButton);
		}

		if (item->text(column) == "Chat Welcome")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.chatWelcome);
		}

		if (item->text(column) == "Chat Toolbar")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.chatToolbar);
		}

		if (item->text(column) == "Library Navigator")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.libraryNavigator);
		}

		if (item->text(column) == "Searches")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.tabSearches);
		}

		if (item->text(column) == "Toolbars")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.toolbars);
		}

		if (item->text(column) == "Media Toolbar")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.mediaToolbar);
		}

		if (item->text(column) == "Media Seek Slider")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.seekSlider);
		}

		if (item->text(column) == "Media Volume Slider")
		{
			ui->stackedWidget->setCurrentIndex(4);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.volumeSlider);
		}

		if (item->text(column) == "Toolbar")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.navigationToolbar);
		}

		if (item->text(column) == "Home")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.homeHeader);
		}

		if (item->text(column) == "Library")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.libraryHeader);
		}

		if (item->text(column) == "Media")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.mediaHeader);
		}

		if (item->text(column) == "Search")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.searchHeader);
		}

		if (item->text(column) == "Transfers")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.transfersHeader);
		}

		if (item->text(column) == "Security")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.securityHeader);
		}

		if (item->text(column) == "Network")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.networkHeader);
		}

		if (item->text(column) == "Chat")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.chatHeader);
		}

		if (item->text(column) == "Generic")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.genericHeader);
		}

		if (item->text(column) == "Dialog Header")
		{
			ui->stackedWidget->setCurrentIndex(5);
			ui->plainTextEditStyleSheet->setPlainText(skinSettings.dialogHeader);
		}
		qApp->processEvents();
		saved = tempSaved;
	}
}

void MainWindow::on_actionOpen_triggered()
{
	DialogOpenSkin *dlgOpenSkin = new DialogOpenSkin(this);
	bool ok;
	ok = dlgOpenSkin->exec();
	if (ok)
	{
		skinSettings.loadSkin(dlgOpenSkin->skinFile);
		ui->lineEditName->setText(skinSettings.skinName);
		ui->lineEditAuthor->setText(skinSettings.skinAuthor);
		ui->lineEditVersion->setText(skinSettings.skinVersion);
		ui->plainTextEditDescription->setPlainText(skinSettings.skinDescription);
		skinChangeEvent();
		this->setWindowTitle(skinSettings.skinName + ".qsf" + " - Quazaa Skin Tool");
		this->enableEditing(true);
		saved = true;
	}
	on_treeWidgetSelector_itemClicked(ui->treeWidgetSelector->currentItem(), 0);
}

void MainWindow::on_actionClose_triggered()
{
	if (!saved && !skinSettings.skinName.isEmpty())
	{
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Question, "Skin Not Saved", "The skin has not been saved. Would you like to save it now?", QMessageBox::Ok|QMessageBox::Cancel, this);
		bool ok = msgBox->exec();
		if (ok)
			on_actionSave_triggered();
	}
	this->enableEditing(false);

	this->setWindowTitle("Quazaa Skin Tool");
}

void MainWindow::on_actionSave_triggered()
{
	skinSettings.saveSkin(qApp->applicationDirPath() + "/Skin/" + skinSettings.skinName + "/" + skinSettings.skinName + ".qsk");
	saved = true;
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::on_actionPackage_For_Distribution_triggered()
{

}

void MainWindow::on_actionCut_triggered()
{

}

void MainWindow::on_actionCopy_triggered()
{

}

void MainWindow::on_actionPaste_triggered()
{

}

void MainWindow::on_actionSkin_Creation_Guide_triggered()
{

}

void MainWindow::on_actionAbout_Quazaa_Skin_Tool_triggered()
{

}

void MainWindow::skinChangeEvent()
{
	ui->frameSplashBackground->setStyleSheet(skinSettings.splashBackground);
	ui->labelSplashlLogo->setStyleSheet(skinSettings.splashLogo);
	ui->frameSplashFooter->setStyleSheet(skinSettings.splashFooter);
	ui->labelSplashStatus->setStyleSheet(skinSettings.splashStatus);
	ui->progressBarSplashStatus->setStyleSheet(skinSettings.splashProgress);
	updateWindowStyleSheet(isMainWindow);
	ui->pageStandardItems->setStyleSheet(skinSettings.standardItems);
	ui->scrollAreaSidebar->setStyleSheet(skinSettings.sidebarBackground);
	ui->toolButtonSidebarTaskHeader->setStyleSheet(skinSettings.sidebarTaskHeader);
	ui->frameSidebarTask->setStyleSheet(skinSettings.sidebarTaskBackground);
	ui->toolButtonUnclickableSidebarTaskHeader->setStyleSheet(skinSettings.sidebarUnclickableTaskHeader);
	ui->toolButtonNewSearch->setStyleSheet(skinSettings.addSearchButton);
	ui->frameChatWelcome->setStyleSheet(skinSettings.chatWelcome);
	ui->toolFrameChat->setStyleSheet(skinSettings.chatToolbar);
	ui->tabWidgetLibraryNavigator->setStyleSheet(skinSettings.libraryNavigator);
	ui->frameSearches->setStyleSheet(skinSettings.tabSearches);
	ui->toolFrame->setStyleSheet(skinSettings.toolbars);
	ui->toolFrameMedia->setStyleSheet(skinSettings.mediaToolbar);
	ui->seekSlider->setStyleSheet(skinSettings.seekSlider);
	ui->volumeSlider->setStyleSheet(skinSettings.volumeSlider);
	ui->toolBarNavigation->setStyleSheet(skinSettings.navigationToolbar);
	ui->frameHomeHeader->setStyleSheet(skinSettings.homeHeader);
	ui->frameLibraryHeader->setStyleSheet(skinSettings.libraryHeader);
	ui->frameMediaHeader->setStyleSheet(skinSettings.mediaHeader);
	ui->frameSearchHeader->setStyleSheet(skinSettings.searchHeader);
	ui->frameTransfersHeader->setStyleSheet(skinSettings.transfersHeader);
	ui->frameSecurityHeader->setStyleSheet(skinSettings.securityHeader);
	ui->frameNetworkHeader->setStyleSheet(skinSettings.networkHeader);
	ui->frameChatHeader->setStyleSheet(skinSettings.chatHeader);
	ui->frameGenericHeader->setStyleSheet(skinSettings.genericHeader);
	ui->frameDialogHeader->setStyleSheet(skinSettings.dialogHeader);
}

void MainWindow::updateWindowStyleSheet(bool mainWindow)
{
	if (mainWindow)
	{
		ui->windowFrameTopLeft->setStyleSheet(skinSettings.windowFrameTopLeftStyleSheet);
		ui->windowFrameLeft->setStyleSheet(skinSettings.windowFrameLeftStyleSheet);
		ui->windowFrameBottomLeft->setStyleSheet(skinSettings.windowFrameBottomLeftStyleSheet);
		ui->windowFrameTop->setStyleSheet(skinSettings.windowFrameTopStyleSheet);
		ui->windowIconFrame->setStyleSheet(skinSettings.windowIconFrameStyleSheet);
		ui->checkBoxMainIconVisible->setChecked(skinSettings.windowIconVisible);
		ui->windowIcon->setVisible(skinSettings.windowIconVisible);
		if (skinSettings.windowIconSize.isValid())
		{
			ui->spinBoxMainIconSize->setValue(skinSettings.windowIconSize.height());
			ui->windowIcon->setIconSize(skinSettings.windowIconSize);
			ui->windowIcon->setIconSize(skinSettings.windowIconSize);
		}
		ui->windowText->setStyleSheet(skinSettings.windowTextStyleSheet);
		ui->windowFrameTopSpacer->setStyleSheet(skinSettings.windowFrameTopSpacerStyleSheet);
		ui->titlebarButtonsFrame->setStyleSheet(skinSettings.titlebarButtonsFrameStyleSheet);
		ui->minimizeButton->setStyleSheet(skinSettings.minimizeButtonStyleSheet);
		ui->maximizeButton->setStyleSheet(skinSettings.maximizeButtonStyleSheet);
		ui->closeButton->setStyleSheet(skinSettings.closeButtonStyleSheet);
		ui->windowFrameBottom->setStyleSheet(skinSettings.windowFrameBottomStyleSheet);
		ui->windowFrameTopRight->setStyleSheet(skinSettings.windowFrameTopRightStyleSheet);
		ui->windowFrameRight->setStyleSheet(skinSettings.windowFrameRightStyleSheet);
		ui->windowFrameBottomRight->setStyleSheet(skinSettings.windowFrameBottomRightStyleSheet);
	} else {
		ui->windowFrameTopLeft->setStyleSheet(skinSettings.childWindowFrameTopLeftStyleSheet);
		ui->windowFrameLeft->setStyleSheet(skinSettings.childWindowFrameLeftStyleSheet);
		ui->windowFrameBottomLeft->setStyleSheet(skinSettings.childWindowFrameBottomLeftStyleSheet);
		ui->windowFrameTop->setStyleSheet(skinSettings.childWindowFrameTopStyleSheet);
		ui->windowIconFrame->setStyleSheet(skinSettings.childWindowIconFrameStyleSheet);
		ui->checkBoxMainIconVisible->setChecked(skinSettings.childWindowIconVisible);
		ui->windowIcon->setVisible(skinSettings.childWindowIconVisible);
		if (skinSettings.childWindowIconSize.isValid())
		{
			ui->spinBoxMainIconSize->setValue(skinSettings.childWindowIconSize.height());
			ui->windowIcon->setIconSize(skinSettings.childWindowIconSize);
			ui->windowIcon->setIconSize(skinSettings.childWindowIconSize);
		}
		ui->windowText->setStyleSheet(skinSettings.childWindowTextStyleSheet);
		ui->windowFrameTopSpacer->setStyleSheet(skinSettings.childWindowFrameTopSpacerStyleSheet);
		ui->titlebarButtonsFrame->setStyleSheet(skinSettings.childTitlebarButtonsFrameStyleSheet);
		ui->minimizeButton->setStyleSheet(skinSettings.childMinimizeButtonStyleSheet);
		ui->maximizeButton->setStyleSheet(skinSettings.childMaximizeButtonStyleSheet);
		ui->closeButton->setStyleSheet(skinSettings.childCloseButtonStyleSheet);
		ui->windowFrameBottom->setStyleSheet(skinSettings.childWindowFrameBottomStyleSheet);
		ui->windowFrameTopRight->setStyleSheet(skinSettings.childWindowFrameTopRightStyleSheet);
		ui->windowFrameRight->setStyleSheet(skinSettings.childWindowFrameRightStyleSheet);
		ui->windowFrameBottomRight->setStyleSheet(skinSettings.childWindowFrameBottomRightStyleSheet);
	}
}

void MainWindow::applySheets()
{
	if (currentSelectionText == "Splash Screen Background")
	{
		skinSettings.splashBackground = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Logo")
	{
		skinSettings.splashLogo = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Footer")
	{
		skinSettings.splashFooter = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Status Text")
	{
		skinSettings.splashStatus = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Progress")
	{
		skinSettings.splashProgress = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Top Left")
	{
		skinSettings.windowFrameTopLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Left")
	{
		skinSettings.windowFrameLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Bottom Left")
	{
		skinSettings.windowFrameBottomLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Top")
	{
		skinSettings.windowFrameTopStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Icon Frame")
	{
		skinSettings.windowIconFrameStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Window Text")
	{
		skinSettings.windowTextStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Spacer Frame")
	{
		skinSettings.windowFrameTopSpacerStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Buttons Frame")
	{
		skinSettings.titlebarButtonsFrameStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Minimize Button")
	{
		skinSettings.minimizeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Maximize Button")
	{
		skinSettings.maximizeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Close Button")
	{
		skinSettings.closeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Bottom")
	{
		skinSettings.windowFrameBottomStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Top Right")
	{
		skinSettings.windowFrameTopRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Right")
	{
		skinSettings.windowFrameRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Bottom Right")
	{
		skinSettings.windowFrameBottomRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Top Left")
	{
		skinSettings.childWindowFrameTopLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Left")
	{
		skinSettings.childWindowFrameLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Bottom Left")
	{
		skinSettings.childWindowFrameBottomLeftStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Top")
	{
		skinSettings.childWindowFrameTopStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Icon Frame")
	{
		skinSettings.childWindowIconFrameStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Window Text")
	{
		skinSettings.childWindowTextStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Spacer Frame")
	{
		skinSettings.childWindowFrameTopSpacerStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Buttons Frame")
	{
		skinSettings.childTitlebarButtonsFrameStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Minimize Button")
	{
		skinSettings.childMinimizeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Maximize Button")
	{
		skinSettings.childMaximizeButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Close Button")
	{
		skinSettings.childCloseButtonStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Bottom")
	{
		skinSettings.childWindowFrameBottomStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Top Right")
	{
		skinSettings.childWindowFrameTopRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Right")
	{
		skinSettings.childWindowFrameRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Child Bottom Right")
	{
		skinSettings.childWindowFrameBottomRightStyleSheet = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Standard Items")
	{
		skinSettings.standardItems = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Sidebar Background")
	{
		skinSettings.sidebarBackground = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Sidebar Task Header")
	{
		skinSettings.sidebarTaskHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Sidebar Task Body")
	{
		skinSettings.sidebarTaskBackground = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Un-Clickable Sidebar Task Header")
	{
		skinSettings.sidebarUnclickableTaskHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Add Search Button")
	{
		skinSettings.addSearchButton = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Chat Welcome")
	{
		skinSettings.chatWelcome = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Chat Toolbar")
	{
		skinSettings.chatToolbar = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Library Navigator")
	{
		skinSettings.libraryNavigator = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Searches")
	{
		skinSettings.tabSearches = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Toolbars")
	{
		skinSettings.toolbars = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Media Toolbar")
	{
		skinSettings.mediaToolbar = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Media Seek Slider")
	{
		skinSettings.seekSlider = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Media Volume Slider")
	{
		skinSettings.volumeSlider = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Toolbar")
	{
		skinSettings.navigationToolbar = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Home")
	{
		skinSettings.homeHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Library")
	{
		skinSettings.libraryHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Media")
	{
		skinSettings.mediaHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Search")
	{
		skinSettings.searchHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Transfers")
	{
		skinSettings.transfersHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Security")
	{
		skinSettings.securityHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Network")
	{
		skinSettings.networkHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Chat")
	{
		skinSettings.chatHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Generic")
	{
		skinSettings.genericHeader = ui->plainTextEditStyleSheet->toPlainText();
	}

	if (currentSelectionText == "Dialog Header")
	{
		skinSettings.dialogHeader = ui->plainTextEditStyleSheet->toPlainText();
	}
	skinChangeEvent();
}

void MainWindow::on_plainTextEditStyleSheet_textChanged()
{
	if (isStyleSheetValid(ui->plainTextEditStyleSheet->styleSheet()))
	{
		saved = false;
		applySheets();
	}
}

void MainWindow::on_checkBoxMainIconVisible_toggled(bool checked)
{
	saved = false;
	applyIcon();
}

void MainWindow::on_spinBoxMainIconSize_valueChanged(int value)
{
	saved = false;
	applyIcon();
}

void MainWindow::on_lineEditVersion_textChanged(QString text)
{
	skinSettings.skinVersion = ui->lineEditVersion->text();
	saved = false;
}

void MainWindow::on_plainTextEditDescription_textChanged()
{
	skinSettings.skinDescription = ui->plainTextEditDescription->toPlainText();
	saved = false;
}

void MainWindow::applyIcon()
{
	if (isMainWindow)
	{
		skinSettings.windowIconVisible = ui->checkBoxMainIconVisible->isChecked();
		if (ui->spinBoxMainIconSize->value() >= 15)
			skinSettings.windowIconSize = QSize(int(ui->spinBoxMainIconSize->value()), ui->spinBoxMainIconSize->value());
	} else {
		skinSettings.childWindowIconVisible = ui->checkBoxMainIconVisible->isChecked();
		if (ui->spinBoxMainIconSize->value() >= 15)
			skinSettings.childWindowIconSize = QSize(ui->spinBoxMainIconSize->value(), ui->spinBoxMainIconSize->value());
	}
	skinChangeEvent();
}
