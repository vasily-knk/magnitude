TEMPLATE = app
DEV_ROOT = $$(SIMLABS_REPO_DIR)

include($$DEV_ROOT/src/predefines.pri)

CONFIG -= qt
CONFIG += console

# LIBS += -llogger -lalloc -lani -lfms -lcrash_handler -lzlib -lprocess_state -ljson_io -lteamcity_support

PRECOMPILED_HEADER = stdafx.h

SOURCES += $$get_all_sources()
SOURCES -= stdafx.h.cpp

HEADERS += $$get_all_headers()

include($$DEV_ROOT/src/postdefines.pri)

