#-------------------------------------------------
#
# Project created by QtCreator 2019-12-20T09:34:13
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mydict
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11 sdk_no_version_check

win32 {
    RC_FILE = resources/Icon.rc
}
unix:!mac {

}
freebsd {

}
mac {
    LIBS += -lz
    ICON = resources/Icon.icns
}

SOURCES += \
        dict/dict.cpp \
        dict/dictentry.cpp \
        dict/dictinfo.cpp \
        dict/dictdatabase.cpp \
        main.cpp \
        mainwindow.cpp \
        convert/dictconvert.cpp \
        convert/startdictconvert.cpp

HEADERS += \
        dict/dict.hpp \
        dict/dictentry.hpp \
        dict/dictinfo.hpp \
        dict/dictdatabase.hpp \
        mainwindow.hpp \
        convert/dictconvert.hpp \
        convert/startdictconvert.hpp

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    readme.md \
    resources/styles/style.qss

