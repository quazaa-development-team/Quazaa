# -------------------------------------------------
# Project created by QtCreator 2010-02-18T20:24:21
# -------------------------------------------------
QT += phonon
CONFIG += designer
TARGET = QuazaaSkinTool
TEMPLATE = app
SOURCES += main.cpp \
	mainwindow.cpp \
	dialogopenskin.cpp \
	dialognewskin.cpp \
	csshighlighter.cpp \
	../QSkinDialog/qskinsettings.cpp
HEADERS += mainwindow.h \
	dialogopenskin.h \
	dialognewskin.h \
	csshighlighter.h \
	../QSkinDialog/qskinsettings.h
FORMS += mainwindow.ui \
	dialogopenskin.ui \
	dialognewskin.ui
RESOURCES += Resource.qrc
RC_FILE = Quazaa.rc
OTHER_FILES += LICENSE.GPL3
include(qtgradienteditor/qtgradienteditor.pri)
