XUPProjectSettings:QT_VERSION = Qt \
    System \
    (4.5.0)

# Quazaa.pro
# Copyright Quazaa Development Team, 2009.
# This file is part of QUAZAA (quazaa.sourceforge.net)
# Quazaa is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3 of
# the License, or (at your option) any later version.
# Quazaa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with Quazaa; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
# -------------------------------------------------
# Project created by QtCreator 2009-03-06T16:54:32
# -------------------------------------------------
QT = core \
    gui \
    network \
    sql \
    webkit \
    xml \
    xmlpatterns
CONFIG += exceptions \
    rtti \
    stl \
    thread \
    no_keywords
TARGET = Quazaa
TEMPLATE = app
SOURCES = main.cpp \
    mainwindow.cpp \
    persistentsettings.cpp \
    dialogwizard.cpp \
    dialogabout.cpp \
    dialogsettings.cpp \
    dialogsplash.cpp \
    dialogprofile.cpp \
    dialogeditshares.cpp \
    dialogcreatetorrent.cpp \
    dialogopentorrent.cpp \
    dialogtorrentproperties.cpp \
    dialogdownloadproperties.cpp \
    dialoglanguage.cpp \
    dialogselectvisualisation.cpp \
    dialogfiltersearch.cpp \
    dialogdownloadmonitor.cpp \
    dialogadddownload.cpp \
    dialogaddrule.cpp \
    dialogsecuritysubscriptions.cpp \
    dialogaddsecuritysubscription.cpp \
    dialogdownloadsimport.cpp \
    dialogscheduler.cpp \
    quazaaglobals.cpp \
    dialoggplview.cpp \
    commonfunctions.cpp \
    dialoghashprogress.cpp \
    dialogtransferprogresstooltip.cpp \
    dialogpreviewprepare.cpp \
    librarybuilder.cpp \
    dialogclosetype.cpp
HEADERS = mainwindow.h \
    persistentsettings.h \
    dialogwizard.h \
    dialogabout.h \
    dialogsettings.h \
    dialogsplash.h \
    dialogprofile.h \
    dialogeditshares.h \
    dialogcreatetorrent.h \
    dialogopentorrent.h \
    dialogtorrentproperties.h \
    dialogdownloadproperties.h \
    dialoglanguage.h \
    dialogselectvisualisation.h \
    dialogfiltersearch.h \
    dialogdownloadmonitor.h \
    dialogadddownload.h \
    dialogaddrule.h \
    dialogsecuritysubscriptions.h \
    dialogaddsecuritysubscription.h \
    dialogdownloadsimport.h \
    dialogscheduler.h \
    quazaaglobals.h \
    dialoggplview.h \
    commonfunctions.h \
    dialoghashprogress.h \
    dialogtransferprogresstooltip.h \
    dialogpreviewprepare.h \
    librarybuilder.h \
    dialogclosetype.h
FORMS = mainwindow.ui \
    dialogwizard.ui \
    dialogabout.ui \
    dialogsettings.ui \
    dialogsplash.ui \
    dialogprofile.ui \
    dialogeditshares.ui \
    dialogcreatetorrent.ui \
    dialogopentorrent.ui \
    dialogtorrentproperties.ui \
    dialogdownloadproperties.ui \
    dialoglanguage.ui \
    dialogselectvisualisation.ui \
    dialogfiltersearch.ui \
    dialogdownloadmonitor.ui \
    dialogadddownload.ui \
    dialogaddrule.ui \
    dialogsecuritysubscriptions.ui \
    dialogaddsecuritysubscription.ui \
    dialogdownloadsimport.ui \
    dialogscheduler.ui \
    dialoggplview.ui \
    dialoghashprogress.ui \
    dialogtransferprogresstooltip.ui \
    dialogpreviewprepare.ui \
    dialogclosetype.ui
TRANSLATIONS = Language/af.ts \
    Language/ar.ts \
    Language/ca.ts \
    Language/chs.ts \
    Language/cz.ts \
    Language/de.ts \
    Language/ee.ts \
    Language/es.ts \
    Language/es-mx.ts \
    Language/fi.ts \
    Language/fr.ts \
    Language/gr.ts \
    Language/heb.ts \
    Language/hr.ts \
    Language/hu.ts \
    Language/it.ts \
    Language/ja.ts \
    Language/lt.ts \
    Language/nl.ts \
    Language/no.ts \
    Language/pl.ts \
    Language/pt-br.ts \
    Language/ru.ts \
    Language/sl-si.ts \
    Language/sq.ts \
    Language/sr.ts \
    Language/sv.ts \
    Language/tr.ts \
    Language/tw.ts
RESOURCES = Graphics.qrc
RC_FILE = Quazaa.rc
OTHER_FILES += LICENSE.GPL3
