# specifying common dirs

unix {
    TEMPDIR         = ../../tmp/unix/$$TARGET
    macx:TEMPDIR    = ../../tmp/osx/$$TARGET
    QMAKE_CFLAGS    += -std=gnu99
}


DESTDIR         = ../../xbin
MOC_DIR         = $$TEMPDIR
OBJECTS_DIR     = $$TEMPDIR
RCC_DIR         = $$TEMPDIR
UI_DIR          = $$TEMPDIR/Ui
DEPENDPATH      += ../../xbin
INCLUDEPATH     +=  . ../../src

