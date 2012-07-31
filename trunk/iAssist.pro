TEMPLATE = app
TARGET = iAssist
CONFIG += console
MOC_DIR = tmp/
OBJECTS_DIR = tmp/
UI_DIR = tmp/
QT += core \
    gui \
    sql
HEADERS += src/SystemCommand.h \
    src/Log.h \
    src/IAssist.h \
    src/CollectThread.h \
    src/AutoProgramThread.h \
    src/MeasurementProfile.h \
    src/ButtonData.h \
    src/ButtonIO.h \
    src/Measurement.h \
    src/MissionData.h \
    src/ThermochronButton.h \
    src/ThermoHygrochronButton.h \
    src/MissionParameterFile.h \
    src/database/DBMeasurementTable.h \
    src/database/DBMeasurementProfileTable.h \
    src/database/DBManagementUI.h \
    src/database/DBConnection.h \
    src/database/DBButtonTable.h \
    src/database/DBAreaTable.h \
    src/ui/UserDialog.h \
    src/ui/MissionParameterUI.h \
    src/ui/CollectUI.h \
    src/ui/AutoProgramUI.h \
    src/ui/DataPlotUI.h \
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
    src/buttonIO/pw77.h \
    src/buttonIO/thermo21.h
SOURCES += src/SystemCommand.cpp \
    src/Main.cpp \
    src/Log.cpp \
    src/IAssist.cpp \
    src/CollectThread.cpp \
    src/AutoProgramThread.cpp \
    src/MeasurementProfile.cpp \
    src/ButtonData.cpp \
    src/ButtonIO.cpp \
    src/Measurement.cpp \
    src/MissionData.cpp \
    src/ThermochronButton.cpp \
    src/ThermoHygrochronButton.cpp \
    src/MissionParameterFile.cpp \
    src/database/DBMeasurementTable.cpp \
    src/database/DBMeasurementProfileTable.cpp \
    src/database/DBManagementUI.cpp \
    src/database/DBConnection.cpp \
    src/database/DBButtonTable.cpp \
    src/database/DBAreaTable.cpp \
    src/ui/UserDialog.cpp \
    src/ui/MissionParameterUI.cpp \
    src/ui/CollectUI.cpp \
    src/ui/AutoProgramUI.cpp \
    src/ui/DataPlotUI.cpp \
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
    src/buttonIO/pw77.c \
    src/buttonIO/thermo21.c
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
