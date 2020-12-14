QT += qml quick 3dcore 3drender 3dinput 3dquick 3dquickextras

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#    ../../../C++/Barcode/source/barcode.cpp \
#    ../../../C++/Barcode/source/barcodeCreator.cpp \
#    ../../../C++/Barcode/source/barcontainer.cpp \
#    ../../../C++/Barcode/source/component.cpp \
#    ../../../C++/Barcode/source/hole.cpp \
#    ../../../C++/Barcode/source/point.cpp \
    backend.cpp \
    face3d.cpp \
    heimapmodel.cpp \
    imagesearcher.cpp \
    main.cpp \
    obj3d.cpp \
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
    QML/NewButton.qml \
    QML/SideMenu.qml \
    QML/SpotItems.qml \
    QML/SpotsEffect.qml \
    QML/SurfaceEffect.qml \
    QML/SurfaceEntry.qml \
    QML/SurfaceMaterial.qml \
    QML/SurfaceScene.qml \
    QML/main.qml \
    as.astylerc


INCLUDEPATH += D:/MyA/Libraries/Include
DEPENDPATH += D:/MyA/Libraries/Include


win32:CONFIG(release, debug|release): LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411d
else:unix: LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411

HEADERS += \
    Decoder.h \
    backend.h \
    base.h \
    face3d.h \
    heimapmodel.h \
    imagesearcher.h \
    obj3d.h \
    tiffreader.h


