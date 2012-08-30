######################################################################
# Communi: wizard.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

FORMS += $$PWD/colorswizardpage.ui
FORMS += $$PWD/generalwizardpage.ui
FORMS += $$PWD/messageswizardpage.ui

HEADERS += $$PWD/colorswizardpage.h
HEADERS += $$PWD/generalwizardpage.h
HEADERS += $$PWD/messageswizardpage.h
HEADERS += $$PWD/settingswizard.h
HEADERS += $$PWD/treewidget.h

SOURCES += $$PWD/colorswizardpage.cpp
SOURCES += $$PWD/generalwizardpage.cpp
SOURCES += $$PWD/messageswizardpage.cpp
SOURCES += $$PWD/settingswizard.cpp
SOURCES += $$PWD/treewidget.cpp
