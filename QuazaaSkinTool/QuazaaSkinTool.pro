# -------------------------------------------------
# Project created by QtCreator 2010-02-18T20:24:21
# -------------------------------------------------
QT += phonon
CONFIG += designer
TARGET = QuazaaSkinTool
CONFIG(debug, debug|release) {
	mac: TARGET = $$join(TARGET,,,_debug)
	win32: TARGET = $$join(TARGET,,,d)
}
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
 TRANSLATIONS = language/quazaaskintool_af.ts \
	language/quazaaskintool_ar.ts \
	language/quazaaskintool_ca.ts \
	language/quazaaskintool_chs.ts \
	language/quazaaskintool_cz.ts \
	language/quazaaskintool_de.ts \
	language/quazaaskintool_default_en.ts \
	language/quazaaskintool_ee.ts \
	language/quazaaskintool_es.ts \
	language/quazaaskintool_es-mx.ts \
	language/quazaaskintool_fi.ts \
	language/quazaaskintool_fr.ts \
	language/quazaaskintool_gr.ts \
	language/quazaaskintool_heb.ts \
	language/quazaaskintool_hr.ts \
	language/quazaaskintool_hu.ts \
	language/quazaaskintool_it.ts \
	language/quazaaskintool_ja.ts \
	language/quazaaskintool_lt.ts \
	language/quazaaskintool_nl.ts \
	language/quazaaskintool_no.ts \
	language/quazaaskintool_pt.ts \
	language/quazaaskintool_pt-br.ts \
	language/quazaaskintool_ru.ts \
	language/quazaaskintool_sl-si.ts \
	language/quazaaskintool_sq.ts \
	language/quazaaskintool_sr.ts \
	language/quazaaskintool_sv.ts \
	language/quazaaskintool_tr.ts \
	language/quazaaskintool_tw.ts
RESOURCES += Resource.qrc
RC_FILE = Quazaa.rc
OTHER_FILES += LICENSE.GPL3
DESTDIR = ../bin
include(qtgradienteditor/qtgradienteditor.pri)
