QT += widgets testlib

SOURCES = ../src/core/convertation.cpp \
    ../src/core/tiffreader.cpp \
    main.cpp

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
INSTALLS += target

HEADERS += \
    ../src/base.h \
    ../src/core/convertion.h \
    ../src/core/decoder.h \
    ../src/core/tiffreader.h


target.path = $$PWD/../side-src/$$TARGET
INSTALLS += target

INCLUDEPATH += $$PWD/../side-src
