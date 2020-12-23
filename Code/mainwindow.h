///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QUrl>
#include "window.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    QString fileName;
    QUrl filepath;

    QString projectPath="";

private slots:
    void open();
    void close();
    void save();
    void saveas();

    void featureManager();
    void dataGroups();
    void predefinedTypes();
private:
    void createActions();
    void createMenus();

    Window *c_window=NULL;
    QMenu *fileMenu;
    QMenu *featureMenu;
    QMenu *windowMenu;
    QAction *featureManagerAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;

    QAction *exitAct;

    QAction *predefinedTypesAct;
    QAction *dataGroupsAct;


    QMenu *selection;
    QAction *open_selection;
    QAction *save_selection;
    std::string filename;


};

#endif
