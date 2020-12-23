///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include <QProcess>
#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>
#include "mainwindow.h"
#include <windows.h>
#include "DarkStyle.h"
#include "framelesswindow/framelesswindow.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    //app.setStyle(new DarkStyle);


    MainWindow *mainWindow;
    mainWindow = new MainWindow;
    mainWindow->setBaseSize(1600,800);     //resolution
    mainWindow->setMinimumSize(100,100); //resolution
    //mainWindow->setAttribute(Qt::WA_TranslucentBackground,true);
    //mainWindow->setWindowFlags(Qt::FramelessWindowHint);
    mainWindow->show();


//    FramelessWindow framelessWindow;
//    framelessWindow.setContent(mainWindow);
//    framelessWindow.setWindowTitle("DXPlorer: Dendritic Spine Analysis");
//    framelessWindow.show();
    return app.exec();
}
