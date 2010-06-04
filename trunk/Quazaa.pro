# -------------------------------------------------
# Project created by QtCreator 2009-12-02T17:28:49
# -------------------------------------------------
QT += network \
phonon
TARGET = Quazaa
CONFIG(debug, debug|release):TARGET = $$join(TARGET,,,_debug)
INCLUDEPATH += vlc \
    NetworkCore \
	UI \
    3rdparty \
    3rdparty/libircclient-qt \
    Models \
    .
CONFIG(debug, debug|release):INCLUDEPATH += temp/debug
CONFIG(release):INCLUDEPATH += temp/release
win32:LIBS += -Lbin # if you are at windows os
LIBS += -lvlc
CONFIG += precompile_header \
	no_icu
DEFINES += IRC_STATIC \
	NO_DEPRECATED
PRECOMPILED_HEADER = NetworkCore/types.h
TEMPLATE = app
SOURCES += main.cpp \
    UI/dialoglanguage.cpp \
    quazaasettings.cpp \
    UI/dialogsplash.cpp \
    UI/widgetsearchtemplate.cpp \
    UI/dialogwizard.cpp \
    UI/dialogtransferprogresstooltip.cpp \
    UI/dialogtorrentproperties.cpp \
    UI/dialogselectvisualisation.cpp \
    UI/dialogsecuritysubscriptions.cpp \
    UI/dialogscheduler.cpp \
    UI/dialogprofile.cpp \
    UI/dialogpreviewprepare.cpp \
    UI/dialogopentorrent.cpp \
    UI/dialoghashprogress.cpp \
    UI/dialogfiltersearch.cpp \
    UI/dialogeditshares.cpp \
    UI/dialogdownloadsimport.cpp \
    UI/dialogdownloadproperties.cpp \
    UI/dialogdownloadmonitor.cpp \
    UI/dialogcreatetorrent.cpp \
    UI/dialogclosetype.cpp \
    UI/dialogaddsecuritysubscription.cpp \
    UI/dialogaddrule.cpp \
    UI/dialogadddownload.cpp \
    UI/dialogabout.cpp \
    UI/dialogsettings.cpp \
    commonfunctions.cpp \
    UI/dialoglibrarysearch.cpp \
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
    vlcmediaplayer.cpp \
    quazaairc.cpp \
    NetworkCore/ZLibUtils.cpp \
    NetworkCore/webcache.cpp \
    NetworkCore/types.cpp \
    NetworkCore/Thread.cpp \
    NetworkCore/SearchManagerr.cpp \
    NetworkCore/RouteTable.cpp \
    NetworkCore/RateController.cpp \
    NetworkCore/QueryHit.cpp \
    NetworkCore/queryhashtable.cpp \
    NetworkCore/Query.cpp \
    NetworkCore/parser.cpp \
    NetworkCore/NetworkConnection.cpp \
    NetworkCore/network.cpp \
    NetworkCore/ManagedSearch.cpp \
    NetworkCore/hostcache.cpp \
    NetworkCore/Handshakes.cpp \
    NetworkCore/Handshake.cpp \
    NetworkCore/g2packet.cpp \
    NetworkCore/g2node.cpp \
    NetworkCore/datagrams.cpp \
    NetworkCore/datagramfrags.cpp \
    NetworkCore/CompressedConnection.cpp \
    NetworkCore/Hashes/sha1.cpp \
    NetworkCore/Hashes/AbstractHash.cpp \
    UI/winmain.cpp \
    UI/widgethome.cpp \
    UI/widgetlibrary.cpp \
    UI/widgetmedia.cpp \
    UI/widgetlibraryview.cpp \
    UI/widgetsearch.cpp \
    UI/widgetsearchresults.cpp \
    UI/widgettransfers.cpp \
    UI/widgetdownloads.cpp \
    UI/widgetuploads.cpp \
    UI/widgetactivity.cpp \
    UI/widgetsecurity.cpp \
    UI/widgetsystemlog.cpp \
    UI/widgetneighbors.cpp \
    UI/widgetchat.cpp \
    UI/widgetchatcenter.cpp \
    UI/widgethostcache.cpp \
    UI/widgetdiscovery.cpp \
    UI/widgetscheduler.cpp \
    UI/widgetgraph.cpp \
    UI/widgetpacketdump.cpp \
    UI/widgetsearchmonitor.cpp \
    UI/widgethitmonitor.cpp \
    systemlog.cpp \
    3rdparty/libircclient-qt/ircutil.cpp \
    3rdparty/libircclient-qt/ircsession.cpp \
    3rdparty/libircclient-qt/ircdccsession.cpp \
    3rdparty/libircclient-qt/ircbuffer.cpp \
    3rdparty/libircclient-qt/irc.cpp \
    3rdparty/CyoEncode/CyoEncode.c \
    3rdparty/CyoEncode/CyoDecode.c \
    Models/NeighboursTableModel.cpp \
    Models/searchtreemodel.cpp \
    geoiplist.cpp
HEADERS += UI/dialoglanguage.h \
    quazaasettings.h \
    quazaaglobals.h \
    UI/dialogsplash.h \
    UI/widgetsearchtemplate.h \
    UI/dialogwizard.h \
    UI/dialogtransferprogresstooltip.h \
    UI/dialogtorrentproperties.h \
    UI/dialogsettings.h \
    UI/dialogselectvisualisation.h \
    UI/dialogsecuritysubscriptions.h \
    UI/dialogscheduler.h \
    UI/dialogprofile.h \
    UI/dialogpreviewprepare.h \
    UI/dialogopentorrent.h \
    UI/dialoghashprogress.h \
    UI/dialogfiltersearch.h \
    UI/dialogeditshares.h \
    UI/dialogdownloadsimport.h \
    UI/dialogdownloadproperties.h \
    UI/dialogdownloadmonitor.h \
    UI/dialogcreatetorrent.h \
    UI/dialogclosetype.h \
    UI/dialogaddsecuritysubscription.h \
    UI/dialogaddrule.h \
    UI/dialogadddownload.h \
    UI/dialogabout.h \
    commonfunctions.h \
    UI/dialoglibrarysearch.h \
    QSkinDialog/qskinsettings.h \
    QSkinDialog/qskindialog.h \
    QSkinDialog/dialogskinpreview.h \
    qtsingleapplication/src/qtsingleapplication.h \
    qtsingleapplication/src/qtlockedfile.h \
    qtsingleapplication/src/qtlocalpeer.h \
    qtsingleapplication/src/qtsinglecoreapplication.h \
    phononmediaplayer.h \
    vlcmediaplayer.h \
    quazaairc.h \
    NetworkCore/ZLibUtils.h \
    NetworkCore/webcache.h \
    NetworkCore/types.h \
    NetworkCore/Thread.h \
    NetworkCore/SearchManager.h \
    NetworkCore/RouteTable.h \
    NetworkCore/RateController.h \
    NetworkCore/QueryHit.h \
    NetworkCore/Query.h \
    NetworkCore/NetworkConnection.h \
    NetworkCore/network.h \
    NetworkCore/ManagedSearch.h \
    NetworkCore/hostcache.h \
    NetworkCore/Handshakes.h \
    NetworkCore/Handshake.h \
    NetworkCore/g2packet.h \
    NetworkCore/g2node.h \
    NetworkCore/datagrams.h \
    NetworkCore/datagramfrags.h \
    NetworkCore/CompressedConnection.h \
    NetworkCore/Hashes/sha1.h \
    NetworkCore/Hashes/AbstractHash.h \
    NetworkCore/queryhashtable.h \
    NetworkCore/parser.h \
    UI/winmain.h \
    UI/widgethome.h \
    UI/widgetlibrary.h \
    UI/widgetmedia.h \
    UI/widgetlibraryview.h \
    UI/widgetsearch.h \
    UI/widgetsearchresults.h \
    UI/widgettransfers.h \
    UI/widgetdownloads.h \
    UI/widgetuploads.h \
    UI/widgetactivity.h \
    UI/widgetsecurity.h \
    UI/widgetsystemlog.h \
    UI/widgetneighbors.h \
    UI/widgetchat.h \
    UI/widgetchatcenter.h \
    UI/widgethostcache.h \
    UI/widgetdiscovery.h \
    UI/widgetscheduler.h \
    UI/widgetgraph.h \
    UI/widgetpacketdump.h \
    UI/widgetsearchmonitor.h \
    UI/widgethitmonitor.h \
    systemlog.h \
    3rdparty/libircclient-qt/ircutil.h \
    3rdparty/libircclient-qt/ircsession_p.h \
    3rdparty/libircclient-qt/ircsession.h \
    3rdparty/libircclient-qt/ircglobal.h \
    3rdparty/libircclient-qt/ircdccsession.h \
    3rdparty/libircclient-qt/ircbuffer_p.h \
    3rdparty/libircclient-qt/ircbuffer.h \
    3rdparty/libircclient-qt/irc.h \
    3rdparty/CyoEncode/CyoEncode.h \
    3rdparty/CyoEncode/CyoDecode.h \
    Models/NeighboursTableModel.h \
    Models/searchtreemodel.h \
    geoiplist.h
FORMS += UI/dialoglanguage.ui \
    UI/dialogsplash.ui \
    UI/widgetsearchtemplate.ui \
    UI/dialogwizard.ui \
    UI/dialogtransferprogresstooltip.ui \
    UI/dialogtorrentproperties.ui \
    UI/dialogsettings.ui \
    UI/dialogselectvisualisation.ui \
    UI/dialogsecuritysubscriptions.ui \
    UI/dialogscheduler.ui \
    UI/dialogprofile.ui \
    UI/dialogpreviewprepare.ui \
    UI/dialogopentorrent.ui \
    UI/dialoghashprogress.ui \
    UI/dialogfiltersearch.ui \
    UI/dialogeditshares.ui \
    UI/dialogdownloadsimport.ui \
    UI/dialogdownloadproperties.ui \
    UI/dialogdownloadmonitor.ui \
    UI/dialogcreatetorrent.ui \
    UI/dialogclosetype.ui \
    UI/dialogaddsecuritysubscription.ui \
    UI/dialogaddrule.ui \
    UI/dialogadddownload.ui \
    UI/dialogabout.ui \
    UI/dialoglibrarysearch.ui \
    UI/mediasettings.ui \
    QSkinDialog/qskindialog.ui \
    QSkinDialog/dialogskinpreview.ui \
    UI/winmain.ui \
    UI/widgethome.ui \
    UI/widgetlibrary.ui \
    UI/widgetmedia.ui \
    UI/widgetlibraryview.ui \
    UI/widgetsearch.ui \
    UI/widgetsearchresults.ui \
    UI/widgettransfers.ui \
    UI/widgetdownloads.ui \
    UI/widgetuploads.ui \
    UI/widgetactivity.ui \
    UI/widgetsecurity.ui \
    UI/widgetsystemlog.ui \
    UI/widgetneighbors.ui \
    UI/widgetchat.ui \
    UI/widgetchatcenter.ui \
    UI/widgethostcache.ui \
    UI/widgetdiscovery.ui \
    UI/widgetscheduler.ui \
    UI/widgetgraph.ui \
    UI/widgetpacketdump.ui \
    UI/widgetsearchmonitor.ui \
    UI/widgethitmonitor.ui
TRANSLATIONS = Language/quazaa_af.ts \
    Language/quazaa_ar.ts \
    Language/quazaa_ca.ts \
    Language/quazaa_chs.ts \
    Language/quazaa_cz.ts \
    Language/quazaa_de.ts \
    Language/quazaa_default_en.ts \
    Language/quazaa_ee.ts \
    Language/quazaa_es.ts \
    Language/quazaa_fi.ts \
    Language/quazaa_fr.ts \
    Language/quazaa_gr.ts \
    Language/quazaa_heb.ts \
    Language/quazaa_hr.ts \
    Language/quazaa_hu.ts \
    Language/quazaa_it.ts \
    Language/quazaa_ja.ts \
    Language/quazaa_lt.ts \
    Language/quazaa_nl.ts \
    Language/quazaa_no.ts \
    Language/quazaa_pt.ts \
    Language/quazaa_pt-br.ts \
    Language/quazaa_ru.ts \
    Language/quazaa_sl-si.ts \
    Language/quazaa_sq.ts \
    Language/quazaa_sr.ts \
    Language/quazaa_sv.ts \
    Language/quazaa_tr.ts \
    Language/quazaa_tw.ts
RESOURCES += Resource.qrc
RC_FILE = Quazaa.rc
OTHER_FILES += LICENSE.GPL3
DESTDIR = ./bin
CONFIG(debug, debug|release):OBJECTS_DIR = temp/debug
CONFIG(release):OBJECTS_DIR = temp/release
CONFIG(debug, debug|release):MOC_DIR = temp/debug
CONFIG(release):MOC_DIR = temp/release
CONFIG(debug, debug|release):UI_DIR = temp/debug
CONFIG(release):UI_DIR = temp/release
