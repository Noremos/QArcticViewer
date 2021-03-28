QT += qml quick 3dcore 3drender 3dinput 3dquick 3dquickextras

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../C++/Barcode/PrjBarlib/source/Barpy.cpp \
    ../../../C++/Barcode/PrjBarlib/source/barImg.cpp \
    ../../../C++/Barcode/PrjBarlib/source/barclasses.cpp \
    ../../../C++/Barcode/PrjBarlib/source/barcodeCreator.cpp \
    ../../../C++/Barcode/PrjBarlib/source/component.cpp \
    ../../../C++/Barcode/PrjBarlib/source/hole.cpp \
    backend.cpp \
    face3d.cpp \
    heimapmodel.cpp \
    imagesearcher.cpp \
    imgproxy.cpp \
    instansemodel.cpp \
    main.cpp \
    obj3d.cpp \
    project.cpp \
    tiffreader.cpp

RESOURCES += qml.qrc \
    objects.qrc \
    shaders.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =
# Default rules for deployment.

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin


#target.path = $${QT_INSTALL_EXAMPLES}/quick3d/view3d

!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    QML/BasicCamera.qml \
    QML/HeightMaterial.qml \
    QML/NewButton.qml \
    QML/SideMenu.qml \
    QML/SpotItems.qml \
    QML/SurfaceEntry.qml \
    QML/SurfaceMaterial.qml \
    QML/SurfaceScene.qml \
    QML/SpinFloat.qml \
    QML/main.qml \
    as.astylerc


INCLUDEPATH += D:/Progs/C++/Barcode/PrjBarlib/include
DEPENDPATH += D:/Progs/C++/Barcode/PrjBarlib/include
INCLUDEPATH += D:\Programs\C++\Barcode\PrjBarlib\include
DEPENDPATH += D:\Programs\C++\Barcode\PrjBarlib\include

win32:CONFIG(release, debug|release): LIBS += -LD:/Programs/QT/QArcticViewer/QArcticViewer/ -lopencv_world411
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Programs/QT/QArcticViewer/QArcticViewer/ -lopencv_world411d
else:unix: LIBS += -LD:/Programs/QT/QArcticViewer/QArcticViewer/ -lopencv_world411

HEADERS += \
    ../../../C++/Barcode/PrjBarlib/include/barImg.h \
    ../../../C++/Barcode/PrjBarlib/include/barclasses.h \
    ../../../C++/Barcode/PrjBarlib/include/barcodeCreator.h \
    ../../../C++/Barcode/PrjBarlib/include/barline.h \
    ../../../C++/Barcode/PrjBarlib/include/barstrucs.h \
    ../../../C++/Barcode/PrjBarlib/include/component.h \
    ../../../C++/Barcode/PrjBarlib/include/hole.h \
    ../../../C++/Barcode/PrjBarlib/include/include_cv.h \
    ../../../C++/Barcode/PrjBarlib/include/include_py.h \
    ../../../C++/Barcode/PrjBarlib/include/presets.h \
    Decoder.h \
    backend.h \
    base.h \
    face3d.h \
    heimapmodel.h \
    imagesearcher.h \
    imgproxy.h \
    instansemodel.h \
    obj3d.h \
    project.h \
    tiffreader.h

