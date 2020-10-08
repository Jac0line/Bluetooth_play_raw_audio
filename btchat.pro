TEMPLATE = app
TARGET = btchat

QT = core bluetooth widgets multimedia
requires(qtConfig(listwidget))
android: QT += androidextras

SOURCES = \
    main.cpp \
    chat.cpp \
    remoteselector.cpp

HEADERS = \
    chat.h \
    remoteselector.h

FORMS = \
    chat.ui \
    remoteselector.ui

target.path = $$[QT_INSTALL_EXAMPLES]/bluetooth/btchat
INSTALLS += target
