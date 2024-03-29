######################################################################
# Automatically generated by qmake (3.1) Tue Sep 26 09:21:47 2023
######################################################################

TEMPLATE = app
TARGET = szpuler
INCLUDEPATH += .

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QT +=   core \
        widgets \
        sql \
        serialport
        
CONFIG += debug

# Input
HEADERS +=  main.h \
            mainwindow.h \
            mdichild.h \
            datawindow.h \
            console.h \
            logger.h \
            logviewer.h \
            settingsdialog.h

SOURCES +=  main.cpp \
            mainwindow.cpp \
            mdichild.cpp \
            datawindow.cpp \
            console.cpp \
            logger.cpp \
            logviewer.cpp \
            settingsdialog.cpp
            
FORMS +=    settingsdialog.ui
            
            
RESOURCES += szpuler.qrc
