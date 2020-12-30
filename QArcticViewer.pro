QT += qml quick 3dcore 3drender 3dinput 3dquick 3dquickextras

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../C++/Barcode/source/barcode.cpp \
    ../../../C++/Barcode/source/barcodeCreator.cpp \
    ../../../C++/Barcode/source/barcontainer.cpp \
    ../../../C++/Barcode/source/component.cpp \
    ../../../C++/Barcode/source/hole.cpp \
    ../../../C++/Barcode/source/point.cpp \
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


INCLUDEPATH += D:/MyA/Libraries/Include
INCLUDEPATH += ../../../C++/Barcode/include
DEPENDPATH += D:/MyA/Libraries/Include


win32:CONFIG(release, debug|release): LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411d
else:unix: LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411

HEADERS += \
    ../../../C++/Barcode/include/barcode.h \
    ../../../C++/Barcode/include/barcodeCreator.h \
    ../../../C++/Barcode/include/barcontainer.h \
    ../../../C++/Barcode/include/component.h \
    ../../../C++/Barcode/include/detectKnolls.h \
    ../../../C++/Barcode/include/hole.h \
    ../../../C++/Barcode/include/point.h \
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

