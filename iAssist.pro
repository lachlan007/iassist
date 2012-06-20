TEMPLATE = app
TARGET = iAssist
CONFIG += console
QT += core \
    gui \
    sql
HEADERS += UserDialog.h \
    SystemCommand.h \
    MissionParameterUI.h \
    Log.h \
    IAssist.h \
    DBMeasurementTable.h \
    DBMeasurementProfileTable.h \
    DBManagementUI.h \
    DBConnection.h \
    DBButtonTable.h \
    DBAreaTable.h \
    CollectUI.h \
    CollectThread.h \
    AutoProgramUI.h \
    AutoProgramThread.h \
    MeasurementProfile.h \
    Area.h \
    ButtonData.h \
    ButtonIO.h \
    Measurement.h \
    DataPlotUI.h \
    buttonIO/findtype.h \
    buttonIO/humutil.h \
    buttonIO/libusbds2490.h \
    buttonIO/mbappreg.h \
    buttonIO/mbee77.h \
    buttonIO/mbeprom.h \
    buttonIO/mbnv.h \
    buttonIO/mbnvcrc.h \
    buttonIO/mbscr.h \
    buttonIO/mbscrcrc.h \
    buttonIO/mbscree.h \
    buttonIO/mbscrex.h \
    buttonIO/mbscrx77.h \
    buttonIO/mbsha.h \
    buttonIO/ownet.h \
    buttonIO/pw77.h
SOURCES += UserDialog.cpp \
    SystemCommand.cpp \
    MissionParameterUI.cpp \
    Main.cpp \
    Log.cpp \
    IAssist.cpp \
    DBMeasurementTable.cpp \
    DBMeasurementProfileTable.cpp \
    DBManagementUI.cpp \
    DBConnection.cpp \
    DBButtonTable.cpp \
    DBAreaTable.cpp \
    CollectUI.cpp \
    CollectThread.cpp \
    AutoProgramUI.cpp \
    AutoProgramThread.cpp \
    MeasurementProfile.cpp \
    Area.cpp \
    ButtonData.cpp \
    ButtonIO.cpp \
    Measurement.cpp \
    DataPlotUI.cpp \
    buttonIO/crcutil.c \
    buttonIO/findtype.c \
    buttonIO/humutil.c \
    buttonIO/ioutil.c \
    buttonIO/libusbds2490.c \
    buttonIO/libusblnk.c \
    buttonIO/libusbnet.c \
    buttonIO/libusbses.c \
    buttonIO/libusbtran.c \
    buttonIO/mbappreg.c \
    buttonIO/mbee77.c \
    buttonIO/mbeprom.c \
    buttonIO/mbnv.c \
    buttonIO/mbnvcrc.c \
    buttonIO/mbscr.c \
    buttonIO/mbscrcrc.c \
    buttonIO/mbscree.c \
    buttonIO/mbscrex.c \
    buttonIO/mbscrx77.c \
    buttonIO/mbsha.c \
    buttonIO/owerr.c \
    buttonIO/pw77.c
FORMS += MissionParameterUI.ui \
    IAssist.ui \
    DBManagementUI.ui \
    CollectUI.ui \
    AutoProgramUI.ui \
    DataPlotUI.ui
LIBS += -L/usr/local/lib \
    -L/usr/lib \
    -lusb
linux-g++ {
LIBS += -lqwt-qt4
INCLUDEPATH += /usr/include/qwt-qt4
}
macx {
INCLUDEPATH += /opt/local/include \
        /opt/local/include/qwt
LIBS += -lqwt \
    -L/opt/local/lib
}
