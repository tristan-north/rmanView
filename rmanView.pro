QT       += widgets
CONFIG   += release
HEADERS   = src/mainwindow.h src/rman.h src/viewport.h src/timer.h
SOURCES   = src/main.cpp src/mainwindow.cpp src/rman.cpp src/viewport.cpp src/timer.cpp
INCLUDEPATH += ${RMANTREE}/include
LIBS += -L${RMANTREE}/lib -lprman
QMAKE_CXXFLAGS += -Wno-write-strings