/********************************************************************************************************
 * PROGRAM		: QSkinDialog
 * DATE - TIME	: 27 December 2009 - 14h38
 * AUTHOR		: (SmokeX)
 * FILENAME		: dialogskinpreview.cpp
 * LICENSE		: QSkinDialog is free software; you can redistribute it
 *				  and/or modify it under the terms of the GNU General Public License
 *				  as published by the Free Software Foundation; either version 3 of
 *				  the License, or (at your option) any later version.
 * COMMENTARY   : A preview dialog for a given skin
 ********************************************************************************************************/

#include "dialogskinpreview.h"
#include "ui_dialogskinpreview.h"

DialogSkinPreview::DialogSkinPreview(bool sizable, bool closable, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogSkinPreview)
{
	ui->setupUi(this);
	maximized = false;
	movable = false;
	dialogSizable = sizable;
	dialogClosable = closable;
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

	// To enable alpha blending and transparency in the frame
	setAttribute(Qt::WA_TranslucentBackground);
	// We are using our own frame of course instead of the system frame
	if(!parent == 0)
	{
		this->setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
	} else {
		this->setWindowFlags(Qt::FramelessWindowHint);
	}
	this->setMaximumSize(QApplication::desktop()->availableGeometry(this).width(), QApplication::desktop()->availableGeometry(this).height());

	// Unset the frameless window hint for our child widget or it won't display at runtime
	// AutoFillbackground has to be set also or the child gets erased in the transparency mask
	// In this example it is set in the ui file
	ui->widgetContents->setWindowFlags(ui->widgetContents->windowFlags() & ~Qt::FramelessWindowHint);

	ui->minimizeButton->setEnabled(dialogSizable);
	ui->maximizeButton->setEnabled(dialogSizable);
	ui->closeButton->setEnabled(dialogClosable);
	if (!dialogSizable)
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

	this->setWindowTitle("Skin Preview");
	this->setWindowIcon(QIcon(":/Resource/Quazaa48.png"));
}

DialogSkinPreview::~DialogSkinPreview()
{
	delete ui;
}

void DialogSkinPreview::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	case QEvent::ActivationChange:
		if (minimized)
		{
			setMinimized(false);
		}
		break;
	default:
		break;
	}
}


void DialogSkinPreview::onClose()
{
	close();
}

void DialogSkinPreview::onMaximize()
{
	setMaximized(!maximized);
}

void DialogSkinPreview::setMaximized(bool maximize)
{
	ui->maximizeButton->setChecked(maximize);
	if (maximize)
	{
		normalGeometry = this->geometry();
		setGeometry(0,0,QApplication::desktop()->availableGeometry(this).width(),QApplication::desktop()->availableGeometry(this).height());
		maximized = true;
		ui->maximizeButton->setToolTip("Restore");
		systemRestoreAction->setEnabled(true);
		systemMaximizeAction->setEnabled(false);
		ui->windowFrameTopLeft->setVisible(false);
		ui->windowFrameLeft->setVisible(false);
		ui->windowFrameBottomLeft->setVisible(false);
		ui->windowFrameBottom->setVisible(false);
		ui->windowFrameTopRight->setVisible(false);
		ui->windowFrameRight->setVisible(false);
		ui->windowFrameBottomRight->setVisible(false);
	} else {
		this->setGeometry(normalGeometry);
		maximized = false;
		ui->maximizeButton->setToolTip("Maximize");
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

void DialogSkinPreview::onMinimize()
{
	setMinimized(!minimized);
}

void DialogSkinPreview::setMinimized(bool minimize)
{
	if (minimize)
	{
		if (!maximized)
			normalGeometry = this->geometry();

		setGeometry(QApplication::desktop()->availableGeometry(this).width(),QApplication::desktop()->availableGeometry(this).height(),0,0);
		minimized = true;
	} else {
		if (maximized)
			setGeometry(0,0,QApplication::desktop()->availableGeometry(this).width(),QApplication::desktop()->availableGeometry(this).height());
		else
			this->setGeometry(normalGeometry);
		minimized = false;
	}
}

void DialogSkinPreview::mousePressEvent(QMouseEvent *e)
{
	switch (e->button())
	{
		case Qt::LeftButton:
		if((ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
			&& !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && !maximized)
		{
			qApp->setOverrideCursor(QCursor(Qt::SizeAllCursor));
			dragPosition = e->globalPos() - frameGeometry().topLeft();
			movable = true;
			e->accept();
		} else if (ui->windowFrameTopLeft->underMouse() && dialogSizable) {
			dragPosition = e->globalPos() - frameGeometry().topLeft();
			sizableTopLeft = true;
			e->accept();
		} else if (ui->windowFrameLeft->underMouse() && dialogSizable) {
			iDragPosition = e->globalPos().x() - frameGeometry().left();
			this->sizableLeft = true;
			e->accept();
		} else if (ui->windowFrameBottomLeft->underMouse() && dialogSizable) {
			dragPosition = e->globalPos() - frameGeometry().bottomLeft();
			sizableBottomLeft = true;
			e->accept();
		} else if (ui->windowFrameBottom->underMouse() && dialogSizable) {
			iDragPosition = e->globalPos().y() - frameGeometry().bottom();
			sizableBottom = true;
			e->accept();
		} else if (ui->windowFrameTopRight->underMouse() && dialogSizable) {
			dragPosition = e->globalPos() - frameGeometry().topRight();
			sizableTopRight = true;
			e->accept();
		} else if (ui->windowFrameRight->underMouse() && dialogSizable) {
			iDragPosition = e->globalPos().x() - frameGeometry().right();
			sizableRight = true;
			e->accept();
		} else if (ui->windowFrameBottomRight->underMouse() && dialogSizable) {
			dragPosition = e->globalPos() - frameGeometry().bottomRight();
			sizableBottomRight = true;
			e->accept();
		} else {
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

void DialogSkinPreview::mouseMoveEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && (ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
		&& !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && movable && !maximized)
	{
		move(e->globalPos() - dragPosition);
		e->accept();
	} else if (ui->windowFrameTopLeft->underMouse() && sizableTopLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setTopLeft(e->globalPos() - dragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameLeft->underMouse() && sizableLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setLeft(e->globalPos().x() - iDragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameBottomLeft->underMouse() && sizableBottomLeft && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottomLeft(e->globalPos() - dragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameBottom->underMouse() && sizableBottom && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottom(e->globalPos().y() - iDragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameTopRight->underMouse() && sizableTopRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setTopRight(e->globalPos() - dragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameRight->underMouse() && sizableRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setRight(e->globalPos().x() - iDragPosition);
		setGeometry(newSize);
		e->accept();
	} else if (ui->windowFrameBottomRight->underMouse() && sizableBottomRight && dialogSizable) {
		QRect newSize = geometry();
		newSize.setBottomRight(e->globalPos() - dragPosition);
		setGeometry(newSize);
		e->accept();
	}
}

void DialogSkinPreview::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton && (ui->windowFrameTop->underMouse() || ui->windowText->underMouse()))
	{
		while (qApp->overrideCursor() != 0)
		qApp->restoreOverrideCursor();
	}
}

void DialogSkinPreview::mouseDoubleClickEvent(QMouseEvent *e)
{
	if ((e->button() == Qt::LeftButton) && (ui->windowFrameTop->underMouse()  || ui->windowText->underMouse()) && !ui->windowIcon->underMouse() && !ui->closeButton->underMouse()
		&& !ui->minimizeButton->underMouse() && !ui->maximizeButton->underMouse() && dialogSizable)
	{
		onMaximize();
	}
}

void DialogSkinPreview::loadSkin(QString file)
{
	QSettings reader(file, QSettings::IniFormat);
	ui->lineEditName->setText(reader.value("skinName", "").toString());
	ui->lineEditAuthor->setText(reader.value("skinAuthor", "").toString());
	ui->lineEditVersion->setText(reader.value("skinVersion", "").toString());
	ui->plainTextEditDescription->setPlainText(reader.value("skinDescription", "").toString());
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

void DialogSkinPreview::on_windowText_customContextMenuRequested(QPoint pos)
{
	systemMenu->exec(QCursor::pos());
}

void DialogSkinPreview::on_windowFrameTop_customContextMenuRequested(QPoint pos)
{
	systemMenu->exec(QCursor::pos());
}

void DialogSkinPreview::setWindowTitle(const QString &title)
{
	ui->windowText->setText(title);
	return QDialog::setWindowTitle(title);
}

void DialogSkinPreview::setWindowIcon(const QIcon &icon)
{
	QIcon windowIcon = icon;
	if (windowIcon.isNull())
	{
		windowIcon = QIcon(":/Resource/quazaa48.png");
	}

	ui->windowIcon->setIcon(windowIcon);
	return QDialog::setWindowIcon(windowIcon);
}

void DialogSkinPreview::on_windowIcon_customContextMenuRequested(QPoint pos)
{
	// This is where your custom right click menu would go for the window icon
	systemMenu->exec(ui->windowIcon->mapToGlobal(ui->windowIcon->geometry().bottomLeft()));
}

void DialogSkinPreview::on_windowIcon_clicked()
{
	systemMenu->exec(ui->windowIcon->mapToGlobal(ui->windowIcon->geometry().bottomLeft()));
}
