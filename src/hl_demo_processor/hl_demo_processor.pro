TEMPLATE = app
DEV_ROOT = ../..

include($$DEV_ROOT/src/predefines.pri)

CONFIG -= qt
CONFIG += console

LIBS += -llogger 

PRECOMPILED_HEADER = stdafx.h

SOURCES += $$get_all_sources()
SOURCES -= stdafx.h.cpp

HEADERS += $$get_all_headers()

include($$DEV_ROOT/src/postdefines.pri)

