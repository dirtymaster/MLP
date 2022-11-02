QT += core gui openglwidgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Controller/controller.cpp \
    ../Model/GraphModel/graph_network.cpp \
    ../Model/GraphModel/layer.cpp \
    ../Model/GraphModel/neuron.cpp \
    ../Model/MatrixModel/activate_function.cpp \
    ../Model/MatrixModel/matrix.cpp \
    ../Model/MatrixModel/matrix_network.cpp \
    form.cpp \
    mainwindow.cpp \
    msegraphwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    ../Controller/controller.h \
    ../Model/GraphModel/graph_network.h \
    ../Model/GraphModel/layer.h \
    ../Model/GraphModel/neuron.h \
    ../Model/MatrixModel/activate_function.h \
    ../Model/MatrixModel/matrix.h \
    ../Model/MatrixModel/matrix_network.h \
    ../Model/abstract_network.h \
    form.h \
    mainwindow.h \
    msegraphwindow.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui \
    msegraphwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

