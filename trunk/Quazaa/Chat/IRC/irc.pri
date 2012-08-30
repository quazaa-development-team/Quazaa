######################################################################
# Communi: irc.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

FORMS += $$PWD/messageview.ui

HEADERS += $$PWD/homepage.h
HEADERS += $$PWD/welcomepage.h
HEADERS += $$PWD/lineeditor.h
HEADERS += $$PWD/maintabwidget.h
HEADERS += $$PWD/messageview.h
HEADERS += $$PWD/searcheditor.h
HEADERS += $$PWD/sessiontabwidget.h

SOURCES += $$PWD/homepage.cpp
SOURCES += $$PWD/welcomepage.cpp
SOURCES += $$PWD/lineeditor.cpp
SOURCES += $$PWD/maintabwidget.cpp
SOURCES += $$PWD/messageview.cpp
SOURCES += $$PWD/searcheditor.cpp
SOURCES += $$PWD/sessiontabwidget.cpp

include(3rdparty/3rdparty.pri)
include(shared/shared.pri)
include(util/util.pri)
include(wizard/wizard.pri)




