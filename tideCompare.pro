#-------------------------------------------------
#
# Project created by QtCreator 2019-03-13T21:00:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
INCLUDEPATH+=D:\eigen-eigen-323c052e1731\Eigen
TARGET = tideCompare
TEMPLATE = app


SOURCES += main.cpp\
        tidecompare.cpp \
    qcustomplot.cpp

HEADERS  += tidecompare.h \
    qcustomplot.h

FORMS    += tidecompare.ui
