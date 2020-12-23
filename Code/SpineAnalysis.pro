##JunYoung Choi
##juny0603@gmail.com
##Ulsan National Institute of Science and Technology(UNIST)

HEADERS       = glwidget.h \
                window.h \
                mainwindow.h \
                circularCoordinate.h \
                parallelCoordinate.h \
                tsneSelection.h \
                setFeatureUsing.h \
                seedSelectionWindow.h \
                colorBar.h \
                framelesswindow/framelesswindow.h \
                framelesswindow/windowdragger.h \
                DarkStyle.h \
                glwidget_for_data.h \
                tinytiffreader.h \
                variationGraph.h \
                typeChangeGraph.h \
                imageControlGraph.h \
                tinytiffwriter.h
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
                mainwindow.cpp \
                circularCoordinate.cpp \
                parallelCoordinate.cpp \
                tsneSelection.cpp \
                setFeatureUsing.cpp \
                seedSelectionWindow.cpp \
                colorBar.cpp \
                framelesswindow/framelesswindow.cpp \
                framelesswindow/windowdragger.cpp \
                DarkStyle.cpp \
                glwidget_for_data.cpp \
                tinytiffreader.cpp  \
                variationGraph.cpp \
                typeChangeGraph.cpp \
                imageControlGraph.cpp \
                tinytiffwriter.cpp

FORMS       +=  framelesswindow/framelesswindow.ui

INCLUDEPATH +="framelesswindow"

RESOURCES   += darkstyle.qrc \
               framelesswindow.qrc

QT           +=widgets
QT           +=gui


TARGET = SpineAnalysis
TEMPLATE = app
