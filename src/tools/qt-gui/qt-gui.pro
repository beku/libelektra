QT += quick  gui core  qml  widgets testlib

HEADERS +=	$$files(src/*.hpp)\
			$$files(src/markdownconverter/*.h, true)\

SOURCES +=	$$files(src/*.cpp,true)\

CONFIG += qml_debug

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

RESOURCES += \
	resources.qrc
	i18n.qrc

OTHER_FILES +=	$$PWD/qml/*.qml\
				$$PWD/qml/*.js

SUBDIRS += \
	unittest/unittest.pro \

unix: LIBS += -L/usr/local/lib/ -lelektra
unix: LIBS += -L/usr/local/lib/ -lelektratools
unix: LIBS += -L/usr/local/lib/ -lmarkdown

INCLUDEPATH += /usr/local/include/elektra
INCLUDEPATH += src/markdownconverter
INCLUDEPATH += ../../libtools/include/
INCLUDEPATH += ../../include/
