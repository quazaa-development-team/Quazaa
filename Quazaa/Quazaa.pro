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

#check Qt version
QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

greaterThan(QT_VER_MAJ, 4) {
QT += widgets \
	gui \
	multimedia \
	multimediawidgets
}

QT += network \
	sql \
	xml

TARGET = Quazaa

# Paths
DESTDIR = ./bin
CONFIG(debug, debug|release) {
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
	3rdparty/nvwa \
        3rdparty/qtsingleapplication \
	Models \
	ShareManager \
	Chat \
	Transfers \
	Skin \
	FileFragments \
	Misc \
	Discovery \
	.

include(3rdparty/communi/examples/desktop/src/src.pri)
include(3rdparty/communi/examples/shared/shared.pri)
include(3rdparty/communi/src/3rdparty/mozilla/mozilla.pri)


# Version stuff
MAJOR = 0
MINOR = 1
VERSION_HEADER = version.h

versiontarget.target = $$VERSION_HEADER
CONFIG(debug, debug|release) {
	versiontarget.commands = ../VersionTool/debug/VersionTool $$MAJOR $$MINOR $$VERSION_HEADER
}
else {
	versiontarget.commands = ../VersionTool/release/VersionTool $$MAJOR $$MINOR $$VERSION_HEADER
}
win32-msvc* {
	versiontarget.commands = $$replace(versiontarget.commands, '/', '\\') # for nmake
}
versiontarget.depends = FORCE

PRE_TARGETDEPS += $$VERSION_HEADER
QMAKE_EXTRA_TARGETS += versiontarget

# Language stuff
isEmpty(QMAKE_LRELEASE) {
	win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\\lrelease.exe
	else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
updateqm.input = TRANSLATIONS
updateqm.output = $$DESTDIR/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$DESTDIR/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all

# Enable C++11 compilation and exception handling for mingw
win32-g++ {
        CONFIG += exceptions
}

# Append _debug to executable name when compiling using debug config
CONFIG(debug, debug|release):TARGET = $$join(TARGET,,,_debug)

# Additional config
win32 {
	LIBS += -Lbin -luser32 -lole32 -lshell32 # if you are at windows os
}
DEFINES += COMMUNI_STATIC
CONFIG(debug, debug|release){
	DEFINES += _DEBUG
	QT_FATAL_WARNINGS = 1
}

mac {
    LIBS += -lz
}
linux {
    LIBS += -lz -L/usr/lib
}
unix {
    LIBS += -lz -L/usr/lib
}


TEMPLATE = app

# MSVC-specific compiler flags
win32-msvc2008 {
	!build_pass:message(Setting up MSVC 2008 Compiler flags)
		QMAKE_CFLAGS_DEBUG += /Gd \
		/Gm \
		/RTC1
		QMAKE_CFLAGS_RELEASE += /Gd \
		/GA
		QMAKE_CXXFLAGS_DEBUG += /Gd \
		/Gm \
		/RTC1
		QMAKE_CXXFLAGS_RELEASE += /Gd \
		/GA
		QMAKE_LFLAGS_DEBUG += /FIXED:NO

		DEFINES += _CRT_SECURE_NO_WARNINGS
}

win32-msvc2010 {
	!build_pass:message(Setting up MSVC 2010 Compiler flags)
		QMAKE_CFLAGS_DEBUG += /Gd \
		#/Gm \
		/RTC1 \
		/MDd \
		/Zi \
		/GS \
		/MP
		QMAKE_CFLAGS_RELEASE += /Gd \
		/GA \
		/Og \
		/MD \
		/MP
		QMAKE_CXXFLAGS_DEBUG += /Gd \
		#/Gm \
		/RTC1 \
		/MDd \
		/Zi \
		/GS \
		/MP
		QMAKE_CXXFLAGS_RELEASE += /Gd \
		/GA \
		/MD \
		/MP
		QMAKE_LFLAGS_DEBUG += /FIXED:NO

		DEFINES += _CRT_SECURE_NO_WARNINGS
}

# use CONFIG-=sse2 to disable SSE2
sse2 {

	# MSVC
	win32-msvc* {
		!build_pass:message( "SSE2 Enabled")
		QMAKE_CFLAGS_DEBUG +=/arch:SSE2
		QMAKE_CFLAGS_RELEASE += /arch:SSE2
		QMAKE_CXXFLAGS_DEBUG += /arch:SSE2
		QMAKE_CXXFLAGS_RELEASE += /arch:SSE2
	}

}

# Sources
SOURCES += \
	3rdparty/communi/src/ircutil.cpp \
	3rdparty/communi/src/ircsession.cpp \
	3rdparty/communi/src/ircsender.cpp \
	3rdparty/communi/src/ircmessageparser.cpp \
	3rdparty/communi/src/ircmessagedecoder.cpp \
	3rdparty/communi/src/ircmessage.cpp \
	3rdparty/communi/src/irccommand.cpp \
	3rdparty/communi/src/irccodecplugin.cpp \
	3rdparty/communi/src/irc.cpp \
	3rdparty/communi/src/ircsessioninfo.cpp \
	3rdparty/communi/src/irctextformat.cpp \
	3rdparty/communi/src/ircpalette.cpp \
	3rdparty/CyoEncode/CyoDecode.c \
	3rdparty/CyoEncode/CyoEncode.c \
	3rdparty/nvwa/debug_new.cpp \
	Chat/chatconverter.cpp \
	Chat/chatcore.cpp \
	Chat/chatsession.cpp \
	Chat/chatsessiong2.cpp \
	commonfunctions.cpp \
	geoiplist.cpp \
	main.cpp \
	Misc/fileiconprovider.cpp \
	Misc/networkiconprovider.cpp \
	Misc/timedsignalqueue.cpp \
	Metalink/magnetlink.cpp \
	Metalink/metalinkhandler.cpp \
	Metalink/metalink4handler.cpp \
	Models/categorynavigatortreemodel.cpp \
	Models/downloadstreemodel.cpp \
	Models/ircuserlistmodel.cpp \
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
	quazaaglobals.cpp \
	quazaasettings.cpp \
	quazaasysinfo.cpp \
	Security/securerule.cpp \
	Security/security.cpp \
	ShareManager/file.cpp \
	ShareManager/filehasher.cpp \
	ShareManager/sharedfile.cpp \
	ShareManager/sharemanager.cpp \
	Skin/skinsettings.cpp \
	systemlog.cpp \
	Transfers/download.cpp \
	Transfers/downloads.cpp \
	Transfers/downloadsource.cpp \
	Transfers/downloadtransfer.cpp \
	Transfers/transfer.cpp \
	Transfers/transfers.cpp \
	UI/completerlineedit.cpp \
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
	UI/dialogircsettings.cpp \
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
	UI/suggestedlineedit.cpp \
	UI/tableview.cpp \
	UI/widgetactivity.cpp \
	UI/widgetchatinput.cpp \
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
	UI/wizardircconnection.cpp \
	UI/wizardquickstart.cpp

win32 {
	SOURCES += \
		3rdparty/zlib/zutil.c \
		3rdparty/zlib/uncompr.c \
		3rdparty/zlib/trees.c \
		3rdparty/zlib/inftrees.c \
		3rdparty/zlib/inflate.c \
		3rdparty/zlib/inffast.c \
		3rdparty/zlib/compress.c \
		3rdparty/zlib/infback.c \
		3rdparty/zlib/gzwrite.c \
		3rdparty/zlib/gzread.c \
		3rdparty/zlib/gzlib.c \
		3rdparty/zlib/gzclose.c \
		3rdparty/zlib/deflate.c \
		3rdparty/zlib/crc32.c \
                3rdparty/zlib/adler32.c
}

HEADERS += \
	3rdparty/communi/include/ircutil.h \
	3rdparty/communi/include/ircsession_p.h \
	3rdparty/communi/include/ircsession.h \
	3rdparty/communi/include/ircsender.h \
	3rdparty/communi/include/ircmessageparser_p.h \
	3rdparty/communi/include/ircmessagedecoder_p.h \
	3rdparty/communi/include/ircmessage.h \
	3rdparty/communi/include/ircglobal.h \
	3rdparty/communi/include/irccommand.h \
	3rdparty/communi/include/irccodecplugin.h \
	3rdparty/communi/include/irc.h \
	3rdparty/communi/include/ircsessioninfo.h \
	3rdparty/communi/include/irctextformat.h \
	3rdparty/communi/include/ircpalette.h \
	3rdparty/CyoEncode/CyoDecode.h \
	3rdparty/CyoEncode/CyoEncode.h \
	3rdparty/nvwa/debug_new.h \
	3rdparty/nvwa/fast_mutex.h \
	3rdparty/nvwa/static_assert.h \
	Chat/chatconverter.h \
	Chat/chatcore.h \
	Chat/chatsession.h \
	Chat/chatsessiong2.h \
	commonfunctions.h \
	FileFragments/Compatibility.hpp \
	FileFragments/Exception.hpp \
	FileFragments/FileFragments.hpp \
	FileFragments/List.hpp \
	FileFragments/Queue.hpp \
	FileFragments/Range.hpp \
	FileFragments/Ranges.hpp \
	geoiplist.h \
	Metalink/magnetlink.h \
	Metalink/metalinkhandler.h \
	Metalink/metalink4handler.h \
	Misc/fileiconprovider.h \
	Misc/networkiconprovider.h \
	Misc/timedsignalqueue.h \
	Misc/timeoutwritelocker.h \
	Models/categorynavigatortreemodel.h \
	Models/downloadstreemodel.h \
	Models/ircuserlistmodel.h \
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
	quazaaglobals.h \
	quazaasettings.h \
	quazaasysinfo.h \
	Security/securerule.h \
	Security/security.h \
	ShareManager/file.h \
	ShareManager/filehasher.h \
	ShareManager/sharedfile.h \
	ShareManager/sharemanager.h \
	Skin/skinsettings.h \
	systemlog.h \
	Transfers/download.h \
	Transfers/downloads.h \
	Transfers/downloadsource.h \
	Transfers/downloadtransfer.h \
	Transfers/transfer.h \
	Transfers/transfers.h \
	UI/completerlineedit.h \
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
	UI/dialogircsettings.h \
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
	UI/suggestedlineedit.h \
	UI/tableview.h \
	UI/widgetactivity.h \
	UI/widgetchatinput.h \
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
	UI/wizardircconnection.h \
	UI/wizardquickstart.h

win32 {
	HEADERS += \
		3rdparty/zlib/zutil.h \
		3rdparty/zlib/zlib.h \
		3rdparty/zlib/zconf.h \
		3rdparty/zlib/trees.h \
		3rdparty/zlib/inftrees.h \
		3rdparty/zlib/inflate.h \
		3rdparty/zlib/inffixed.h \
		3rdparty/zlib/inffast.h \
		3rdparty/zlib/gzguts.h \
		3rdparty/zlib/deflate.h \
                3rdparty/zlib/crc32.h
}

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
	UI/dialogircsettings.ui \
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
	UI/widgetchatinput.ui \
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
	UI/wizardquickstart.ui \
	UI/wizardircconnection.ui

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

mac {
	QM_DATA.path = Contents/MacOS
	for(translation, TRANSLATIONS) {
		translation ~= s/\\.ts$/.qm
		QM_DATA.files += $$DESTDIR/$$basename(translation)
	}
	QMAKE_BUNDLE_DATA += QM_DATA

	GEOIP_DATA.path = Contents/MacOS
	GEOIP_DATA.files += $$DESTDIR/GeoIP
	QMAKE_BUNDLE_DATA += GEOIP_DATA
}
