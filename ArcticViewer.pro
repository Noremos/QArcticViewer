QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Skyboxgui.cpp \
    cubeg.cpp \
    main.cpp \
    mainwidget.cpp \
    mainwindow.cpp \
    src/core/imagesearcher.cpp \
    src/core/project.cpp \
    src/render/spotzones.cpp \
    terrarian/terrariangui.cpp \
    terrarian/textgui.cpp

HEADERS += \
    Skyboxgui.h \
    cubeg.h \
    mainwidget.h \
    mainwindow.h \
    src/base.h \
    src/core/decoder.h \
    src/core/face3d.h \
    src/core/imagesearcher.h \
    src/core/project.h \
    src/core/tiffreader.h \
    src/types/instinfo.h \
    src/types/types.h \
    terrarian/cameragui.h \
    src/render/spotzones.h \
    terrarian/terrariangui.h \
    terrarian/textgui.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    shaders.qrc \
    textures.qrc

