TARGET = hwkbd

CONFIG += link_pkgconfig
PKGCONFIG += mlite5

QT += dbus
QT -= gui

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

target.path = /usr/bin/

systemd.path = /usr/lib/systemd/user/
systemd.files = config/$${TARGET}.service

udevrule.path = /etc/udev/rules.d/
udevrule.files = config/95-$${TARGET}.rules

dbusconf.path = /etc/dbus-1/system.d/
dbusconf.files = config/$${TARGET}.conf

ambience.path = /usr/share/ambience/$${TARGET}
ambience.files = ambience/$${TARGET}.ambience

images.path = $${ambience.path}/images
images.files = ambience/images/*

vkblayout.path = /usr/share/maliit/plugins/com/jolla/layouts/
vkblayout.files = config/layouts/$${TARGET}.conf config/layouts/$${TARGET}.qml

INSTALLS += target systemd udevrule dbusconf ambience images vkblayout

message($${DEFINES})

SOURCES += \
    src/hwkbd-daemon.cpp \
    src/hwkeyboard.cpp \
    src/worker.cpp

HEADERS += \
    src/hwkeyboard.h \
    src/worker.h

OTHER_FILES += \
    rpm/$${TARGET}.spec \
    config/$${TARGET}.service \
    config/$${TARGET}.conf \
    config/layouts/$${TARGET}.conf \
    config/layouts/$${TARGET}.qml
