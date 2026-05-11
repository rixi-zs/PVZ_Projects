QT += widgets multimedia
QT += multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bullet.cpp \
    conezombie.cpp \
    hungrygame.cpp \
    main.cpp \
    mainwindow.cpp \
    normalgame.cpp \
    peashooter.cpp \
    sun.cpp \
    sunflower.cpp \
    zombie.cpp

HEADERS += \
    bullet.h \
    conezombie.h \
    hungrygame.h \
    mainwindow.h \
    normalgame.h \
    peashooter.h \
    sun.h \
    sunflower.h \
    zombie.h

TRANSLATIONS += \
    PlantsVsZombies_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
