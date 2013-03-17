OTHER_FILES += \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    qtc_packaging/debian_fremantle/prerm \
    qtc_packaging/debian_fremantle/postinst

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    colorconvert.cpp \
    bmpcheck.cpp \
    patchNolo.cpp

HEADERS += \
    mainwindow.h \
    colorconvert.h \
    bmpcheck.h \
    bmp.h

maemo5 {

image.files = \
    logo.original.bmp \
    usb.original.bmp \
    rd.original.bmp
image.path = /opt/n900-bootlogo-changer
INSTALLS += image

script.files = \
    flashnolo.sh
script.path = /opt/n900-bootlogo-changer
INSTALLS += script

desktop.files = \
    bootlogochanger.desktop
desktop.path = /usr/share/applications/hildon
INSTALLS += desktop

icon.files = \
    bootlogochanger.png
icon.path = /usr/share/icons/hicolor/64x64/apps
INSTALLS += icon

    target.path = /opt/n900-bootlogo-changer/bin
    INSTALLS += target
}
