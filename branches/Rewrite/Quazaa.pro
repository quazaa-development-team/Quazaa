# Quazaa is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3 of
# the License, or (at your option) any later version.
# Quazaa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with Quazaa; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# Qt modules used in application
QT += network \
    sql
TARGET = Quazaa

# Paths
DESTDIR = ./bin
CONFIG(debug) { 
    OBJECTS_DIR = temp/obj/debug
    RCC_DIR = temp/qrc/debug
}
else { 
    OBJECTS_DIR = temp/obj/release
    RCC_DIR = temp/qrc/release
}
MOC_DIR = temp/moc
UI_DIR = temp/uic
INCLUDEPATH += NetworkCore \
    UI \
    3rdparty \
    3rdparty/libircclient-qt/include \
    Models \
    ShareManager \
    Chat \
    .

# Append _debug to executable name when compiling using debug config
CONFIG(debug, debug|release):TARGET = $$join(TARGET,,,_debug)

# Additional config
win32:LIBS += -Lbin # if you are at windows os
mac:CONFIG -= app_bundle
CONFIG += no_icu
DEFINES += IRC_STATIC \
    IRC_NO_DEPRECATED
TEMPLATE = app

# MSVC-specific compiler flags
win32-msvc2008 { 
    !build_pass:message(Setting up MSVC 2008 Compiler flags)
    QMAKE_CFLAGS_DEBUG += /Gd \
        /arch:SSE2 \
        /Gm \
        /RTC1
    QMAKE_CFLAGS_RELEASE += /Gd \
        /arch:SSE2 \
        /GA
    QMAKE_CXXFLAGS_DEBUG += /Gd \
        /arch:SSE2 \
        /Gm \
        /RTC1
    QMAKE_CXXFLAGS_RELEASE += /Gd \
        /arch:SSE2 \
        /GA
    QMAKE_LFLAGS_DEBUG += /FIXED:NO
}

# Sources
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
    qtsingleapplication/src/qtsingleapplication.cpp \
    qtsingleapplication/src/qtsinglecoreapplication.cpp \
    qtsingleapplication/src/qtlockedfile_win.cpp \
    qtsingleapplication/src/qtlockedfile_unix.cpp \
    qtsingleapplication/src/qtlockedfile.cpp \
    qtsingleapplication/src/qtlocalpeer.cpp \
    quazaairc.cpp \
    NetworkCore/zlibutils.cpp \
    NetworkCore/webcache.cpp \
    NetworkCore/types.cpp \
    NetworkCore/thread.cpp \
    NetworkCore/searchmanager.cpp \
    NetworkCore/routetable.cpp \
    NetworkCore/ratecontroller.cpp \
    NetworkCore/queryhit.cpp \
    NetworkCore/queryhashtable.cpp \
    NetworkCore/query.cpp \
    NetworkCore/parser.cpp \
    NetworkCore/networkconnection.cpp \
    NetworkCore/network.cpp \
    NetworkCore/managedsearch.cpp \
    NetworkCore/hostcache.cpp \
    NetworkCore/handshakes.cpp \
    NetworkCore/handshake.cpp \
    NetworkCore/g2packet.cpp \
    NetworkCore/g2node.cpp \
    NetworkCore/datagrams.cpp \
    NetworkCore/datagramfrags.cpp \
    NetworkCore/compressedconnection.cpp \
    NetworkCore/Hashes/sha1.cpp \
    NetworkCore/Hashes/abstracthash.cpp \
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
    UI/widgetneighbours.cpp \
    UI/widgetchat.cpp \
    UI/widgetchatmiddle.cpp \
    UI/widgethostcache.cpp \
    UI/widgetdiscovery.cpp \
    UI/widgetscheduler.cpp \
    UI/widgetgraph.cpp \
    UI/widgetpacketdump.cpp \
    UI/widgetsearchmonitor.cpp \
    UI/widgethitmonitor.cpp \
    systemlog.cpp \
    3rdparty/CyoEncode/CyoEncode.c \
    3rdparty/CyoEncode/CyoDecode.c \
    Models/neighbourstablemodel.cpp \
    Models/searchtreemodel.cpp \
    geoiplist.cpp \
    UI/dialogconnectto.cpp \
    NetworkCore/Hashes/hash.cpp \
    ShareManager/ShareManager.cpp \
    ShareManager/SharedFile.cpp \
    ShareManager/FileHasher.cpp \
    NetworkCore/queryhashmaster.cpp \
    NetworkCore/queryhashgroup.cpp \
    Models/chatroomslistmodel.cpp \
    NetworkCore/neighbours.cpp \
    3rdparty/libircclient-qt/src/ircutil.cpp \
    3rdparty/libircclient-qt/src/ircsession.cpp \
    3rdparty/libircclient-qt/src/ircdccsession.cpp \
    3rdparty/libircclient-qt/src/ircbuffer.cpp \
    3rdparty/libircclient-qt/src/irc.cpp \
    UI/widgetchatroom.cpp \
    NetworkCore/buffer.cpp \
    NetworkCore/endpoint.cpp \
    NetworkCore/neighbour.cpp \
    Chat/chatcore.cpp \
    Chat/chatsession.cpp \
    UI/dialogprivatemessages.cpp \
    UI/widgetchatinput.cpp \
    UI/widgetprivatemessage.cpp \
    Chat/chatsessiong2.cpp \
    UI/widgetreturnemittextedit.cpp \
    UI/widgetsmileylist.cpp \
    Chat/chatconverter.cpp
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
    qtsingleapplication/src/qtsingleapplication.h \
    qtsingleapplication/src/qtlockedfile.h \
    qtsingleapplication/src/qtlocalpeer.h \
    qtsingleapplication/src/qtsinglecoreapplication.h \
    quazaairc.h \
    NetworkCore/zlibutils.h \
    NetworkCore/webcache.h \
    NetworkCore/types.h \
    NetworkCore/thread.h \
    NetworkCore/searchmanager.h \
    NetworkCore/routetable.h \
    NetworkCore/ratecontroller.h \
    NetworkCore/queryhit.h \
    NetworkCore/query.h \
    NetworkCore/networkconnection.h \
    NetworkCore/network.h \
    NetworkCore/managedsearch.h \
    NetworkCore/hostcache.h \
    NetworkCore/handshakes.h \
    NetworkCore/handshake.h \
    NetworkCore/g2packet.h \
    NetworkCore/g2node.h \
    NetworkCore/datagrams.h \
    NetworkCore/datagramfrags.h \
    NetworkCore/compressedconnection.h \
    NetworkCore/Hashes/sha1.h \
    NetworkCore/Hashes/abstracthash.h \
    NetworkCore/queryhashtable.h \
    NetworkCore/parser.h \
    NetworkCore/types.h \
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
    UI/widgetneighbours.h \
    UI/widgetchat.h \
    UI/widgetchatmiddle.h \
    UI/widgethostcache.h \
    UI/widgetdiscovery.h \
    UI/widgetscheduler.h \
    UI/widgetgraph.h \
    UI/widgetpacketdump.h \
    UI/widgetsearchmonitor.h \
    UI/widgethitmonitor.h \
    systemlog.h \
    Models/searchtreemodel.h \
    geoiplist.h \
    UI/dialogconnectto.h \
    NetworkCore/Hashes/hash.h \
    ShareManager/ShareManager.h \
    ShareManager/SharedFile.h \
    ShareManager/FileHasher.h \
    3rdparty/CyoEncode/CyoEncode.h \
    3rdparty/CyoEncode/CyoDecode.h \
    Models/neighbourstablemodel.h \
    NetworkCore/queryhashmaster.h \
    NetworkCore/queryhashgroup.h \
    Models/chatroomslistmodel.h \
    NetworkCore/neighbours.h \
    3rdparty/libircclient-qt/include/ircutil.h \
    3rdparty/libircclient-qt/include/ircsession_p.h \
    3rdparty/libircclient-qt/include/ircsession.h \
    3rdparty/libircclient-qt/include/ircglobal.h \
    3rdparty/libircclient-qt/include/ircdccsession.h \
    3rdparty/libircclient-qt/include/ircbuffer_p.h \
    3rdparty/libircclient-qt/include/ircbuffer.h \
    3rdparty/libircclient-qt/include/irc.h \
    UI/widgetchatroom.h \
    NetworkCore/buffer.h \
    NetworkCore/endpoint.h \
    NetworkCore/neighbour.h \
    Chat/chatcore.h \
    Chat/chatsession.h \
    UI/dialogprivatemessages.h \
    UI/widgetchatinput.h \
    UI/widgetprivatemessage.h \
    Chat/chatsessiong2.h \
    UI/widgetreturnemittextedit.h \
    UI/widgetsmileylist.h \
    Chat/chatconverter.h
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
    UI/widgetneighbours.ui \
    UI/widgetchat.ui \
    UI/widgetchatmiddle.ui \
    UI/widgethostcache.ui \
    UI/widgetdiscovery.ui \
    UI/widgetscheduler.ui \
    UI/widgetgraph.ui \
    UI/widgetpacketdump.ui \
    UI/widgetsearchmonitor.ui \
    UI/widgethitmonitor.ui \
    UI/dialogconnectto.ui \
    UI/widgetchatroom.ui \
    UI/dialogprivatemessages.ui \
    UI/widgetchatinput.ui \
    UI/widgetprivatemessage.ui \
    UI/widgetsmileylist.ui
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
    Language/quazaa_pl.ts \
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
