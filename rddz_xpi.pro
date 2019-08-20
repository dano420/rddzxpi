#-------------------------------------------------
#
# Project created by QtCreator 2014-07-03T14:47:05
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RDDZ_XPI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    xidel.cpp

HEADERS  += mainwindow.h \
    xidel.h

FORMS    += mainwindow.ui \
    about.ui

TRANSLATIONS = rddzxpi_fr.ts

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    rddz_xpi.rc

win32: RC_FILE = rddz_xpi.rc
macx: ICON = rddzxpi.icns
