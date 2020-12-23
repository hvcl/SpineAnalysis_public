///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include <QtWidgets>

#include "mainwindow.h"
#include "window.h"
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QDockWidget>
#include <seedSelectionWindow.h>
#include <fstream>
MainWindow::MainWindow()
{
    setAttribute(Qt::WA_DeleteOnClose);
    createActions();
    createMenus();
    (void)statusBar();
//    setWindowFilePath(QString());

    c_window=new Window(this);
    QPalette pal = palette();

    // set black background
    pal.setColor(QPalette::Background, QColor(255,255,255));
    c_window->setAutoFillBackground(true);
    c_window->setPalette(pal);
    setCentralWidget(c_window);
    this->resize(1600,800);
}


void MainWindow::open()
{
    QString t=QFileDialog::getOpenFileName(this,tr("Open project file"),"",tr("*.DXplorer"));
    if(t=="")return;

    delete c_window;
    c_window=new Window(this);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255,255,255));
    c_window->setAutoFillBackground(true);
    c_window->setPalette(pal);
    setCentralWidget(c_window);

    projectPath=t;

    std::ifstream f;
    f.open(projectPath.toStdString().c_str());

    char tstr1[100];
    char tstr2[100];
    char tstr3[100];

    int tint1;
    int tint2;
    int tint3;

    // Group and data info

    f>>tint1;
    qDebug()<<tint1;
    f.ignore();
    for(int i=0;i<tint1;i++){
        c_window->handleGroupAdd();
        f.getline(tstr2,100);

        qDebug()<<tstr2;

        c_window->groups[i]->name->setText(QString(tstr2));
        f.getline(tstr2,100);

        qDebug()<<tstr2;

        c_window->groups[i]->color.setNamedColor(QString(tstr2));
        c_window->groups[i]->colorChange->setStyleSheet(QString("background-color: %1;foreground-color: %1; border-style: none;").arg(c_window->groups[i]->color.name()));
        f>>tint2;
        qDebug()<<tint2;
        f.ignore();
        for(int j=0;j<tint2;j++){
            f.getline(tstr2,100);

            qDebug()<<tstr2;
            QString tt=QString(tstr2);
            if(tt=="continue")continue;
            if(tt[tt.length()-1]=='/'){
                c_window->addDataSub(QString("dataset/")+QString(tstr2)+QString("dataset.DXplorerInfo"),i);
            }
            else{
                c_window->addDataSub(QString("dataset/")+QString(tstr2)+QString("/dataset.DXplorerInfo"),i);
            }
        }
    }

    // Image adjust info

    f>>tint1;
    qDebug()<<tint1;
    c_window->imageBrightControl->setValue(tint1);
    f>>tint1;
    qDebug()<<tint1;
    c_window->imageContrastControl->setValue(tint1);
    f>>tint1;
    qDebug()<<tint1;
    c_window->imageContrastPosControl->setValue(tint1);
    f>>tint1;
    qDebug()<<tint1;
    c_window->imageLabelOpacityControl->setValue(tint1);

    // pre post info
    f>>tint1;
    c_window->preGroup->setCurrentIndex(tint1);
    f>>tint1;
    c_window->postGroup->setCurrentIndex(tint1);


    float tfloat1,tfloat2,tfloat3,tfloat4;
    QList <int> tlist;
    // ui info
    f>>tfloat1>>tfloat2>>tfloat3;
    tlist<<this->width()*tfloat1<<this->width()*tfloat2<<this->width()*tfloat3;
    c_window->dataLayout1->setSizes(tlist);

    f>>tfloat1>>tfloat2>>tfloat3;
    tlist.clear();
    tlist<<this->width()*tfloat1<<this->width()*tfloat2<<this->width()*tfloat3;
    c_window->groupInfoLayout->setSizes(tlist);

    f>>tfloat1>>tfloat2;
    tlist.clear();
    tlist<<this->height()*tfloat1<<this->height()*tfloat2;
    c_window->Frame1->setSizes(tlist);

    f>>tint1;
    if(tint1==1)c_window->typeGroup->show();
    else c_window->typeGroup->hide();

    f>>tint1;
    if(tint1==1)c_window->datagroupGroup->show();
    else c_window->datagroupGroup->hide();


    // curvature info
    f>>tfloat1;
    c_window->colorbar->minV=tfloat1;
    f>>tfloat1;
    c_window->colorbar->maxV=tfloat1;
    f>>tint1;
    c_window->curvatureType->setCurrentIndex(tint1);


    // representative color setting
    f>>tint1;
    c_window->coloringType=tint1;
    c_window->coloringTypeSet->setCurrentIndex(tint1);

    // graph selection info
    f>>tint1;
    qDebug()<<tint1;

    for(int i=0;i<tint1;i++){
        f>>tint2;
        c_window->featureEnables[i]=tint2;
        f>>tint2;
        c_window->selection[i]=tint2;
        f>>tfloat1>>tfloat2;
        c_window->selectionRange[i]=QVector2D(tfloat1,tfloat2);
    }
    f>>tint1;
    qDebug()<<tint1;

    c_window->AreaSelection=tint1;
    f>>tfloat1>>tfloat2>>tfloat3>>tfloat4;
    c_window->selectionAreaStart=QVector2D(tfloat1,tfloat2);
    c_window->selectionAreaEnd=QVector2D(tfloat3,tfloat4);

    qDebug()<<tfloat1<<tfloat2<<tfloat3<<tfloat4;

    // projection info
    f>>tint1;
    c_window->projection->setCurrentIndex(tint1);
    f>>tint1;
    for(int i=0;i<tint1;i++){
        f>>tfloat1>>tfloat2;
        c_window->tsneResult[i]=QVector2D(tfloat1,tfloat2);
    }

    //enable info
    f>>tint1;
    for(int i=0;i<tint1;i++){
        f>>tint2;
        c_window->datagroupOnOff[i]->setChecked(tint2);
    }
    for(int i=0;i<5;i++){
        f>>tint1;
        c_window->typeOnOff[i]->setChecked(tint2);

    }


    // classification info
    for(int i=0;i<4;i++){
        f>>tint1;
        c_window->clusterOnOff[i]->setChecked(tint1);
    }
    f>>tint1;
    c_window->lookSelectedSpine->setChecked(tint1);
    f>>tint1;
    for(int i=0;i<tint1;i++){
        f>>tint2;
        c_window->indexOfSelectedSpines[i]=tint2;
        if(c_window->indexOfSelectedSpines[i]!=-1){
            c_window->selectedSpines[c_window->indexOfSelectedSpines[i]].push_back(i);
        }
    }
    for(int i=0;i<tint1;i++){
        f>>tint2;
        c_window->clusterLabel[i]=tint2;
    }
    f.ignore();
    for(int i=0;i<4;i++){
        f.getline(tstr1,100);

        QString t="";
        for(int j=1;;j++){
            if(tstr1[j]==' '){
                t+=QChar('\n');
            }
            else if(tstr1[j]==']'){
                break;
            }
            else t+=QChar(tstr1[j]);
        }
        c_window->constrains[i]=t;
        c_window->selectionWindow->tEdit[i]->setPlainText(c_window->constrains[i]);
    }
    c_window->seedChanged=true;

    int sa=0;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureNames[i]=="SA")sa=i;
    }

    int tint4,tint5;
    f>>tint1;
    for(int i=0;i<tint1;i++){
        f>>tint2>>tfloat1>>tint3>>tint4>>tint5;
        if(tint2)c_window->spines[i]->getNegativeCurvatureAreaLoad(QPoint(tint3,tint4),tint5);

    }

    f.close();

}

void MainWindow::save(){
    if(projectPath==""){
        saveas();
        return;
    }

    FILE *f=fopen(projectPath.toStdString().c_str(),"w");

    // Group and data info
    fprintf(f,"%d\n",c_window->groups.length());
    for(int i=0;i<c_window->groups.length();i++){
        fprintf(f,"%s\n",c_window->groups[i]->name->text().toStdString().c_str());
        fprintf(f,"%s\n",c_window->groups[i]->color.name().toStdString().c_str());
        fprintf(f,"%d\n",c_window->groups[i]->dataList->count());
        for(int j=0;j<c_window->groups[i]->dataList->count();j++){
            QString tname=c_window->groups[i]->dataList->item(j)->text();
            if(tname[tname.length()-4]=='.' && tname[tname.length()-3]=='t'
                    && tname[tname.length()-2]=='i' && tname[tname.length()-1]=='f'){
                fprintf(f,"continue\n");
                continue;
            }
            fprintf(f,"%s\n",tname.toStdString().c_str());
        }
    }

    // Image adjust info
    fprintf(f,"%d\n",c_window->imageBrightControl->value());
    fprintf(f,"%d\n",c_window->imageContrastControl->value());
    fprintf(f,"%d\n",c_window->imageContrastPosControl->value());
    fprintf(f,"%d\n",c_window->imageLabelOpacityControl->value());

    // pre post info
    fprintf(f,"%d\n",c_window->preGroup->currentIndex());
    fprintf(f,"%d\n",c_window->postGroup->currentIndex());


    // ui info
    fprintf(f,"%f %f %f\n",float(c_window->dataLayout1->sizes().at(0))/this->width(),float(c_window->dataLayout1->sizes().at(1))/this->width(),float(c_window->dataLayout1->sizes().at(2))/this->width());
    fprintf(f,"%f %f %f\n",float(c_window->groupInfoLayout->sizes().at(0))/this->width(),float(c_window->groupInfoLayout->sizes().at(1))/this->width(),float(c_window->groupInfoLayout->sizes().at(2))/this->width());
    fprintf(f,"%f %f\n",float(c_window->Frame1->sizes().at(0))/this->height(),float(c_window->Frame1->sizes().at(1))/this->height());
    fprintf(f,"%d\n",c_window->typeGroup->isHidden());
    fprintf(f,"%d\n",c_window->datagroupGroup->isHidden());

    // curvature info
    fprintf(f,"%f\n",c_window->colorbar->minV);
    fprintf(f,"%f\n",c_window->colorbar->maxV);
    fprintf(f,"%d\n",c_window->curvatureType->currentIndex());

    // representative color setting
    fprintf(f,"%d\n",c_window->coloringType);

    // graph selection info
    fprintf(f,"%d\n",c_window->featureNum);
    for(int i=0;i<c_window->featureNum;i++){
        fprintf(f,"%d\n",c_window->featureEnables[i]);
        fprintf(f,"%d\n",c_window->selection[i]);
        fprintf(f,"%f %f\n",c_window->selectionRange[i].x(),c_window->selectionRange[i].y());
    }
    fprintf(f,"%d\n",c_window->AreaSelection);
    fprintf(f,"%f %f %f %f\n",c_window->selectionAreaStart.x(),c_window->selectionAreaStart.y(),
            c_window->selectionAreaEnd.x(),c_window->selectionAreaEnd.y());


    // projection info
    fprintf(f,"%d\n",c_window->projection->currentIndex());
    fprintf(f,"%d\n",c_window->tsneResult.length());
    for(int i=0;i<c_window->tsneResult.length();i++){
        fprintf(f,"%f %f\n",c_window->tsneResult[i].x(),c_window->tsneResult[i].y());
    }


    //enable info
    fprintf(f,"%d\n",c_window->groups.length());
    for(int i=0;i<c_window->groups.length();i++){
        fprintf(f,"%d\n",c_window->datagroupOnOff[i]->isChecked());
    }

    for(int i=0;i<5;i++){
        fprintf(f,"%d\n",c_window->typeOnOff[i]->isChecked());
    }


    // classification info
    for(int i=0;i<4;i++){
        fprintf(f,"%d\n",c_window->clusterOnOff[i]->isChecked());
    }
    fprintf(f,"%d\n",c_window->lookSelectedSpine->isChecked());
    fprintf(f,"%d\n",c_window->data.length());
    for(int i=0;i<c_window->data.length();i++){
        fprintf(f,"%d\n",c_window->indexOfSelectedSpines[i]);
    }
    for(int i=0;i<c_window->data.length();i++){
        fprintf(f,"%d\n",c_window->clusterLabel[i]);
    }

    for(int i=0;i<4;i++){
        QString t="[";
        for(int j=0;j<c_window->constrains[i].length();j++){
            if(c_window->constrains[i][j]=='\n'){
                t+=' ';
            }
            else t+=c_window->constrains[i][j];
        }
        t+="]";

        fprintf(f,"%s\n",t.toStdString().c_str());
    }

    fprintf(f,"%d\n",c_window->data.length());
    for(int i=0;i<c_window->data.length();i++){
        fprintf(f,"%d %f %d %d %d\n",c_window->spines[i]->fixedNegativeCurvature,
                c_window->spines[i]->negativeCurvatureArea,
                c_window->spines[i]->fixedPos.x(),
                c_window->spines[i]->fixedPos.y(),
                c_window->spines[i]->fixedVert);
    }

    fclose(f);

    QMessageBox msgBox;
    msgBox.setStyle(new DarkStyle);
    msgBox.setText("Save complete");
    msgBox.exec();



}

void MainWindow::saveas(){

    projectPath=QFileDialog::getSaveFileName(this,tr("Save project file"),"untitled.DXplorer",tr("*.DXplorer"));
    if(projectPath!=""){
        save();
    }

}

void MainWindow::close()
{
//    setCentralWidget(NULL);
//    delete [] c_window;
//    setWindowFilePath(QString());
}
void MainWindow::featureManager(){
    c_window->featureUsageSetting();
}
void MainWindow::predefinedTypes(){
    if(c_window->typeGroup->isHidden()){
        c_window->typeGroup->show();
    }
    else{
        c_window->typeGroup->hide();
    }
}
void MainWindow::dataGroups(){
    if(c_window->datagroupGroup->isHidden()){
        c_window->datagroupGroup->show();
    }
    else{
        c_window->datagroupGroup->hide();
    }
}
void MainWindow::createActions()
{

    openAct = new QAction(tr("Load..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveas()));

    featureManagerAct = new QAction(tr("Open Manager..."), this);
    connect(featureManagerAct, SIGNAL(triggered()), this, SLOT(featureManager()));

    predefinedTypesAct = new QAction(tr("Pre-defined Types On/Off..."), this);
    connect(predefinedTypesAct, SIGNAL(triggered()), this, SLOT(predefinedTypes()));

    dataGroupsAct = new QAction(tr("Data Groups On/Off..."), this);
    connect(dataGroupsAct, SIGNAL(triggered()), this, SLOT(dataGroups()));


//    exitAct = new QAction(tr("Exit"), this);
//    exitAct->setShortcuts(QKeySequence::Quit);
//    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("Project"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);

    featureMenu= menuBar()->addMenu(tr("Feature"));
    featureMenu->addAction(featureManagerAct);

    windowMenu=menuBar()->addMenu(tr("Window"));
    windowMenu->addAction(predefinedTypesAct);
    windowMenu->addAction(dataGroupsAct);

}
