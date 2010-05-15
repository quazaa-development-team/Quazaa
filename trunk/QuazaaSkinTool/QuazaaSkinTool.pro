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
 TRANSLATIONS = ../Language/quazaaskintool_af.ts \
	../Language/quazaaskintool_ar.ts \
	../Language/quazaaskintool_ca.ts \
	../Language/quazaaskintool_chs.ts \
	../Language/quazaaskintool_cz.ts \
	../Language/quazaaskintool_de.ts \
	../Language/quazaaskintool_default_en.ts \
	../Language/quazaaskintool_ee.ts \
	../Language/quazaaskintool_es.ts \
	../Language/quazaaskintool_es-mx.ts \
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
include(qtgradienteditor/qtgradienteditor.pri)
