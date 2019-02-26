TEMPLATE = subdirs
DEV_ROOT = ..

include($$DEV_ROOT/src/predefines.pri)


CONFIG += ordered

SUBDIRS = \
    $$DEV_ROOT/simlabs-core/src/_include/_include_core.pro    \
    $$DEV_ROOT/simlabs-core/src/logger \
    hl_demo_processor \



include($$DEV_ROOT/src/postdefines.pri)
