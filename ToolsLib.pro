#-------------------------------------------------
#
# Project created by QtCreator 2018-08-19T21:08:04
#
#-------------------------------------------------

QT       -= core gui

TARGET = ToolsLib
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# __STDC_INT64__
#CPPFLAGS= -std=gnu++0x
#APP_CPPFLAGS= -std=gnu++0x
#CPPFLAGS -= -Wunused-parameter

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CalcFunc.cpp \
    Errors.cpp \
    File_com.cpp \
    File2.cpp \
    File2com.cpp \
    FileTable.cpp \
    GetStr.cpp \
    MaaTL.cpp \
    MaaXml.cpp \
    md4c.cpp \
    md5.cpp \
    mthrlib.cpp \
    perm.cpp \
    Refs.cpp \
    resize_lim.cpp \
    sk2add.cpp \
    socket2.cpp \
    StringEx.cpp \
    StringLib.cpp \
    MaaEvents.cpp \
    Timer.cpp \
    wwwcgi/cgi_helper.cpp \
    wwwcgi/gd_helper.cpp

HEADERS += \
    Defs.h \
    toolslib.h \
    BTree.h \
    CalcFunc.h \
    cheap.h \
    Comm.h \
    console.h \
    err2.h \
    Errors.h \
    File2.h \
    FileTable.h \
    MaaTL.h \
    MaaXml.h \
    md4.h \
    md5.h \
    mthrlib.h \
    mthrlib2.h \
    os_sig.h \
    perm.h \
    rbtree.h \
    rbtree_book.h \
    Refs.h \
    resize_lim.h \
    sk2add.h \
    smartpt_.h \
    socket2.h \
    StringEx.h \
    StringLib.h \
    MaaEvents.h \
    temp.h \
    templates.h \
    Timer.h \
    toolslib.h \
    WIN32/Drv.h \
    WIN32/Maareg.h \
    WIN32/Maareg2.h \
    WIN32/Socket.h \
    WIN32/winerrors.h \
    wwwcgi/cgi_helper.h \
    wwwcgi/gd_helper.h \
    unix/errno.h \
    unix/errno-base.h \
    unix/unixerrors.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
win32 {
SOURCES += \
    Comm.cpp \
    Console.cpp \
    WIN32/Drv.cpp \
    WIN32/Maareg2.cpp \
    WIN32/Socket.cpp
}
DISTFILES +=
