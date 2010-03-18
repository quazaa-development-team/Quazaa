# -------------------------------------------------
# Project created by QtCreator 2009-12-02T17:28:49
# -------------------------------------------------
QT += network \
	sql \
	script \
	svg \
	webkit \
	xml \
	xmlpatterns \
	phonon
TARGET = Quazaa
CONFIG(debug, debug|release) {
	mac:TARGET = $$join(TARGET,,,_debug)
	win32:TARGET = $$join(TARGET,,,d)
}
INCLUDEPATH += vlcmediaplayer
win32:LIBS += -L./bin # if you are at windows os
LIBS += -lvlc
TEMPLATE = app
SOURCES += main.cpp \
	mainwindow.cpp \
	dialoglanguage.cpp \
	quazaasettings.cpp \
	dialogsplash.cpp \
	widgetsearchtemplate.cpp \
	dialogwizard.cpp \
	dialogtransferprogresstooltip.cpp \
	dialogtorrentproperties.cpp \
	dialogselectvisualisation.cpp \
	dialogsecuritysubscriptions.cpp \
	dialogscheduler.cpp \
	dialogprofile.cpp \
	dialogpreviewprepare.cpp \
	dialogopentorrent.cpp \
	dialoghashprogress.cpp \
	dialoggplview.cpp \
	dialogfiltersearch.cpp \
	dialogeditshares.cpp \
	dialogdownloadsimport.cpp \
	dialogdownloadproperties.cpp \
	dialogdownloadmonitor.cpp \
	dialogcreatetorrent.cpp \
	dialogclosetype.cpp \
	dialogaddsecuritysubscription.cpp \
	dialogaddrule.cpp \
	dialogadddownload.cpp \
	dialogabout.cpp \
	dialogsettings.cpp \
	commonfunctions.cpp \
	dialoglibrarysearch.cpp \
	QSkinDialog/qskinsettings.cpp \
	QSkinDialog/qskindialog.cpp \
	QSkinDialog/dialogskinpreview.cpp \
	qtsingleapplication/src/qtsingleapplication.cpp \
	qtsingleapplication/src/qtsinglecoreapplication.cpp \
	qtsingleapplication/src/qtlockedfile_win.cpp \
	qtsingleapplication/src/qtlockedfile_unix.cpp \
	qtsingleapplication/src/qtlockedfile.cpp \
	qtsingleapplication/src/qtlocalpeer.cpp \
	phononmediaplayer.cpp \
	vlcmediaplayer.cpp
HEADERS += mainwindow.h \
	dialoglanguage.h \
	quazaasettings.h \
	quazaaglobals.h \
	dialogsplash.h \
	widgetsearchtemplate.h \
	dialogwizard.h \
	dialogtransferprogresstooltip.h \
	dialogtorrentproperties.h \
	dialogsettings.h \
	dialogselectvisualisation.h \
	dialogsecuritysubscriptions.h \
	dialogscheduler.h \
	dialogprofile.h \
	dialogpreviewprepare.h \
	dialogopentorrent.h \
	dialoghashprogress.h \
	dialoggplview.h \
	dialogfiltersearch.h \
	dialogeditshares.h \
	dialogdownloadsimport.h \
	dialogdownloadproperties.h \
	dialogdownloadmonitor.h \
	dialogcreatetorrent.h \
	dialogclosetype.h \
	dialogaddsecuritysubscription.h \
	dialogaddrule.h \
	dialogadddownload.h \
	dialogabout.h \
	commonfunctions.h \
	dialoglibrarysearch.h \
	QSkinDialog/qskinsettings.h \
	QSkinDialog/qskindialog.h \
	QSkinDialog/dialogskinpreview.h \
	qtsingleapplication/src/qtsingleapplication.h \
	qtsingleapplication/src/qtlockedfile.h \
	qtsingleapplication/src/qtlocalpeer.h \
	qtsingleapplication/src/qtsinglecoreapplication.h \
	phononmediaplayer.h \
	vlcmediaplayer.h
FORMS += mainwindow.ui \
	dialoglanguage.ui \
	dialogsplash.ui \
	widgetsearchtemplate.ui \
	dialogwizard.ui \
	dialogtransferprogresstooltip.ui \
	dialogtorrentproperties.ui \
	dialogsettings.ui \
	dialogselectvisualisation.ui \
	dialogsecuritysubscriptions.ui \
	dialogscheduler.ui \
	dialogprofile.ui \
	dialogpreviewprepare.ui \
	dialogopentorrent.ui \
	dialoghashprogress.ui \
	dialoggplview.ui \
	dialogfiltersearch.ui \
	dialogeditshares.ui \
	dialogdownloadsimport.ui \
	dialogdownloadproperties.ui \
	dialogdownloadmonitor.ui \
	dialogcreatetorrent.ui \
	dialogclosetype.ui \
	dialogaddsecuritysubscription.ui \
	dialogaddrule.ui \
	dialogadddownload.ui \
	dialogabout.ui \
	dialoglibrarysearch.ui \
	mediasettings.ui \
	QSkinDialog/qskindialog.ui \
	QSkinDialog/dialogskinpreview.ui
 TRANSLATIONS = language/quazaa_af.ts \
	language/quazaa_ar.ts \
	language/quazaa_ca.ts \
	language/quazaa_chs.ts \
	language/quazaa_cz.ts \
	language/quazaa_de.ts \
	language/quazaa_ee.ts \
	language/quazaa_es.ts \
	language/quazaa_es-mx.ts \
	language/quazaa_fi.ts \
	language/quazaa_fr.ts \
	language/quazaa_gr.ts \
	language/quazaa_heb.ts \
	language/quazaa_hr.ts \
	language/quazaa_hu.ts \
	language/quazaa_it.ts \
	language/quazaa_ja.ts \
	language/quazaa_lt.ts \
	language/quazaa_nl.ts \
	language/quazaa_no.ts \
	language/quazaa_pt.ts \
	language/quazaa_pt-br.ts \
	language/quazaa_ru.ts \
	language/quazaa_sl-si.ts \
	language/quazaa_sq.ts \
	language/quazaa_sr.ts \
	language/quazaa_sv.ts \
	language/quazaa_tr.ts \
	language/quazaa_tw.ts
RESOURCES += Resource.qrc
RC_FILE = Quazaa.rc
OTHER_FILES += LICENSE.GPL3
DESTDIR = ./bin
