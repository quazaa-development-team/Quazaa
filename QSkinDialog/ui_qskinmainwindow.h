/********************************************************************************
** Form generated from reading UI file 'qskinmainwindow.ui'
**
** Created: Wed Dec 23 16:28:24 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QSKINMAINWINDOW_H
#define UI_QSKINMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QSkinMainWindow
{
public:
    QAction *actionChangeSkin;
    QGridLayout *gridLayout;
    QFrame *windowFrameLeft;
    QFrame *windowFrameBottom;
    QFrame *windowFrameTop;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayoutTitlebar;
    QFrame *titlebarFrame;
    QHBoxLayout *horizontalLayout;
    QToolButton *applicationIcon;
    QLabel *windowText;
    QFrame *titlebarButtonsFrame;
    QGridLayout *gridLayoutTitlebarButtons;
    QToolButton *minimizeButton;
    QToolButton *maximizeButton;
    QToolButton *closeButton;
    QSpacerItem *horizontalSpacerTitlebar;
    QWidget *widgetContents;
    QGridLayout *gridLayoutContents;
    QMainWindow *MainWindow;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menu_Tools;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QFrame *windowFrameTopLeft;
    QFrame *windowFrameTopRight;
    QFrame *windowFrameBottomLeft;
    QFrame *windowFrameBottomRight;
    QFrame *windowFrameRight;

    void setupUi(QDialog *QSkinMainWindow)
    {
        if (QSkinMainWindow->objectName().isEmpty())
            QSkinMainWindow->setObjectName(QString::fromUtf8("QSkinMainWindow"));
        QSkinMainWindow->resize(383, 246);
        QSkinMainWindow->setStyleSheet(QString::fromUtf8("QDialog#QSkinDialog {\n"
"	background-color: transparent;\n"
"}"));
        QSkinMainWindow->setSizeGripEnabled(false);
        actionChangeSkin = new QAction(QSkinMainWindow);
        actionChangeSkin->setObjectName(QString::fromUtf8("actionChangeSkin"));
        gridLayout = new QGridLayout(QSkinMainWindow);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        windowFrameLeft = new QFrame(QSkinMainWindow);
        windowFrameLeft->setObjectName(QString::fromUtf8("windowFrameLeft"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(windowFrameLeft->sizePolicy().hasHeightForWidth());
        windowFrameLeft->setSizePolicy(sizePolicy);
        windowFrameLeft->setMinimumSize(QSize(12, 0));
        windowFrameLeft->setMaximumSize(QSize(12, 16777215));
        windowFrameLeft->setCursor(QCursor(Qt::SizeHorCursor));
        windowFrameLeft->setStyleSheet(QString::fromUtf8("border-image: url(:/Resource/frameLeft.png);\n"
"border-left: 1;\n"
"border-top: 10;"));
        windowFrameLeft->setFrameShape(QFrame::NoFrame);
        windowFrameLeft->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(windowFrameLeft, 1, 0, 2, 1);

        windowFrameBottom = new QFrame(QSkinMainWindow);
        windowFrameBottom->setObjectName(QString::fromUtf8("windowFrameBottom"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(windowFrameBottom->sizePolicy().hasHeightForWidth());
        windowFrameBottom->setSizePolicy(sizePolicy1);
        windowFrameBottom->setMinimumSize(QSize(0, 12));
        windowFrameBottom->setMaximumSize(QSize(16777215, 12));
        windowFrameBottom->setCursor(QCursor(Qt::SizeVerCursor));
        windowFrameBottom->setStyleSheet(QString::fromUtf8("border-image: url(:/Resource/frameBottom.png);\n"
"border-bottom: 1;"));
        windowFrameBottom->setFrameShape(QFrame::NoFrame);
        windowFrameBottom->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(windowFrameBottom, 3, 1, 1, 1);

        windowFrameTop = new QFrame(QSkinMainWindow);
        windowFrameTop->setObjectName(QString::fromUtf8("windowFrameTop"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(windowFrameTop->sizePolicy().hasHeightForWidth());
        windowFrameTop->setSizePolicy(sizePolicy2);
        windowFrameTop->setMinimumSize(QSize(0, 30));
        windowFrameTop->setMaximumSize(QSize(16777215, 30));
        windowFrameTop->setContextMenuPolicy(Qt::CustomContextMenu);
        windowFrameTop->setStyleSheet(QString::fromUtf8("QFrame#windowFrameTop {\n"
"	border-left: 85;\n"
"	padding-left: -85;\n"
"	border-image: url(:/Resource/frameTop.png);\n"
"}"));
        windowFrameTop->setFrameShape(QFrame::NoFrame);
        windowFrameTop->setFrameShadow(QFrame::Raised);
        gridLayout_3 = new QGridLayout(windowFrameTop);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(0);
        gridLayoutTitlebar = new QGridLayout();
        gridLayoutTitlebar->setSpacing(0);
        gridLayoutTitlebar->setObjectName(QString::fromUtf8("gridLayoutTitlebar"));
        gridLayoutTitlebar->setContentsMargins(0, -1, 0, 0);
        titlebarFrame = new QFrame(windowFrameTop);
        titlebarFrame->setObjectName(QString::fromUtf8("titlebarFrame"));
        titlebarFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"	background-color: transparent;\n"
"}"));
        titlebarFrame->setFrameShape(QFrame::NoFrame);
        titlebarFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(titlebarFrame);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        applicationIcon = new QToolButton(titlebarFrame);
        applicationIcon->setObjectName(QString::fromUtf8("applicationIcon"));
        applicationIcon->setContextMenuPolicy(Qt::CustomContextMenu);
        applicationIcon->setStyleSheet(QString::fromUtf8("border: 0px solid transparent;"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Resource/qtlogo-64.png"), QSize(), QIcon::Normal, QIcon::Off);
        applicationIcon->setIcon(icon);
        applicationIcon->setIconSize(QSize(20, 20));

        horizontalLayout->addWidget(applicationIcon);

        windowText = new QLabel(titlebarFrame);
        windowText->setObjectName(QString::fromUtf8("windowText"));
        windowText->setContextMenuPolicy(Qt::CustomContextMenu);
        windowText->setStyleSheet(QString::fromUtf8("font-weight: bold;"));

        horizontalLayout->addWidget(windowText);


        gridLayoutTitlebar->addWidget(titlebarFrame, 0, 1, 2, 1);

        titlebarButtonsFrame = new QFrame(windowFrameTop);
        titlebarButtonsFrame->setObjectName(QString::fromUtf8("titlebarButtonsFrame"));
        titlebarButtonsFrame->setStyleSheet(QString::fromUtf8("/*These move the buttons up so they aren't displayed in the center of the titlebar\n"
"   Remove these to center your buttons on the titlebar.*/\n"
"QFrame#titlebarButtonsFrame {\n"
"	padding-top: -1;\n"
"	padding-bottom: 10;\n"
"}"));
        titlebarButtonsFrame->setFrameShape(QFrame::NoFrame);
        titlebarButtonsFrame->setFrameShadow(QFrame::Raised);
        gridLayoutTitlebarButtons = new QGridLayout(titlebarButtonsFrame);
        gridLayoutTitlebarButtons->setSpacing(0);
        gridLayoutTitlebarButtons->setContentsMargins(0, 0, 0, 0);
        gridLayoutTitlebarButtons->setObjectName(QString::fromUtf8("gridLayoutTitlebarButtons"));
        minimizeButton = new QToolButton(titlebarButtonsFrame);
        minimizeButton->setObjectName(QString::fromUtf8("minimizeButton"));
        minimizeButton->setMinimumSize(QSize(26, 18));
        minimizeButton->setMaximumSize(QSize(26, 18));
        minimizeButton->setStyleSheet(QString::fromUtf8("QToolButton {\n"
"	border: 0px solid transparent;\n"
"	border-image: url(:/Resource/minButton.png);\n"
"}\n"
"\n"
"QToolButton:hover {\n"
"	border-image: url(:/Resource/minButtonH.png);\n"
"}\n"
"\n"
"QToolButton:disabled {\n"
"	border-image: url(:/Resource/minButtonD.png);\n"
"}"));

        gridLayoutTitlebarButtons->addWidget(minimizeButton, 0, 0, 1, 1);

        maximizeButton = new QToolButton(titlebarButtonsFrame);
        maximizeButton->setObjectName(QString::fromUtf8("maximizeButton"));
        maximizeButton->setMinimumSize(QSize(26, 18));
        maximizeButton->setMaximumSize(QSize(26, 18));
        maximizeButton->setStyleSheet(QString::fromUtf8("QToolButton {\n"
"	border: 0px solid transparent;\n"
"	border-image: url(:/Resource/maxButton.png);\n"
"}\n"
"\n"
"QToolButton:hover {\n"
"	border-image: url(:/Resource/maxButtonH.png);\n"
"}\n"
"\n"
"QToolButton:disabled {\n"
"	border-image: url(:/Resource/maxButtonD.png);\n"
"}\n"
"\n"
"QToolButton:checked { /*The button will be checked when maximized is set*/\n"
"	\n"
"	border-image: url(:/Resource/restoreButton.png);\n"
"}\n"
"\n"
"QToolButton:checked:hover {\n"
"	\n"
"	border-image: url(:/Resource/restoreButtonH.png);\n"
"}\n"
"\n"
"QToolButton:checked:disabled {\n"
"	\n"
"	border-image: url(:/Resource/restoreButtonD.png);\n"
"}"));
        maximizeButton->setCheckable(true);
        maximizeButton->setChecked(false);

        gridLayoutTitlebarButtons->addWidget(maximizeButton, 0, 1, 1, 1);

        closeButton = new QToolButton(titlebarButtonsFrame);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        closeButton->setEnabled(true);
        closeButton->setMinimumSize(QSize(44, 18));
        closeButton->setMaximumSize(QSize(44, 18));
        closeButton->setStyleSheet(QString::fromUtf8("QToolButton {\n"
"	border: 0px solid transparent;\n"
"	border-image: url(:/Resource/quitButton.png);\n"
"}\n"
"\n"
"QToolButton:hover {\n"
"	border-image: url(:/Resource/quitButtonH.png);\n"
"}\n"
"\n"
"QToolButton:disabled {\n"
"	border-image: url(:/Resource/quitButtonD.png);\n"
"}"));
        closeButton->setIconSize(QSize(16, 16));

        gridLayoutTitlebarButtons->addWidget(closeButton, 0, 2, 1, 1);


        gridLayoutTitlebar->addWidget(titlebarButtonsFrame, 0, 3, 2, 1);

        horizontalSpacerTitlebar = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayoutTitlebar->addItem(horizontalSpacerTitlebar, 0, 2, 2, 1);


        gridLayout_3->addLayout(gridLayoutTitlebar, 0, 0, 1, 1);


        gridLayout->addWidget(windowFrameTop, 0, 1, 2, 1);

        widgetContents = new QWidget(QSkinMainWindow);
        widgetContents->setObjectName(QString::fromUtf8("widgetContents"));
        widgetContents->setStyleSheet(QString::fromUtf8(""));
        gridLayoutContents = new QGridLayout(widgetContents);
        gridLayoutContents->setSpacing(0);
        gridLayoutContents->setContentsMargins(0, 0, 0, 0);
        gridLayoutContents->setObjectName(QString::fromUtf8("gridLayoutContents"));
        MainWindow = new QMainWindow(widgetContents);
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setAutoFillBackground(true);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 359, 20));
        menu_Tools = new QMenu(menubar);
        menu_Tools->setObjectName(QString::fromUtf8("menu_Tools"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        gridLayoutContents->addWidget(MainWindow, 0, 0, 1, 1);


        gridLayout->addWidget(widgetContents, 2, 1, 1, 1);

        windowFrameTopLeft = new QFrame(QSkinMainWindow);
        windowFrameTopLeft->setObjectName(QString::fromUtf8("windowFrameTopLeft"));
        windowFrameTopLeft->setMinimumSize(QSize(12, 12));
        windowFrameTopLeft->setMaximumSize(QSize(12, 12));
        windowFrameTopLeft->setCursor(QCursor(Qt::SizeFDiagCursor));
        windowFrameTopLeft->setStyleSheet(QString::fromUtf8("border-image: url(:/Resource/frameTopLeft.png);"));
        windowFrameTopLeft->setFrameShape(QFrame::NoFrame);
        windowFrameTopLeft->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(windowFrameTopLeft, 0, 0, 1, 1);

        windowFrameTopRight = new QFrame(QSkinMainWindow);
        windowFrameTopRight->setObjectName(QString::fromUtf8("windowFrameTopRight"));
        windowFrameTopRight->setMinimumSize(QSize(12, 12));
        windowFrameTopRight->setMaximumSize(QSize(12, 12));
        windowFrameTopRight->setCursor(QCursor(Qt::SizeBDiagCursor));
        windowFrameTopRight->setStyleSheet(QString::fromUtf8("border-image: url(:/Resource/frameTopRight.png);"));
        windowFrameTopRight->setFrameShape(QFrame::NoFrame);
        windowFrameTopRight->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(windowFrameTopRight, 0, 2, 1, 1);

        windowFrameBottomLeft = new QFrame(QSkinMainWindow);
        windowFrameBottomLeft->setObjectName(QString::fromUtf8("windowFrameBottomLeft"));
        windowFrameBottomLeft->setMinimumSize(QSize(12, 12));
        windowFrameBottomLeft->setMaximumSize(QSize(12, 12));
        windowFrameBottomLeft->setCursor(QCursor(Qt::SizeBDiagCursor));
        windowFrameBottomLeft->setStyleSheet(QString::fromUtf8("border-image: url(:/Resource/frameBottomLeft.png);"));
        windowFrameBottomLeft->setFrameShape(QFrame::NoFrame);
        windowFrameBottomLeft->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(windowFrameBottomLeft, 3, 0, 1, 1);

        windowFrameBottomRight = new QFrame(QSkinMainWindow);
        windowFrameBottomRight->setObjectName(QString::fromUtf8("windowFrameBottomRight"));
        windowFrameBottomRight->setMinimumSize(QSize(12, 12));
        windowFrameBottomRight->setMaximumSize(QSize(12, 12));
        windowFrameBottomRight->setCursor(QCursor(Qt::SizeFDiagCursor));
        windowFrameBottomRight->setStyleSheet(QString::fromUtf8("border-image: url(:/Resource/frameBottomRight.png);"));
        windowFrameBottomRight->setFrameShape(QFrame::NoFrame);
        windowFrameBottomRight->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(windowFrameBottomRight, 3, 2, 1, 1);

        windowFrameRight = new QFrame(QSkinMainWindow);
        windowFrameRight->setObjectName(QString::fromUtf8("windowFrameRight"));
        sizePolicy.setHeightForWidth(windowFrameRight->sizePolicy().hasHeightForWidth());
        windowFrameRight->setSizePolicy(sizePolicy);
        windowFrameRight->setMinimumSize(QSize(12, 0));
        windowFrameRight->setMaximumSize(QSize(12, 16777215));
        windowFrameRight->setCursor(QCursor(Qt::SizeHorCursor));
        windowFrameRight->setStyleSheet(QString::fromUtf8("QFrame {\n"
"	border-image: url(:/Resource/frameRight.png);\n"
"	border-right: 1;\n"
"	border-top: 10;\n"
"}"));
        windowFrameRight->setFrameShape(QFrame::NoFrame);
        windowFrameRight->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(windowFrameRight, 1, 2, 2, 1);


        menubar->addAction(menu_Tools->menuAction());
        menu_Tools->addAction(actionChangeSkin);

        retranslateUi(QSkinMainWindow);

        QMetaObject::connectSlotsByName(QSkinMainWindow);
    } // setupUi

    void retranslateUi(QDialog *QSkinMainWindow)
    {
        QSkinMainWindow->setWindowTitle(QApplication::translate("QSkinMainWindow", "QSkinDialog", 0, QApplication::UnicodeUTF8));
        actionChangeSkin->setText(QApplication::translate("QSkinMainWindow", "Change &Skin", 0, QApplication::UnicodeUTF8));
        applicationIcon->setText(QString());
        windowText->setText(QApplication::translate("QSkinMainWindow", "QSkinMainWindow", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        minimizeButton->setToolTip(QApplication::translate("QSkinMainWindow", "Minimize", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        minimizeButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        maximizeButton->setToolTip(QApplication::translate("QSkinMainWindow", "Maximize", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        maximizeButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        closeButton->setToolTip(QApplication::translate("QSkinMainWindow", "Close", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        closeButton->setText(QString());
        MainWindow->setWindowTitle(QApplication::translate("QSkinMainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        menu_Tools->setTitle(QApplication::translate("QSkinMainWindow", "&Tools", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("QSkinMainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QSkinMainWindow: public Ui_QSkinMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QSKINMAINWINDOW_H
