TEMPLATE = subdirs
DEV_ROOT = $$(SIMLABS_REPO_DIR)

include($$DEV_ROOT/src/predefines.pri)


CONFIG += ordered

SUBDIRS = $$DEV_ROOT/src/_Include    \
    hl_demo_processor \



include($$DEV_ROOT/src/postdefines.pri)
