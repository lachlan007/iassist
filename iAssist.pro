TEMPLATE = app
TARGET = iAssist
CONFIG += console
MOC_DIR = tmp/
OBJECTS_DIR = tmp/
UI_DIR = tmp/
QT += core \
    gui \
    sql
HEADERS += src/UserDialog.h \
    src/SystemCommand.h \
    src/MissionParameterUI.h \
    src/Log.h \
    src/IAssist.h \
    src/DBMeasurementTable.h \
    src/DBMeasurementProfileTable.h \
    src/DBManagementUI.h \
    src/DBConnection.h \
    src/DBButtonTable.h \
    src/DBAreaTable.h \
    src/CollectUI.h \
    src/CollectThread.h \
    src/AutoProgramUI.h \
    src/AutoProgramThread.h \
    src/MeasurementProfile.h \
    src/Area.h \
    src/ButtonData.h \
    src/ButtonIO.h \
    src/Measurement.h \
    src/DataPlotUI.h \
    src/buttonIO/findtype.h \
    src/buttonIO/humutil.h \
    src/buttonIO/libusbds2490.h \
    src/buttonIO/mbappreg.h \
    src/buttonIO/mbee77.h \
    src/buttonIO/mbeprom.h \
    src/buttonIO/mbnv.h \
    src/buttonIO/mbnvcrc.h \
    src/buttonIO/mbscr.h \
    src/buttonIO/mbscrcrc.h \
    src/buttonIO/mbscree.h \
    src/buttonIO/mbscrex.h \
    src/buttonIO/mbscrx77.h \
    src/buttonIO/mbsha.h \
    src/buttonIO/ownet.h \
    src/buttonIO/pw77.h
SOURCES += src/UserDialog.cpp \
    src/SystemCommand.cpp \
    src/MissionParameterUI.cpp \
    src/Main.cpp \
    src/Log.cpp \
    src/IAssist.cpp \
    src/DBMeasurementTable.cpp \
    src/DBMeasurementProfileTable.cpp \
    src/DBManagementUI.cpp \
    src/DBConnection.cpp \
    src/DBButtonTable.cpp \
    src/DBAreaTable.cpp \
    src/CollectUI.cpp \
    src/CollectThread.cpp \
    src/AutoProgramUI.cpp \
    src/AutoProgramThread.cpp \
    src/MeasurementProfile.cpp \
    src/Area.cpp \
    src/ButtonData.cpp \
    src/ButtonIO.cpp \
    src/Measurement.cpp \
    src/DataPlotUI.cpp \
    src/buttonIO/crcutil.c \
    src/buttonIO/findtype.c \
    src/buttonIO/humutil.c \
    src/buttonIO/ioutil.c \
    src/buttonIO/libusbds2490.c \
    src/buttonIO/libusblnk.c \
    src/buttonIO/libusbnet.c \
    src/buttonIO/libusbses.c \
    src/buttonIO/libusbtran.c \
    src/buttonIO/mbappreg.c \
    src/buttonIO/mbee77.c \
    src/buttonIO/mbeprom.c \
    src/buttonIO/mbnv.c \
    src/buttonIO/mbnvcrc.c \
    src/buttonIO/mbscr.c \
    src/buttonIO/mbscrcrc.c \
    src/buttonIO/mbscree.c \
    src/buttonIO/mbscrex.c \
    src/buttonIO/mbscrx77.c \
    src/buttonIO/mbsha.c \
    src/buttonIO/owerr.c \
    src/buttonIO/pw77.c
FORMS += src/widgets/MissionParameterUI.ui \
    src/widgets/IAssist.ui \
    src/widgets/DBManagementUI.ui \
    src/widgets/CollectUI.ui \
    src/widgets/AutoProgramUI.ui \
    src/widgets/DataPlotUI.ui
LIBS += -L/usr/local/lib \
    -L/usr/lib \
    -lusb \
	-lqwt
INCLUDEPATH += /usr/include/qwt
macx {
INCLUDEPATH += /opt/local/include \
        /opt/local/include/qwt
LIBS += -L/opt/local/lib
}
