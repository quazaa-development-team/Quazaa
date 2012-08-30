#-------------------------------------------------
#
# Project created by QtCreator 2011-08-24T23:32:17
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = VersionTool
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG(debug, debug|release) {
	DESTDIR = ./debug
}
else {
	DESTDIR = ./release
}

SOURCES += main.cpp
