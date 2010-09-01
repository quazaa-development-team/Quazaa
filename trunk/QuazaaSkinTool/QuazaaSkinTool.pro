# -------------------------------------------------
# Project created by QtCreator 2010-02-18T20:24:21
# -------------------------------------------------
CONFIG += designer
TARGET = QuazaaSkinTool
CONFIG(debug, debug|release):TARGET = $$join(TARGET,,,_debug)
INCLUDEPATH += . \
	text
CONFIG(debug, debug|release):INCLUDEPATH += temp/debug
CONFIG(release):INCLUDEPATH += temp/release
TEMPLATE = app
SOURCES += main.cpp \
	mainwindow.cpp \
	dialogopenskin.cpp \
	dialognewskin.cpp \
	csshighlighter.cpp \
	../QSkinDialog/qskinsettings.cpp \
	widgetextendeditems.cpp \
	widgetnavigation.cpp \
	qcssparser.cpp \
	qcssscanner.cpp
HEADERS += mainwindow.h \
	dialogopenskin.h \
	dialognewskin.h \
	csshighlighter.h \
	../QSkinDialog/qskinsettings.h \
	widgetextendeditems.h \
	widgetnavigation.h \
	qcssparser.h
FORMS += mainwindow.ui \
	dialogopenskin.ui \
	dialognewskin.ui \
	widgetextendeditems.ui \
	widgetnavigation.ui
TRANSLATIONS = ../Language/quazaaskintool_af.ts \
	../Language/quazaaskintool_ar.ts \
	../Language/quazaaskintool_ca.ts \
	../Language/quazaaskintool_chs.ts \
	../Language/quazaaskintool_cz.ts \
	../Language/quazaaskintool_de.ts \
	../Language/quazaaskintool_default_en.ts \
	../Language/quazaaskintool_ee.ts \
	../Language/quazaaskintool_es.ts \
	../Language/quazaaskintool_fi.ts \
	../Language/quazaaskintool_fr.ts \
	../Language/quazaaskintool_gr.ts \
	../Language/quazaaskintool_heb.ts \
	../Language/quazaaskintool_hr.ts \
	../Language/quazaaskintool_hu.ts \
	../Language/quazaaskintool_it.ts \
	../Language/quazaaskintool_ja.ts \
	../Language/quazaaskintool_lt.ts \
	../Language/quazaaskintool_nl.ts \
	../Language/quazaaskintool_no.ts \
	../Language/quazaaskintool_pt.ts \
	../Language/quazaaskintool_pt-br.ts \
	../Language/quazaaskintool_ru.ts \
	../Language/quazaaskintool_sl-si.ts \
	../Language/quazaaskintool_sq.ts \
	../Language/quazaaskintool_sr.ts \
	../Language/quazaaskintool_sv.ts \
	../Language/quazaaskintool_tr.ts \
	../Language/quazaaskintool_tw.ts
RESOURCES += Resource.qrc
RC_FILE = Quazaa.rc
OTHER_FILES += LICENSE.GPL3
DESTDIR = ../bin
CONFIG(debug, debug|release):OBJECTS_DIR = temp/debug
CONFIG(release):OBJECTS_DIR = temp/release
CONFIG(debug, debug|release):MOC_DIR = temp/debug
CONFIG(release):MOC_DIR = temp/release
CONFIG(debug, debug|release):UI_DIR = temp/debug
CONFIG(release):UI_DIR = temp/release
include(qtgradienteditor/qtgradienteditor.pri)
