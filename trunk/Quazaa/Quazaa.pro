#
# Quazaa.pro
#
# Copyright © Quazaaa Development Team, 2009-2011.
# This file is part of QUAZAA (quazaa.sourceforge.net)
#
# Quazaa is free software; this file may be used under the terms of the GNU
# General Public License version 3.0 or later or later as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.
#
# Quazaa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# Please review the following information to ensure the GNU General Public
# License version 3.0 requirements will be met:
# http://www.gnu.org/copyleft/gpl.html.
#
# You should have received a copy of the GNU General Public License version
# 3.0 along with Quazaa; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

QT += network \
	sql \
	xml
TARGET = Quazaa

# Paths
DESTDIR = ./bin
CONFIG(debug, debug|release) {
	QT += testlib
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
	3rdparty/communi/include \
	3rdparty/icu \
	Models \
	ShareManager \
	Chat \
	Transfers \
	.

# Version stuff
MAJOR = 0
MINOR = 1
VERSION_HEADER = ./version.h

versiontarget.target = $$VERSION_HEADER
versiontarget.commands = ../VersionTool/debug/VersionTool $$MAJOR $$MINOR $$VERSION_HEADER
versiontarget.depends = FORCE

PRE_TARGETDEPS += $$VERSION_HEADER
QMAKE_EXTRA_TARGETS += versiontarget

# Append _debug to executable name when compiling using debug config
CONFIG(debug, debug|release):TARGET = $$join(TARGET,,,_debug)

# Additional config
win32:LIBS += -Lbin # if you are at windows os
mac:CONFIG -= app_bundle
DEFINES += COMMUNI_STATIC
CONFIG(debug, debug|release){
	DEFINES += _DEBUG
}
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

win32-msvc2010 {
	!build_pass:message(Setting up MSVC 2010 Compiler flags)
	QMAKE_CFLAGS_DEBUG += /Gd \
		/arch:SSE2 \
		/Gm \
		/RTC1 \
		/MDd \
		/Zi \
		/GS
	QMAKE_CFLAGS_RELEASE += /Gd \
		/arch:SSE2 \
		/GA \
		/Og \
		/MD
	QMAKE_CXXFLAGS_DEBUG += /Gd \
		/arch:SSE2 \
		/Gm \
		/RTC1 \
		/MDd \
		/Zi \
		/GS
	QMAKE_CXXFLAGS_RELEASE += /Gd \
		/arch:SSE2 \
		/GA \
		/Og \
		/MD 
	QMAKE_LFLAGS_DEBUG += /FIXED:NO
}

# Sources
SOURCES += \
	#Metalink/metalink4handler.cpp \
	#Metalink/metalinkhandler.cpp \
	3rdparty/CyoEncode/CyoDecode.c \
		3rdparty/CyoEncode/CyoEncode.c \
	Chat/chatconverter.cpp \
	Chat/chatcore.cpp \
	Chat/chatsession.cpp \
	Chat/chatsessiong2.cpp \
	commonfunctions.cpp \
	geoiplist.cpp \
	main.cpp \
	Models/chatroomslistmodel.cpp \
	Models/chatuserlistmodel.cpp \
	Models/neighbourstablemodel.cpp \
	Models/searchtreemodel.cpp \
	Models/securitytablemodel.cpp \
	Models/sharesnavigatortreemodel.cpp \
	NetworkCore/buffer.cpp \
	NetworkCore/compressedconnection.cpp \
	NetworkCore/datagramfrags.cpp \
	NetworkCore/datagrams.cpp \
	NetworkCore/endpoint.cpp \
	NetworkCore/g2node.cpp \
	NetworkCore/g2packet.cpp \
	NetworkCore/handshake.cpp \
	NetworkCore/handshakes.cpp \
	NetworkCore/Hashes/hash.cpp \
	NetworkCore/hostcache.cpp \
	NetworkCore/hubhorizon.cpp \
	NetworkCore/managedsearch.cpp \
	NetworkCore/neighbour.cpp \
	NetworkCore/neighbours.cpp \
	NetworkCore/neighboursbase.cpp \
	NetworkCore/neighboursconnections.cpp \
	NetworkCore/neighboursg2.cpp \
	NetworkCore/neighboursrouting.cpp \
	NetworkCore/network.cpp \
	NetworkCore/networkconnection.cpp \
	NetworkCore/parser.cpp \
	NetworkCore/query.cpp \
	NetworkCore/queryhashgroup.cpp \
	NetworkCore/queryhashmaster.cpp \
	NetworkCore/queryhashtable.cpp \
	NetworkCore/queryhit.cpp \
	NetworkCore/querykeys.cpp \
	NetworkCore/ratecontroller.cpp \
	NetworkCore/routetable.cpp \
	NetworkCore/searchmanager.cpp \
	NetworkCore/thread.cpp \
	NetworkCore/types.cpp \
	NetworkCore/webcache.cpp \
	NetworkCore/zlibutils.cpp \
	qtsingleapplication/src/qtlocalpeer.cpp \
	qtsingleapplication/src/qtlockedfile_unix.cpp \
	qtsingleapplication/src/qtlockedfile_win.cpp \
	qtsingleapplication/src/qtlockedfile.cpp \
	qtsingleapplication/src/qtsingleapplication.cpp \
	qtsingleapplication/src/qtsinglecoreapplication.cpp \
	quazaaglobals.cpp \
	quazaasettings.cpp \
	Security/securerule.cpp \
	Security/security.cpp \
	ShareManager/file.cpp \
	ShareManager/filehasher.cpp \
	ShareManager/sharedfile.cpp \
	ShareManager/sharemanager.cpp \
	systemlog.cpp \
	timedsignalqueue.cpp \
	Transfers/download.cpp \
	Transfers/downloads.cpp \
	Transfers/downloadsource.cpp \
	Transfers/filefragments.cpp \
	Transfers/transfers.cpp \
	UI/dialogabout.cpp \
	UI/dialogadddownload.cpp \
	UI/dialogaddrule.cpp \
	UI/dialogaddsecuritysubscription.cpp \
	UI/dialogclosetype.cpp \
	UI/dialogconnectto.cpp \
	UI/dialogcreatetorrent.cpp \
	UI/dialogdownloadmonitor.cpp \
	UI/dialogdownloadproperties.cpp \
	UI/dialogdownloadsimport.cpp \
	UI/dialogeditshares.cpp \
	UI/dialogfiltersearch.cpp \
	UI/dialoghashprogress.cpp \
	UI/dialogirccolordialog.cpp \
	UI/dialoglanguage.cpp \
	UI/dialoglibrarysearch.cpp \
	UI/dialogopentorrent.cpp \
	UI/dialogpreviewprepare.cpp \
	UI/dialogprivatemessages.cpp \
	UI/dialogprofile.cpp \
	UI/dialogscheduler.cpp \
	UI/dialogsecuritysubscriptions.cpp \
	UI/dialogselectvisualisation.cpp \
	UI/dialogsettings.cpp \
	UI/dialogsplash.cpp \
	UI/dialogtorrentproperties.cpp \
	UI/dialogtransferprogresstooltip.cpp \
	UI/widgetactivity.cpp \
	UI/widgetchat.cpp \
	UI/widgetchatinput.cpp \
	UI/widgetchatmiddle.cpp \
	UI/widgetchatroom.cpp \
	UI/widgetdiscovery.cpp \
	UI/widgetdownloads.cpp \
	UI/widgetgraph.cpp \
	UI/widgethitmonitor.cpp \
	UI/widgethome.cpp \
	UI/widgethostcache.cpp \
	UI/widgetlibrary.cpp \
	UI/widgetlibraryview.cpp \
	UI/widgetmedia.cpp \
	UI/widgetneighbours.cpp \
	UI/widgetpacketdump.cpp \
	UI/widgetprivatemessage.cpp \
	UI/widgetreturnemittextedit.cpp \
	UI/widgetscheduler.cpp \
	UI/widgetsearch.cpp \
	UI/widgetsearchmonitor.cpp \
	UI/widgetsearchresults.cpp \
	UI/widgetsearchtemplate.cpp \
	UI/widgetsecurity.cpp \
	UI/widgetsmileylist.cpp \
	UI/widgetsystemlog.cpp \
	UI/widgettransfers.cpp \
	UI/widgetuploads.cpp \
	UI/winmain.cpp \
	UI/wizardquickstart.cpp \
    3rdparty/communi/src/ircutil.cpp \
    3rdparty/communi/src/ircsession.cpp \
    3rdparty/communi/src/ircsender.cpp \
    3rdparty/communi/src/ircparser.cpp \
    3rdparty/communi/src/ircmessage.cpp \
    3rdparty/communi/src/ircencoder.cpp \
    3rdparty/communi/src/irccommand.cpp \
    3rdparty/communi/src/irc.cpp

HEADERS += \
	#Metalink/metalink4handler.h \
	#Metalink/metalinkhandler.h \
	3rdparty/CyoEncode/CyoDecode.h \
		3rdparty/CyoEncode/CyoEncode.h \
	Chat/chatconverter.h \
	Chat/chatcore.h \
	Chat/chatsession.h \
	Chat/chatsessiong2.h \
	commonfunctions.h \
	geoiplist.h \
	Models/chatroomslistmodel.h \
	Models/chatuserlistmodel.h \
	Models/neighbourstablemodel.h \
	Models/searchtreemodel.h \
	Models/securitytablemodel.h \
	Models/sharesnavigatortreemodel.h \
	NetworkCore/buffer.h \
	NetworkCore/compressedconnection.h \
	NetworkCore/datagramfrags.h \
	NetworkCore/datagrams.h \
	NetworkCore/endpoint.h \
	NetworkCore/g2node.h \
	NetworkCore/g2packet.h \
	NetworkCore/handshake.h \
	NetworkCore/handshakes.h \
	NetworkCore/Hashes/hash.h \
	NetworkCore/hostcache.h \
	NetworkCore/hubhorizon.h \
	NetworkCore/managedsearch.h \
	NetworkCore/neighbour.h \
	NetworkCore/neighbours.h \
	NetworkCore/neighboursbase.h \
	NetworkCore/neighboursconnections.h \
	NetworkCore/neighboursg2.h \
	NetworkCore/neighboursrouting.h \
	NetworkCore/network.h \
	NetworkCore/networkconnection.h \
	NetworkCore/parser.h \
	NetworkCore/query.h \
	NetworkCore/queryhashgroup.h \
	NetworkCore/queryhashmaster.h \
	NetworkCore/queryhashtable.h \
	NetworkCore/queryhit.h \
	NetworkCore/querykeys.h \
	NetworkCore/ratecontroller.h \
	NetworkCore/routetable.h \
	NetworkCore/searchmanager.h \
	NetworkCore/thread.h \
	NetworkCore/types.h \
	NetworkCore/types.h \
	NetworkCore/webcache.h \
	NetworkCore/zlibutils.h \
	qtsingleapplication/src/qtlocalpeer.h \
	qtsingleapplication/src/qtlockedfile.h \
	qtsingleapplication/src/qtsingleapplication.h \
	qtsingleapplication/src/qtsinglecoreapplication.h \
	quazaaglobals.h \
	quazaasettings.h \
	Security/securerule.h \
	Security/security.h \
	ShareManager/file.h \
	ShareManager/filehasher.h \
	ShareManager/sharedfile.h \
	ShareManager/sharemanager.h \
	systemlog.h \
	timedsignalqueue.h \
	Transfers/download.h \
	Transfers/downloads.h \
	Transfers/downloadsource.h \
	Transfers/filefragments.h \
	Transfers/transfers.h \
	UI/dialogabout.h \
	UI/dialogadddownload.h \
	UI/dialogaddrule.h \
	UI/dialogaddsecuritysubscription.h \
	UI/dialogclosetype.h \
	UI/dialogconnectto.h \
	UI/dialogcreatetorrent.h \
	UI/dialogdownloadmonitor.h \
	UI/dialogdownloadproperties.h \
	UI/dialogdownloadsimport.h \
	UI/dialogeditshares.h \
	UI/dialogfiltersearch.h \
	UI/dialoghashprogress.h \
	UI/dialogirccolordialog.h \
	UI/dialoglanguage.h \
	UI/dialoglibrarysearch.h \
	UI/dialogopentorrent.h \
	UI/dialogpreviewprepare.h \
	UI/dialogprivatemessages.h \
	UI/dialogprofile.h \
	UI/dialogscheduler.h \
	UI/dialogsecuritysubscriptions.h \
	UI/dialogselectvisualisation.h \
	UI/dialogsettings.h \
	UI/dialogsplash.h \
	UI/dialogtorrentproperties.h \
	UI/dialogtransferprogresstooltip.h \
	UI/widgetactivity.h \
	UI/widgetchat.h \
	UI/widgetchatinput.h \
	UI/widgetchatmiddle.h \
	UI/widgetchatroom.h \
	UI/widgetdiscovery.h \
	UI/widgetdownloads.h \
	UI/widgetgraph.h \
	UI/widgethitmonitor.h \
	UI/widgethome.h \
	UI/widgethostcache.h \
	UI/widgetlibrary.h \
	UI/widgetlibraryview.h \
	UI/widgetmedia.h \
	UI/widgetneighbours.h \
	UI/widgetpacketdump.h \
	UI/widgetprivatemessage.h \
	UI/widgetreturnemittextedit.h \
	UI/widgetscheduler.h \
	UI/widgetsearch.h \
	UI/widgetsearchmonitor.h \
	UI/widgetsearchresults.h \
	UI/widgetsearchtemplate.h \
	UI/widgetsecurity.h \
	UI/widgetsmileylist.h \
	UI/widgetsystemlog.h \
	UI/widgettransfers.h \
	UI/widgetuploads.h \
	UI/winmain.h \
	UI/wizardquickstart.h \
    3rdparty/communi/include/ircutil.h \
    3rdparty/communi/include/ircsession_p.h \
    3rdparty/communi/include/ircsession.h \
    3rdparty/communi/include/ircsender.h \
    3rdparty/communi/include/ircparser_p.h \
    3rdparty/communi/include/ircmessage.h \
    3rdparty/communi/include/ircglobal.h \
    3rdparty/communi/include/ircencoder_p.h \
    3rdparty/communi/include/irccommand.h \
    3rdparty/communi/include/irc.h

FORMS += \
	UI/dialogabout.ui \
	UI/dialogadddownload.ui \
	UI/dialogaddrule.ui \
	UI/dialogaddsecuritysubscription.ui \
	UI/dialogclosetype.ui \
	UI/dialogconnectto.ui \
	UI/dialogcreatetorrent.ui \
	UI/dialogdownloadmonitor.ui \
	UI/dialogdownloadproperties.ui \
	UI/dialogdownloadsimport.ui \
	UI/dialogeditshares.ui \
	UI/dialogfiltersearch.ui \
	UI/dialoghashprogress.ui \
	UI/dialogirccolordialog.ui \
	UI/dialoglanguage.ui \
	UI/dialoglibrarysearch.ui \
	UI/dialogopentorrent.ui \
	UI/dialogpreviewprepare.ui \
	UI/dialogprivatemessages.ui \
	UI/dialogprofile.ui \
	UI/dialogscheduler.ui \
	UI/dialogsecuritysubscriptions.ui \
	UI/dialogselectvisualisation.ui \
	UI/dialogsettings.ui \
	UI/dialogsplash.ui \
	UI/dialogtorrentproperties.ui \
	UI/dialogtransferprogresstooltip.ui \
	UI/mediasettings.ui \
	UI/widgetactivity.ui \
	UI/widgetchat.ui \
	UI/widgetchatinput.ui \
	UI/widgetchatmiddle.ui \
	UI/widgetchatroom.ui \
	UI/widgetdiscovery.ui \
	UI/widgetdownloads.ui \
	UI/widgetgraph.ui \
	UI/widgethitmonitor.ui \
	UI/widgethome.ui \
	UI/widgethostcache.ui \
	UI/widgetlibrary.ui \
	UI/widgetlibraryview.ui \
	UI/widgetmedia.ui \
	UI/widgetneighbours.ui \
	UI/widgetpacketdump.ui \
	UI/widgetprivatemessage.ui \
	UI/widgetscheduler.ui \
	UI/widgetsearch.ui \
	UI/widgetsearchmonitor.ui \
	UI/widgetsearchresults.ui \
	UI/widgetsearchtemplate.ui \
	UI/widgetsecurity.ui \
	UI/widgetsmileylist.ui \
	UI/widgetsystemlog.ui \
	UI/widgettransfers.ui \
	UI/widgetuploads.ui \
	UI/winmain.ui \
	UI/wizardquickstart.ui

TRANSLATIONS = \
	Language/quazaa_af.ts \
	Language/quazaa_ar.ts \
	Language/quazaa_ca.ts \
	Language/quazaa_chs.ts \
	Language/quazaa_cz.ts \
	Language/quazaa_de.ts \
	Language/quazaa_default_en.ts \
	Language/quazaa_ee.ts \
	Language/quazaa_es.ts \
	Language/quazaa_fa.ts \
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
	Language/quazaa_pt-br.ts \
	Language/quazaa_pt.ts \
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






