QT += quick datavisualization

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    backend.cpp \
    heimapmodel.cpp \
    main.cpp \
    tiffreader.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    QML/DataImage.qml \
    QML/HeiScene.qml \
    QML/NewButton.qml \
    QML/SideMenu.qml \
    QML/main.qml


INCLUDEPATH += D:/MyA/Libraries/Include
DEPENDPATH += D:/MyA/Libraries/Include

win32:CONFIG(release, debug|release): LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411d
else:unix: LIBS += -LD:/MyA/Libraries/Libs/ -lopencv_world411

HEADERS += \
    backend.h \
    heimapmodel.h \
    tiffreader.h
