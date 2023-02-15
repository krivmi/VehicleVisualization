# Include sample configurations.
include(../Samples.pri)

# Target name.
TARGET = Multidemo

# Target version.
VERSION = 0.1

# Build a library.
TEMPLATE = app

# Add header files.
HEADERS +=                      \
    src/cam.h \
    src/datahandler.h \
    src/denm.h \
    src/dialogs.h \
    src/eventcounter.h \
    src/gpstracker.h \
    src/logwidget.h \
    src/mainmapwindow.h \
    src/mapem.h \
    src/message.h \
    src/messageparser.h \
    src/processhandler.h \
    src/spatem.h \
    src/srem.h \
    src/trafficlightwidget.h \
    src/visualizer.h

# Add source files.
SOURCES +=                      \
    src/cam.cpp \
    src/datahandler.cpp \
    src/denm.cpp \
    src/dialogs.cpp \
    src/eventcounter.cpp \
    src/gpstracker.cpp \
    src/logwidget.cpp \
    src/main.cpp                \
    src/mainmapwindow.cpp \
    src/mapem.cpp \
    src/message.cpp \
    src/messageparser.cpp \
    src/processhandler.cpp \
    src/spatem.cpp \
    src/srem.cpp \
    src/trafficlightwidget.cpp \
    src/visualizer.cpp

# Add resource files.
RESOURCES +=                    \
    ../resources/Resources.qrc  \

LIBS += -lgps
