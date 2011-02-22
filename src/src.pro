#-------------------------------------------------
#
# Project created by QtCreator 2010-05-27T22:49:57
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += dbus maemo5

TARGET = fastsms
TEMPLATE = app
DEFINES = FASTSMS_APP


SOURCES += main.cpp\
    fastsms.cpp \
    dictionary.cpp \
    button.cpp \
    phonebook.cpp

HEADERS  += \
    fastsms.h \
    dictionary.h \
    button.h \
    phonebook.h \
    ui_dialogoptions.h

FORMS    +=

CONFIG += mobility
MOBILITY = messaging contacts

#symbian {
#    TARGET.UID3 = 0xe69eb78b
#    # TARGET.CAPABILITY +=
#    TARGET.EPOCSTACKSIZE = 0x14000
#    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
#}

unix {
    PREFIX = ../debian/fastsms
    BINDIR = $$PREFIX/opt/fastsms
    DATADIR = $$PREFIX/usr/share
    DEFINES += DATADIR=\"$$DATADIR\" \
        PKGDATADIR=\"$$PKGDATADIR\"

    # MAKE INSTALL
    INSTALLS += target dictionary desktop icon64 meta

    target.path = $$BINDIR
    dictionary.path = $$BINDIR/data
    dictionary.files += english-lite.idc
    dictionary.files += english-full.idc
    dictionary.files += french.idc
    dictionary.files += italian.idc
    dictionary.files += german.idc
    dictionary.files += spanish.idc
    dictionary.files += swedish.idc
    dictionary.files += basekey.txt
    desktop.path = $$DATADIR/applications/hildon
    desktop.files += fastsms.desktop
    icon64.path = $$BINDIR/icons/hicolor/64x64/apps
    icon64.files += fastsms.png
    meta.path = $$BINDIR/meta
    meta.files = application.install
}

OTHER_FILES += \
    inputdict.txt \
    changelog.txt \
    basekey.txt
