# -------------------------------------------------
# Project created by QtCreator 2010-02-18T20:24:21
# -------------------------------------------------
#
# QuazaaSkinTool.pro
#
# Copyright © Quazaa Development Team, 2009-2010.
# This file is part of QUAZAA (quazaa.sourceforge.net)
#
# Quazaa is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3 of
# the License, or (at your option) any later version.
#
# Quazaa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Quazaa; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

# Qt modules used in application
#CONFIG += designer
TARGET = QuazaaSkinTool

# Paths
DESTDIR = ../bin
CONFIG(debug){
	OBJECTS_DIR = temp/obj/debug
	RCC_DIR = temp/qrc/debug
} else {
	OBJECTS_DIR = temp/obj/release
	RCC_DIR = temp/qrc/release
}
MOC_DIR = temp/moc
UI_DIR = temp/uic

INCLUDEPATH += . \
	text

# Append _debug to executable name when compiling using debug config
CONFIG(debug, debug|release):TARGET = $$join(TARGET,,,_debug)

#Additional config
mac:CONFIG -= app_bundle

TEMPLATE = app

# MSVC-specific compiler flags
win32-msvc2008{
	!build_pass:message(Setting up MSVC 2008 Compiler flags)
	QMAKE_CFLAGS_DEBUG += /Gd /arch:SSE2 /Gm /RTC1
	QMAKE_CFLAGS_RELEASE += /Gd /arch:SSE2 /GA

	QMAKE_CXXFLAGS_DEBUG += /Gd /arch:SSE2 /Gm /RTC1
	QMAKE_CXXFLAGS_RELEASE += /Gd /arch:SSE2 /GA

	QMAKE_LFLAGS_DEBUG += /FIXED:NO
}

# Sources
SOURCES += main.cpp \
	mainwindow.cpp \
	dialogopenskin.cpp \
	dialognewskin.cpp \
	csshighlighter.cpp \
	../QSkinDialog/qskinsettings.cpp \
	widgetextendeditems.cpp \
	widgetnavigation.cpp \
	quazaacssparser.cpp \
	quazaacssscanner.cpp
HEADERS += mainwindow.h \
	dialogopenskin.h \
	dialognewskin.h \
	csshighlighter.h \
	../QSkinDialog/qskinsettings.h \
	widgetextendeditems.h \
	widgetnavigation.h \
	quazaacssparser.h
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
	../Language/quazaaskintool_pl.ts \
	../Language/quazaaskintool_pt.ts \
	../Language/quazaaskintool_pt-br.ts \
	../Language/quazaaskintool_ru.ts \
	../Language/quazaaskintool_sl-si.ts \
	../Language/quazaaskintool_sq.ts \
	../Language/quazaaskintool_sr.ts \
	../Language/quazaaskintool_sv.ts \
	../Language/quazaaskintool_tr.ts \
	../Language/quazaaskintool_tw.ts
include(qtgradienteditor/qtgradienteditor.pri)
RESOURCES += Resource.qrc
RC_FILE = Quazaa.rc
OTHER_FILES += LICENSE.GPL3
