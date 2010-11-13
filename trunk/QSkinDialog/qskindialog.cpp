/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: qskindialog.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : The core of QSkinDialog, a library allowing skinning in Qt through
 *				  style sheets. It is written for reuse in multiple programs.
 ********************************************************************************************************/

#include "qskindialog.h"
#include "ui_qskindialog.h"
#include "qskinsettings.h"
#include <quazaasettings.h>

#include "systemlog.h"

QSkinDialog::QSkinDialog(bool sizable, bool closable, bool mainDialog, bool preview, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::QSkinDialog)
{
	ui->setupUi(this);
	maximized = false;
	minimized = false;
	movable = false;
	moving = false;
	dialogSizable = sizable;
	dialogClosable = closable;
	dialogPreview = preview;
	isMainDialog = mainDialog;
	sizableTopLeft = false;
	sizableLeft = false;
	sizableBottomLeft = false;
	sizableBottom = false;
	sizableTopRight = false;
	sizableRight = false;
	sizableBottomRight = false;

	systemMenu = new QMenu(this);
	systemRestoreAction = new QAction(tr("Restore"), this);
	systemRestoreAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
	systemRestoreAction->setEnabled(false);
	systemMenu->addAction(systemRestoreAction);
	systemMinimizeAction = new QAction(tr("Minimize"), this);
	systemMinimizeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
	systemMenu->addAction(systemMinimizeAction);
	systemMinimizeAction->setEnabled(dialogSizable);
	systemMaximizeAction = new QAction(tr("Maximize"), this);
	systemMaximizeAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
	systemMenu->addAction(systemMaximizeAction);
	systemMaximizeAction->setEnabled(dialogSizable);
	systemMenu->addSeparator();
	systemCloseAction = new QAction(tr("Close"), this);
	systemCloseAction->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	systemCloseAction->setShortcut(QKeySequence::Close);
	systemCloseAction->setEnabled(dialogClosable);
	systemMenu->addAction(systemCloseAction);

	// We are using our own frame of course instead of the system frame
	if(!parent == 0)
	{
		this->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
	}
	else
	{
		this->setWindowFlags(Qt::FramelessWindowHint);
	}
	// To enable alpha blending and transparency in the frame
	setAttribute(Qt::WA_TranslucentBackground);

	this->setMaximumSize(QApplication::desktop()->availableGeometry(this).width(), QApplication::desktop()->availableGeometry(this).height());

	ui->minimizeButton->setEnabled(dialogSizable);
	ui->maximizeButton->setEnabled(dialogSizable);
	ui->closeButton->setEnabled(dialogClosable);
	if(!dialogSizable)
	{
		ui->windowFrameTopLeft->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameLeft->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameBottomLeft->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameTop->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameBottom->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameTopRight->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameRight->setCursor(QCursor(Qt::ArrowCursor));
		ui->windowFrameBottomRight->setCursor(QCursor(Qt::ArrowCursor));
	}

	connect(systemRestoreAction, SIGNAL(triggered()), this, SLOT(onMaximize()));
	connect(systemMaximizeAction, SIGNAL(triggered()), this, SLOT(onMaximize()));
	connect(systemMinimizeAction, SIGNAL(triggered()), this, SLOT(onMinimize()));
	connect(systemCloseAction, SIGNAL(triggered()), this, SLOT(onClose()));
	connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(onClose()));
	connect(ui->minimizeButton, SIGNAL(clicked()), this, SLOT(onMinimize()));
	connect(ui->maximizeButton, SIGNAL(clicked()), this, SLOT(onMaximize()));
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));

	if(quazaaSettings.Skin.File.isEmpty())
	{
		quazaaSettings.loadSkinSettings();
	}

	skinSettings.loadSkin(quazaaSettings.Skin.File);
	quazaaSettings.saveSkinSettings();

	// Load the previously set skin

	if(!dialogPreview)
	{
		if(isMainDialog)
		{
			ui->windowFrameTopLeft->setStyleSheet(skinSettings.windowFrameTopLeftStyleSheet);
			ui->windowFrameLeft->setStyleSheet(skinSettings.windowFrameLeftStyleSheet);
			ui->windowFrameBottomLeft->setStyleSheet(skinSettings.windowFrameBottomLeftStyleSheet);
			ui->windowFrameTop->setStyleSheet(skinSettings.windowFrameTopStyleSheet);
			ui->windowFrameBottom->setStyleSheet(skinSettings.windowFrameBottomStyleSheet);
			ui->windowFrameTopRight->setStyleSheet(skinSettings.windowFrameTopRightStyleSheet);
			ui->windowFrameRight->setStyleSheet(skinSettings.windowFrameRightStyleSheet);
			ui->windowFrameBottomRight->setStyleSheet(skinSettings.windowFrameBottomRightStyleSheet);
			ui->titlebarButtonsFrame->setStyleSheet(skinSettings.titlebarButtonsFrameStyleSheet);
			ui->minimizeButton->setStyleSheet(skinSettings.minimizeButtonStyleSheet);
			ui->maximizeButton->setStyleSheet(skinSettings.maximizeButtonStyleSheet);
			ui->closeButton->setStyleSheet(skinSettings.closeButtonStyleSheet);
			ui->windowFrameTopSpacer->setStyleSheet(skinSettings.windowFrameTopSpacerStyleSheet);
			ui->windowText->setStyleSheet(skinSettings.windowTextStyleSheet);
			ui->windowIconFrame->setStyleSheet(skinSettings.windowIconFrameStyleSheet);
			ui->windowIcon->setVisible(skinSettings.windowIconVisible);
			quazaaSettings.loadSkinWindowSettings(this);
		}
		else
		{
			ui->windowFrameTopLeft->setStyleSheet(skinSettings.childWindowFrameTopLeftStyleSheet);
			ui->windowFrameLeft->setStyleSheet(skinSettings.childWindowFrameLeftStyleSheet);
			ui->windowFrameBottomLeft->setStyleSheet(skinSettings.childWindowFrameBottomLeftStyleSheet);
			ui->windowFrameTop->setStyleSheet(skinSettings.childWindowFrameTopStyleSheet);
			ui->windowFrameBottom->setStyleSheet(skinSettings.childWindowFrameBottomStyleSheet);
			ui->windowFrameTopRight->setStyleSheet(skinSettings.childWindowFrameTopRightStyleSheet);
			ui->windowFrameRight->setStyleSheet(skinSettings.childWindowFrameRightStyleSheet);
			ui->windowFrameBottomRight->setStyleSheet(skinSettings.childWindowFrameBottomRightStyleSheet);
			ui->titlebarButtonsFrame->setStyleSheet(skinSettings.childTitlebarButtonsFrameStyleSheet);
			ui->minimizeButton->setStyleSheet(skinSettings.childMinimizeButtonStyleSheet);
			ui->maximizeButton->setStyleSheet(skinSettings.childMaximizeButtonStyleSheet);
			ui->closeButton->setStyleSheet(skinSettings.childCloseButtonStyleSheet);
			ui->windowFrameTopSpacer->setStyleSheet(skinSettings.childWindowFrameTopSpacerStyleSheet);
			ui->windowText->setStyleSheet(skinSettings.childWindowTextStyleSheet);
			ui->windowIconFrame->setStyleSheet(skinSettings.childWindowIconFrameStyleSheet);
			ui->windowIcon->setVisible(skinSettings.childWindowIconVisible);
		}
	}
}

QSkinDialog::~QSkinDialog()
{
	delete ui;
}

void QSkinDialog::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
		{
			QIcon m_windowIcon = windowIcon();
			QString m_windowTitle = windowTitle();
			ui->retranslateUi(this);
			setWindowIcon(m_windowIcon);
			setWindowTitle(m_windowTitle);
			break;
		}
		case QEvent::ActivationChange:
			if(minimized && qApp->activeWindow())
			{
				setMinimized(false);
			}
			repaint();
			break;
		default:
			break;
	}
}

void QSkinDialog::onClose()
{
	if(isMainDialog)
	{
		quazaaSettings.saveSkinWindowSettings(this);
		emit mainClose();
	}
	else
	{
		close();
	}
}

void QSkinDialog::onMaximize()
{
	setMaximized(!maximized);
}

// Custom maximize event because showMaximized resizes the window to the entire desktop (including
// over panels and the taskbar) when FramelessWindowHint is used.
void QSkinDialog::setMaximized(bool maximize)
{
	ui->maximizeButton->setChecked(maximize);
	if(maximize)
	{
		normalGeometry = this->geometry();
		setGeometry(0, 0, QApplication::desktop()->availableGeometry(this).width(), QApplication::desktop()->availableGeometry(this).height());
		maximized = true;
		ui->maximizeButton->setToolTip(tr("Restore Down"));
		systemRestoreAction->setEnabled(true);
		systemMaximizeAction->setEnabled(false);
		ui->windowFrameTopLeft->setVisible(false);
		ui->windowFrameLeft->setVisible(false);
		ui->windowFrameBottomLeft->setVisible(false);
		ui->windowFrameBottom->setVisible(false);
		ui->windowFrameTopRight->setVisible(false);
		ui->windowFrameRight->setVisible(false);
		ui->windowFrameBottomRight->setVisible(false);
	}
	else
	{
		this->setGeometry(normalGeometry);
		maximized = false;
		ui->maximizeButton->setToolTip(tr("Maximize"));
		systemRestoreAction->setEnabled(false);
		systemMaximizeAction->setEnabled(true);
		ui->windowFrameTopLeft->setVisible(true);
		ui->windowFrameLeft->setVisible(true);
		ui->windowFrameBottomLeft->setVisible(true);
		ui->windowFrameBottom->setVisible(true);
		ui->windowFrameTopRight->setVisible(true);
		ui->windowFrameRight->setVisible(true);
		ui->windowFrameBottomRight->setVisible(true);
	}
}

void QSkinDialog::onMinimize()
{
	if(isMainDialog && quazaaSettings.System.MinimizeToTray)
	{
		hide();
	}
	else
	{
		setMinimized(!minimized);
	}
}

void QSkinDialog::setMinimized(bool minimize)
{
	if(minimize)
	{
		if(!maximized)
		{
			normalGeometry = this->geometry();
		}

		setGeometry(QApplication::desktop()->availableGeometry(this).width(), QApplication::desktop()->availableGeometry(this).height(), 0, 0);
		minimized = true;
	}
	else
	{
		if(maximized)
		{
			setGeometry(0, 0, QApplication::desktop()->availableGeometry(this).width(), QApplication::desktop()->availableGeometry(this).height());
		}
		else
		{
			this->setGeometry(normalGeometry);
		}
		minimized = false;
		activateWindow();
	}
}

void QSkinDialog::restore()
{
	if(minimized)
	{
		setMinimized(false);
		emit needToShow();
	}
	else if(isHidden())
	{
		show();
	}
	else
	{
		emit needToShow();
	}
}

// These are checks to make sure erroneous move and resize events aren't triggered when the mouse is moved
// over the window frame with the left mouse button held down. Mime drags from another app for example.
void QSkinDialog::mousePressEvent(QMouseEvent* e)
{
	switch(e->button())
	{
		case Qt::LeftButton:
			if((ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
			        && !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && !maximized)
			{
				dragPosition = e->globalPos() - frameGeometry().topLeft();
				movable = true;
				e->accept();
			}
			else if(ui->windowFrameTopLeft->underMouse() && dialogSizable)
			{
				dragPosition = e->globalPos() - frameGeometry().topLeft();
				sizableTopLeft = true;
				e->accept();
			}
			else if(ui->windowFrameLeft->underMouse() && dialogSizable)
			{
				iDragPosition = e->globalPos().x() - frameGeometry().left();
				this->sizableLeft = true;
				e->accept();
			}
			else if(ui->windowFrameBottomLeft->underMouse() && dialogSizable)
			{
				dragPosition = e->globalPos() - frameGeometry().bottomLeft();
				sizableBottomLeft = true;
				e->accept();
			}
			else if(ui->windowFrameBottom->underMouse() && dialogSizable)
			{
				iDragPosition = e->globalPos().y() - frameGeometry().bottom();
				sizableBottom = true;
				e->accept();
			}
			else if(ui->windowFrameTopRight->underMouse() && dialogSizable)
			{
				dragPosition = e->globalPos() - frameGeometry().topRight();
				sizableTopRight = true;
				e->accept();
			}
			else if(ui->windowFrameRight->underMouse() && dialogSizable)
			{
				iDragPosition = e->globalPos().x() - frameGeometry().right();
				sizableRight = true;
				e->accept();
			}
			else if(ui->windowFrameBottomRight->underMouse() && dialogSizable)
			{
				dragPosition = e->globalPos() - frameGeometry().bottomRight();
				sizableBottomRight = true;
				e->accept();
			}
			else
			{
				movable = false;
				sizableTopLeft = false;
				sizableLeft = false;
				sizableBottomLeft = false;
				sizableBottom = false;
				sizableTopRight = false;
				sizableRight = false;
				sizableBottomRight = false;
				e->accept();
			}
			break;
		default:
			e->ignore();
			break;
	}
}

// All your drag move and resize events on the window frame
void QSkinDialog::mouseMoveEvent(QMouseEvent* e)
{
	if((e->buttons() & Qt::LeftButton) && (ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
	        && !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && movable && !maximized)
	{
		moving = true;
		move(e->globalPos() - dragPosition);
		normalGeometry = this->geometry();
		e->accept();
	}
	else if(ui->windowFrameTopLeft->underMouse() && sizableTopLeft && dialogSizable)
	{
		QRect newSize = geometry();
		newSize.setTopLeft(e->globalPos() - dragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	}
	else if(ui->windowFrameLeft->underMouse() && sizableLeft && dialogSizable)
	{
		QRect newSize = geometry();
		newSize.setLeft(e->globalPos().x() - iDragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	}
	else if(ui->windowFrameBottomLeft->underMouse() && sizableBottomLeft && dialogSizable)
	{
		QRect newSize = geometry();
		newSize.setBottomLeft(e->globalPos() - dragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	}
	else if(ui->windowFrameBottom->underMouse() && sizableBottom && dialogSizable)
	{
		QRect newSize = geometry();
		newSize.setBottom(e->globalPos().y() - iDragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	}
	else if(ui->windowFrameTopRight->underMouse() && sizableTopRight && dialogSizable)
	{
		QRect newSize = geometry();
		newSize.setTopRight(e->globalPos() - dragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	}
	else if(ui->windowFrameRight->underMouse() && sizableRight && dialogSizable)
	{
		QRect newSize = geometry();
		newSize.setRight(e->globalPos().x() - iDragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	}
	else if(ui->windowFrameBottomRight->underMouse() && sizableBottomRight && dialogSizable)
	{
		QRect newSize = geometry();
		newSize.setBottomRight(e->globalPos() - dragPosition);
		setGeometry(newSize);
		normalGeometry = newSize;
		e->accept();
	}
}

void QSkinDialog::mouseReleaseEvent(QMouseEvent* e)
{
	// "Window Snapping" Windows 7 style
	if(moving && dialogSizable)
	{
		if(e->globalPos().y() < (QApplication::desktop()->availableGeometry(this).top() + 5))
		{
			setMaximized(true);
		}
		if(e->globalPos().x() < (QApplication::desktop()->screenGeometry(this).left() + 5))
		{
			setGeometry(0, 0, QApplication::desktop()->availableGeometry(this).width() / 2, QApplication::desktop()->availableGeometry(this).height());
		}
		if(e->globalPos().x() > (QApplication::desktop()->availableGeometry(this).right() - 5))
		{
			setGeometry(QApplication::desktop()->availableGeometry(this).right() / 2, 0, QApplication::desktop()->availableGeometry(this).right() / 2, QApplication::desktop()->availableGeometry(this).height());
			move(QApplication::desktop()->availableGeometry(this).right() - (geometry().width() - 1), 0);
		}
	}
	moving = false;
}

void QSkinDialog::mouseDoubleClickEvent(QMouseEvent* e)
{
	if((e->button() == Qt::LeftButton) && (ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
	        && !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && dialogSizable)
	{
		onMaximize();
	}
}

void QSkinDialog::on_windowText_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	systemMenu->exec(QCursor::pos());
}

void QSkinDialog::on_windowFrameTop_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	systemMenu->exec(QCursor::pos());
}

void QSkinDialog::addChildWidget(QWidget* parent)
{
	// Unset the frameless window hint for our child widget or it won't display at runtime
	// In this example it is set in the ui files
	parent->setWindowFlags(ui->widgetContents->windowFlags() & ~Qt::FramelessWindowHint);
	ui->layoutContents->addWidget(parent);
	setWindowTitle(parent->windowTitle());
	setWindowIcon(parent->windowIcon());
	qApp->processEvents();
	if(!isMainDialog)
	{
		move(QPoint(((QApplication::desktop()->screenGeometry(this).width() / 2) - (parent->width() / 2)), ((QApplication::desktop()->screenGeometry(this).height() / 2) - (parent->height() / 2))));
	}
}

void QSkinDialog::setWindowTitle(const QString& title)
{
	ui->windowText->setText(title);
	return QDialog::setWindowTitle(title);
}

void QSkinDialog::setWindowIcon(const QIcon& icon)
{
	QIcon windowIcon = icon;
	if(windowIcon.isNull())
	{
		windowIcon = QIcon(":/Resource/qtlogo-64.png");
	}

	ui->windowIcon->setIcon(windowIcon);
	return QDialog::setWindowIcon(windowIcon);
}

void QSkinDialog::on_windowIcon_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	// This is where your custom right click menu would go for the window icon
	systemMenu->exec(ui->windowIcon->mapToGlobal(ui->windowIcon->geometry().bottomLeft()));
}

void QSkinDialog::on_windowIcon_clicked()
{
	systemMenu->exec(ui->windowIcon->mapToGlobal(ui->windowIcon->geometry().bottomLeft()));
}


// Catches the skinChanged() signal from qskinsettings and sould change skins
// in all skinned dialogs
void QSkinDialog::skinChangeEvent()
{
	if(!dialogPreview)
	{
		if(isMainDialog)
		{
			ui->windowFrameTopLeft->setStyleSheet(skinSettings.windowFrameTopLeftStyleSheet);
			ui->windowFrameLeft->setStyleSheet(skinSettings.windowFrameLeftStyleSheet);
			ui->windowFrameBottomLeft->setStyleSheet(skinSettings.windowFrameBottomLeftStyleSheet);
			ui->windowFrameTop->setStyleSheet(skinSettings.windowFrameTopStyleSheet);
			ui->windowFrameBottom->setStyleSheet(skinSettings.windowFrameBottomStyleSheet);
			ui->windowFrameTopRight->setStyleSheet(skinSettings.windowFrameTopRightStyleSheet);
			ui->windowFrameRight->setStyleSheet(skinSettings.windowFrameRightStyleSheet);
			ui->windowFrameBottomRight->setStyleSheet(skinSettings.windowFrameBottomRightStyleSheet);
			ui->titlebarButtonsFrame->setStyleSheet(skinSettings.titlebarButtonsFrameStyleSheet);
			ui->minimizeButton->setStyleSheet(skinSettings.minimizeButtonStyleSheet);
			ui->maximizeButton->setStyleSheet(skinSettings.maximizeButtonStyleSheet);
			ui->closeButton->setStyleSheet(skinSettings.closeButtonStyleSheet);
			ui->windowFrameTopSpacer->setStyleSheet(skinSettings.windowFrameTopSpacerStyleSheet);
			ui->windowText->setStyleSheet(skinSettings.windowTextStyleSheet);
			ui->windowIconFrame->setStyleSheet(skinSettings.windowIconFrameStyleSheet);
			ui->windowIcon->setVisible(skinSettings.windowIconVisible);
		}
		else
		{
			ui->windowFrameTopLeft->setStyleSheet(skinSettings.childWindowFrameTopLeftStyleSheet);
			ui->windowFrameLeft->setStyleSheet(skinSettings.childWindowFrameLeftStyleSheet);
			ui->windowFrameBottomLeft->setStyleSheet(skinSettings.childWindowFrameBottomLeftStyleSheet);
			ui->windowFrameTop->setStyleSheet(skinSettings.childWindowFrameTopStyleSheet);
			ui->windowFrameBottom->setStyleSheet(skinSettings.childWindowFrameBottomStyleSheet);
			ui->windowFrameTopRight->setStyleSheet(skinSettings.childWindowFrameTopRightStyleSheet);
			ui->windowFrameRight->setStyleSheet(skinSettings.childWindowFrameRightStyleSheet);
			ui->windowFrameBottomRight->setStyleSheet(skinSettings.childWindowFrameBottomRightStyleSheet);
			ui->titlebarButtonsFrame->setStyleSheet(skinSettings.childTitlebarButtonsFrameStyleSheet);
			ui->minimizeButton->setStyleSheet(skinSettings.childMinimizeButtonStyleSheet);
			ui->maximizeButton->setStyleSheet(skinSettings.childMaximizeButtonStyleSheet);
			ui->closeButton->setStyleSheet(skinSettings.childCloseButtonStyleSheet);
			ui->windowFrameTopSpacer->setStyleSheet(skinSettings.childWindowFrameTopSpacerStyleSheet);
			ui->windowText->setStyleSheet(skinSettings.childWindowTextStyleSheet);
			ui->windowIconFrame->setStyleSheet(skinSettings.childWindowIconFrameStyleSheet);
			ui->windowIcon->setVisible(skinSettings.childWindowIconVisible);
		}
	}
}

void QSkinDialog::on_windowIconFrame_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	systemMenu->exec(QCursor::pos());
}

void QSkinDialog::on_titlebarButtonsFrame_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	systemMenu->exec(QCursor::pos());
}

void QSkinDialog::on_windowFrameTopSpacer_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	systemMenu->exec(QCursor::pos());
}

void QSkinDialog::loadPreviewSkin(QString file)
{
	QSettings reader(file, QSettings::IniFormat);
	ui->windowFrameTopLeft->setStyleSheet(reader.value("windowFrameTopLeftStyleSheet", "border-image: url(:/Resource/frameTopLeft.png);").toString());
	ui->windowFrameLeft->setStyleSheet(reader.value("windowFrameLeftStyleSheet", "border-image: url(:/Resource/frameLeft.png); border-left: 1; border-top: 10;").toString());
	ui->windowFrameBottomLeft->setStyleSheet(reader.value("windowFrameBottomLeftStyleSheet", "border-image: url(:/Resource/frameBottomLeft.png);").toString());
	ui->windowFrameTop->setStyleSheet(reader.value("windowFrameTopStyleSheet", "").toString());
	ui->windowFrameBottom->setStyleSheet(reader.value("windowFrameBottomStyleSheet", "border-image: url(:/Resource/frameBottom.png); border-bottom: 1;").toString());
	ui->windowFrameTopRight->setStyleSheet(reader.value("windowFrameTopRightStyleSheet", "border-image: url(:/Resource/frameTopRight.png);").toString());
	ui->windowFrameRight->setStyleSheet(reader.value("windowFrameRightStyleSheet", "QFrame { border-image: url(:/Resource/frameRight.png); border-right: 1; border-top: 10; }").toString());
	ui->windowFrameBottomRight->setStyleSheet(reader.value("windowFrameBottomRightStyleSheet", "border-image: url(:/Resource/frameBottomRight.png);").toString());
	ui->titlebarButtonsFrame->setStyleSheet(reader.value("titlebarButtonsFrameStyleSheet", "QFrame#titlebarButtonsFrame { padding-top: -1; padding-bottom: 10; }").toString());
	ui->minimizeButton->setStyleSheet(reader.value("minimizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/minButton.png); } QToolButton:hover { border-image: url(:/Resource/minButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/minButtonD.png); }").toString());
	ui->maximizeButton->setStyleSheet(reader.value("maximizeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/maxButton.png); } QToolButton:hover { border-image: url(:/Resource/maxButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/maxButtonD.png); } QToolButton:checked { border-image: url(:/Resource/restoreButton.png); } QToolButton:checked:hover { border-image: url(:/Resource/restoreButtonH.png); } QToolButton:checked:disabled { border-image: url(:/Resource/restoreButtonD.png); }").toString());
	ui->closeButton->setStyleSheet(reader.value("closeButtonStyleSheet", "QToolButton { border: 0px solid transparent; border-image: url(:/Resource/quitButton.png); } QToolButton:hover { border-image: url(:/Resource/quitButtonH.png); } QToolButton:disabled { border-image: url(:/Resource/quitButtonD.png); }").toString());
	ui->windowFrameTopSpacer->setStyleSheet(reader.value("windowFrameTopSpacerStyleSheet", "QFrame#windowFrameTopSpacer {\n	border-image: url(:/Resource/frameTop.png);\n}").toString());
	ui->windowText->setStyleSheet(reader.value("windowTextStyleSheet", "border-image: url(:/Resource/windowTextBackground.png);\npadding-left: -2px;\npadding-right: -2px;\npadding-bottom: 2px;\nfont-weight: bold;\nfont-size: 16px;\ncolor: rgb(255, 255, 255);").toString());
	ui->windowIconFrame->setStyleSheet(reader.value("windowIconFrameStyleSheet", "QFrame#windowIconFrame {\n	border-image: url(:/Resource/windowIconFrame.png);\n}").toString());
	ui->windowIcon->setVisible(reader.value("windowIconVisible", true).toBool());
	ui->windowIcon->setIconSize(reader.value("windowIconSize", QSize(20, 20)).toSize());
}
