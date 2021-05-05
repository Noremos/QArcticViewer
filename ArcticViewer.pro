QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Skyboxgui.cpp \
    cubeg.cpp \
    main.cpp \
    mainwidget.cpp \
    mainwindow.cpp \
    side-src/Barcode/PrjBarlib/source/barImg.cpp \
    side-src/Barcode/PrjBarlib/source/barclasses.cpp \
    side-src/Barcode/PrjBarlib/source/barcodeCreator.cpp \
    side-src/Barcode/PrjBarlib/source/component.cpp \
    side-src/Barcode/PrjBarlib/source/hole.cpp \
    src/core/imagesearcher.cpp \
    src/core/obj3d.cpp \
    src/core/project.cpp \
    src/core/tiffreader.cpp \
    src/render/spotzones.cpp \
    src/render/text2d.cpp \
    terrarian/terrariangui.cpp

HEADERS += \
    Skyboxgui.h \
    cubeg.h \
    mainwidget.h \
    mainwindow.h \
    src/base.h \
    src/core/decoder.h \
    src/core/face3d.h \
    src/core/imagesearcher.h \
    src/core/obj3d.h \
    src/core/project.h \
    src/core/tiffreader.h \
    src/render/text2d.h \
    src/types/img.h \
    src/types/instinfo.h \
    src/types/types.h \
    terrarian/cameragui.h \
    src/render/spotzones.h \
    terrarian/terrariangui.h

FORMS += \
    mainwindow.ui


INCLUDEPATH += side-src/Barcode/PrjBarlib/include
DEPENDPATH += side-src/Barcode/PrjBarlib/include
#INCLUDEPATH += D:\Install\Qt\5.15.2\Src\qtbase\src\3rdparty\freetype\include
#DEPENDPATH += D:\Install\Qt\5.15.2\Src\qtbase\src\3rdparty\freetype\include

#MSCV
#win32-msvc* {
#CONFIG += c++17

#INCLUDEPATH += $$PWD/side-src/opencv
#DEPENDPATH += $$PWD/side-src/opencv
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/side-src/opencv/ -lopencv_world411
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/side-src/opencv/ -lopencv_world411d
#else:unix: LIBS += -L$$PWD/side-src/opencv/ -lopencv_world411

#}


#MINGW
#win32-mingw*{
CONFIG += c++20


INCLUDEPATH += $$PWD/side-src/opencv-mingw
DEPENDPATH += $$PWD/side-src/opencv-mingw

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/side-src/opencv-mingw/opencv_core452.a
#else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/side-src/opencv-mingw/libopencv_core452.a

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/side-src/opencv-mingw/libopencv_imgproc452.a
#else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/side-src/opencv-mingw/libopencv_imgproc452.a
#}


RESOURCES += \
    baritems.qrc \
    shaders.qrc \
    textures.qrc


unix|win32: LIBS += -L$$PWD/side-src/opencv-mingw/ -llibopencv_world452.dll

INCLUDEPATH += $$PWD/side-src/opencv-mingw
DEPENDPATH += $$PWD/side-src/opencv-mingw

target.path = $$PWD/side-src/$$TARGET
INSTALLS += target
