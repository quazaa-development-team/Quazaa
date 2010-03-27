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
 TRANSLATIONS = ../quazaaskintool_af.ts \
	../quazaaskintool_ar.ts \
	../quazaaskintool_ca.ts \
	../quazaaskintool_chs.ts \
	../quazaaskintool_cz.ts \
	../quazaaskintool_de.ts \
	../quazaaskintool_default_en.ts \
	../quazaaskintool_ee.ts \
	../quazaaskintool_es.ts \
	../quazaaskintool_es-mx.ts \
	../quazaaskintool_fi.ts \
	../quazaaskintool_fr.ts \
	../quazaaskintool_gr.ts \
	../quazaaskintool_heb.ts \
	../quazaaskintool_hr.ts \
	../quazaaskintool_hu.ts \
	../quazaaskintool_it.ts \
	../quazaaskintool_ja.ts \
	../quazaaskintool_lt.ts \
	../quazaaskintool_nl.ts \
	../quazaaskintool_no.ts \
	../quazaaskintool_pt.ts \
	../quazaaskintool_pt-br.ts \
	../quazaaskintool_ru.ts \
	../quazaaskintool_sl-si.ts \
	../quazaaskintool_sq.ts \
	../quazaaskintool_sr.ts \
	../quazaaskintool_sv.ts \
	../quazaaskintool_tr.ts \
	../quazaaskintool_tw.ts
RESOURCES += Resource.qrc
RC_FILE = Quazaa.rc
OTHER_FILES += LICENSE.GPL3
DESTDIR = ../bin
include(qtgradienteditor/qtgradienteditor.pri)
