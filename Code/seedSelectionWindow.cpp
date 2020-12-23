///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "seedSelectionWindow.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QPainter>
#include <QtGui>
#include <QDockWidget>
#include <QColorDialog>

#include <window.h>
#include <iostream>
#include <QDebug>
#include <QCheckBox>
#include <DarkStyle.h>


seedSelectionWindow::seedSelectionWindow(Window *p, QWidget *parent)
    : QWidget(parent)
{


//    setStyleSheet("QWidget {font: bold;}");
    c_window=p;

    groupN=c_window->GroupNumSet->value();
//    QHBoxLayout *groupNumSetLayout=new QHBoxLayout;
//    QLabel *groupNumLabel=new QLabel("Group Number: ");
//    groupNum=new QSpinBox;
//    groupNum->setValue(c_window->GroupNumSet->value());
//    groupNum->setMinimum(c_window->GroupNumSet->value());
//    groupNum->setMaximum(c_window->GroupNumSet->value());
//    groupNumSetLayout->addWidget(groupNumLabel);
//    groupNumSetLayout->addWidget(groupNum);




    QHBoxLayout *curSelectionLayout=new QHBoxLayout;
    QLabel *curSelectionLabel=new QLabel("Working Group: ");
    curSelectGroup=new QComboBox;
    curSelectGroup->setStyle(new DarkStyle);
    curSelectGroup->addItem("Irregular");
    curSelectGroup->addItem("Mushroom");
    curSelectGroup->addItem("Stubby");
    curSelectGroup->addItem("Thin");

    curSelectGroup->setCurrentIndex(c_window->currentSelectGroup);
//    curSelectGroup->setMinimum(0);
//    curSelectGroup->setMaximum(groupN-1);
    curSelectionLayout->addWidget(curSelectionLabel);
    curSelectionLayout->addWidget(curSelectGroup);

    connect(curSelectGroup,SIGNAL(currentIndexChanged(int)),this,SLOT(changeSelectionGroup(int)));

    addAllSelectedSpines=new QPushButton("Add Enabled Spines");
    connect(addAllSelectedSpines,SIGNAL(pressed()),this,SLOT(pushAddAllButton()));
    addAllSelectedSpines->setStyle(new DarkStyle);

    listLayout=new QHBoxLayout;
    for(int i=0;i<15;i++){
        char t[10];
        sprintf(t,"Group %d",i);
        groupBox[i]=new QGroupBox(QString(t));
        groupBox[i]->setStyleSheet("font: bold;");
        spineList[i]=new QListWidget;
//        for(int j=0;j<c_window->selectedSpines[i].length();j++){
//            spineList[i]->addItem(c_window->spineNames[c_window->selectedSpines[i][j]]);
//        }
        spineList[i]->setMouseTracking(true);
        connect(spineList[i],SIGNAL(itemEntered(QListWidgetItem*)),this,SLOT(focusItem(QListWidgetItem*)));
        connect(spineList[i],SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(clickItem(QListWidgetItem*)));
        spineList[i]->setSelectionMode(QAbstractItemView::ExtendedSelection);
        spineList[i]->setStyleSheet("font: bold; font-size: 10px;");

        deleteSpine[i]=new QPushButton("Delete");
        connect(deleteSpine[i],SIGNAL(pressed()),this,SLOT(pushDeleteButton()));
        deleteSpine[i]->setStyle(new DarkStyle);

        tEdit[i]=new QPlainTextEdit(c_window->constrains[i]);
        connect(tEdit[i],SIGNAL(textChanged()),this,SLOT(constrainChanged()));
        tEdit[i]->setStyleSheet("font: bold; font-size: 10px;");

        down[i]=new QPushButton;
        down[i]->setIcon(QIcon("Resource/down.png"));
        down[i]->setFixedSize(30*c_window->WINDOW_SCALE,15*c_window->WINDOW_SCALE);
        down[i]->setFlat(true);
        up[i]=new QPushButton;
        up[i]->setIcon(QIcon("Resource/up.png"));
        up[i]->setFixedSize(30*c_window->WINDOW_SCALE,10*c_window->WINDOW_SCALE);
        up[i]->setIconSize(QSize(30*c_window->WINDOW_SCALE,10*c_window->WINDOW_SCALE));
        up[i]->setFlat(true);

        connect(down[i],SIGNAL(pressed()),this,SLOT(makeStandard()));
        connect(up[i],SIGNAL(pressed()),this,SLOT(makeSelection()));

        QHBoxLayout *middle=new QHBoxLayout;
        //middle->addWidget(down[i]);
        middle->addWidget(up[i]);
        middle->setSpacing(0);
        middle->setMargin(0);

        QVBoxLayout *t_layout=new QVBoxLayout;
        //t_layout->setMargin(0);
        t_layout->setSpacing(0);
        t_layout->setContentsMargins(10,0,10,0);
        t_layout->addSpacing(8);
        t_layout->addWidget(spineList[i]);
        t_layout->addLayout(middle);
        t_layout->addWidget(tEdit[i]);
        t_layout->addSpacing(8);
        t_layout->addWidget(deleteSpine[i]);
        t_layout->addSpacing(8);

        groupBox[i]->setLayout(t_layout);

    }
    for(int i=0;i<groupN;i++){

        listLayout->addWidget(groupBox[i]);
    }



     groupBox[0]->setTitle("Irregular");
     groupBox[1]->setTitle("Mushroom");
     groupBox[2]->setTitle("Stubby");
     groupBox[3]->setTitle("Thin");

    c_window->lookSelectedSpine=new QCheckBox("Display Selected Spines");
    c_window->lookSelectedSpine->setChecked(false);
    connect(c_window->lookSelectedSpine,SIGNAL(clicked(bool)),c_window,SLOT(runCheckSpineEnable(bool)));

    QHBoxLayout *tFrame=new QHBoxLayout;
    tFrame->addWidget(c_window->lookSelectedSpine);
    tFrame->addWidget(addAllSelectedSpines);

    QVBoxLayout *wholeFrame=new QVBoxLayout;

//    wholeFrame->addLayout(groupNumSetLayout);

    wholeFrame->addLayout(curSelectionLayout);
    wholeFrame->addLayout(tFrame);

    wholeFrame->addLayout(listLayout);

    setLayout(wholeFrame);


    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);


    setWindowTitle("Selection Manager");


}

void seedSelectionWindow::makeSelection(){
    int curGroup=-1;
    for(int i=0;i<groupN;i++){
        if(up[i]->underMouse()){
            curGroup=i;
            break;
        }
    }
    if(curGroup==-1)return;
    if(c_window->constrains[curGroup]=="")return;


    int len=spineList[curGroup]->count();
    for(int i=len-1;i>=0;i--){
        QListWidgetItem *t=spineList[curGroup]->item(i);
        spineList[curGroup]->removeItemWidget(t);
        delete t;
        int spineNum=c_window->selectedSpines[curGroup].at(i);
        c_window->deleteSpineFromGroup(spineNum,curGroup);
    }


    for(int i=0;i<c_window->data.length();i++){
        if(c_window->checkConstraint(curGroup,i)){
            c_window->addSpineToGroup(i,curGroup);
        }
    }
    update();
}
void seedSelectionWindow::makeStandard(){
    int curGroup=-1;
    for(int i=0;i<groupN;i++){
        if(down[i]->underMouse()){
            curGroup=i;
            break;
        }
    }
    if(curGroup==-1)return;
    doMakeStandard(curGroup);



}


void seedSelectionWindow::doMakeStandard(int g){
    int curGroup=g;
    c_window->constrains[curGroup]="";
    if(c_window->selectedSpines[curGroup].size()==0)return;


    float score[20]; //for cur group and each feature
    float var[20]; //for each group and each feature
    float mean[10][20]; //for each group and each feature
    for(int n=0;n<groupN;n++){
        for(int i=0;i<c_window->featureNum;i++){
            score[i]=0;
            var[i]=0;
            mean[n][i]=0;
        }
    }


    for(int n=0;n<groupN;n++){
        if(c_window->selectedSpines[n].size()==0)continue;
        for(int i=0;i<c_window->featureNum;i++){
            for(int k=0;k<c_window->selectedSpines[n].size();k++){
                mean[n][i]+=(c_window->data[c_window->selectedSpines[n][k]][i]-c_window->featureRanges[i].x())/(c_window->featureRanges[i].y()-c_window->featureRanges[i].x());
            }
            mean[n][i]/=c_window->selectedSpines[n].size();
            qDebug()<<"mean "<<n<<":"<<i<<"="<<mean[n][i];
        }
    }

    for(int i=0;i<c_window->featureNum;i++){
        for(int k=0;k<c_window->selectedSpines[curGroup].size();k++){
            float t=mean[curGroup][i]-(c_window->data[c_window->selectedSpines[curGroup][k]][i]-c_window->featureRanges[i].x())/(c_window->featureRanges[i].y()-c_window->featureRanges[i].x());
            t*=t;
            var[i]+=t;
        }
        var[i]/=c_window->selectedSpines[curGroup].size();
        qDebug()<<"var "<<curGroup<<":"<<i<<"="<<var[i];

    }

    for(int i=0;i<c_window->featureNum;i++){
        score[i]+=-var[i];

        for(int n=0;n<groupN;n++){
            if(n==curGroup)continue;
            if(c_window->selectedSpines[n].size()==0)continue;


            float t=mean[curGroup][i]-mean[n][i];
            score[i]+=t*t;
        }
        qDebug()<<"score "<<c_window->featureNames[i]<<"="<<score[i];
    }

    bool ck[20];
    for(int i=0;i<c_window->featureNum;i++){
        ck[i]=true;
    }

    for(int k=0;k<3;k++){
        float maxScore=0;
        int index=-1;
        for(int i=0;i<c_window->featureNum;i++){
            if(ck[i]){
                if(maxScore<score[i]){
                    maxScore=score[i];
                    index=i;
                }
            }
        }
        if(index==-1)break;
        ck[index]=false;


        float minV=10000;
        float maxV=-10000;
        for(int i=0;i<c_window->selectedSpines[curGroup].size();i++){
            if(minV>c_window->data[c_window->selectedSpines[curGroup][i]][index]){
                minV=c_window->data[c_window->selectedSpines[curGroup][i]][index];
            }
            if(maxV<c_window->data[c_window->selectedSpines[curGroup][i]][index]){
                maxV=c_window->data[c_window->selectedSpines[curGroup][i]][index];
            }
        }

        char tt[50];
        sprintf(tt,"%.2f<%s<%.2f",minV-0.01,c_window->featureNames[index].toStdString().c_str(),maxV+0.01);
        c_window->constrains[curGroup]+=QString(tt);
        c_window->constrains[curGroup]+='\n';

    }

    c_window->selectionWindow->tEdit[curGroup]->setPlainText(c_window->constrains[curGroup]);



//    QVector<int>tg[4];
//    for(int i=0;i<c_window->data.length();i++){
//        if(c_window->clusterLabel[i]>=0){
//            tg[c_window->clusterLabel[i]].push_back(i);
//        }
//    }

//    if(tg[curGroup].size()==0)return;
//    c_window->constrains[curGroup]="";

//    float score[20]; //for cur group and each feature
//    float var[20]; //for each group and each feature
//    float mean[10][20]; //for each group and each feature
//    for(int n=0;n<groupN;n++){
//        for(int i=0;i<c_window->featureNum;i++){
//            score[i]=0;
//            var[i]=0;
//            mean[n][i]=0;
//        }
//    }


//    for(int n=1;n<groupN;n++){
//        if(tg[n].size()==0)continue;
//        for(int i=0;i<c_window->featureNum;i++){
//            for(int k=0;k<tg[n].size();k++){
//                mean[n][i]+=(c_window->data[tg[n][k]][i]-c_window->featureRanges[i].x())/(c_window->featureRanges[i].y()-c_window->featureRanges[i].x());
//            }
//            mean[n][i]/=tg[n].size();
//            qDebug()<<"mean "<<n<<":"<<i<<"="<<mean[n][i];
//        }
//    }

//    for(int i=0;i<c_window->featureNum;i++){
//        for(int k=0;k<tg[curGroup].size();k++){
//            float t=mean[curGroup][i]-(c_window->data[tg[curGroup][k]][i]-c_window->featureRanges[i].x())/(c_window->featureRanges[i].y()-c_window->featureRanges[i].x());
//            t*=t;
//            var[i]+=t;
//        }
//        var[i]/=tg[curGroup].size();
//        qDebug()<<"var "<<curGroup<<":"<<i<<"="<<var[i];

//    }

//    for(int i=0;i<c_window->featureNum;i++){
//        score[i]+=-var[i];

//        for(int n=1;n<groupN;n++){
//            if(n==curGroup)continue;
//            if(tg[n].size()==0)continue;


//            float t=mean[curGroup][i]-mean[n][i];
//            score[i]+=t*t;
//        }
//        qDebug()<<"score "<<c_window->featureNames[i]<<"="<<score[i];
//    }

//    bool ck[20];
//    for(int i=0;i<c_window->featureNum;i++){
//        ck[i]=true;
//    }

//    for(int k=0;k<4;k++){
//        float maxScore=0;
//        int index=-1;
//        for(int i=0;i<c_window->featureNum;i++){
//            if(ck[i]){
//                if(maxScore<score[i]){
//                    maxScore=score[i];
//                    index=i;
//                }
//            }
//        }
//        if(index==-1)break;
//        ck[index]=false;

//        if(mean[curGroup][index]>=mean[1][index]
//                && mean[curGroup][index]>=mean[2][index]
//                && mean[curGroup][index]>=mean[3][index]){
//            float minV=10000;
//            for(int i=0;i<tg[curGroup].size();i++){
//                if(minV>c_window->data[tg[curGroup][i]][index]){
//                    minV=c_window->data[tg[curGroup][i]][index];
//                }
//            }

//            char tt[50];
//            sprintf(tt,"%s>%.2f",c_window->featureNames[index].toStdString().c_str(),minV-0.01);
//            c_window->constrains[curGroup]+=QString(tt);
//            c_window->constrains[curGroup]+='\n';

//        }
//        else if(mean[curGroup][index]<=mean[1][index]
//                && mean[curGroup][index]<=mean[2][index]
//                && mean[curGroup][index]<=mean[3][index]){
//            float maxV=-10000;
//            for(int i=0;i<tg[curGroup].size();i++){
//                if(maxV<c_window->data[tg[curGroup][i]][index]){
//                    maxV=c_window->data[tg[curGroup][i]][index];
//                }
//            }

//            char tt[50];
//            sprintf(tt,"%s<%.2f",c_window->featureNames[index].toStdString().c_str(),maxV+0.01);
//            c_window->constrains[curGroup]+=QString(tt);
//            c_window->constrains[curGroup]+='\n';

//        }
//        else if(mean[curGroup][index]>0.5){

//            float minV=10000;
//            for(int i=0;i<tg[curGroup].size();i++){
//                if(minV>c_window->data[tg[curGroup][i]][index]){
//                    minV=c_window->data[tg[curGroup][i]][index];
//                }
//            }

//            char tt[50];
//            sprintf(tt,"%s>%.2f",c_window->featureNames[index].toStdString().c_str(),minV-0.01);
//            c_window->constrains[curGroup]+=QString(tt);
//            c_window->constrains[curGroup]+='\n';
//        }
//        else{
//            float maxV=-10000;
//            for(int i=0;i<tg[curGroup].size();i++){
//                if(maxV<c_window->data[tg[curGroup][i]][index]){
//                    maxV=c_window->data[tg[curGroup][i]][index];
//                }
//            }

//            char tt[50];
//            sprintf(tt,"%s<%.2f",c_window->featureNames[index].toStdString().c_str(),maxV+0.01);
//            c_window->constrains[curGroup]+=QString(tt);
//            c_window->constrains[curGroup]+='\n';
//        }
//    }

//    c_window->selectionWindow->tEdit[curGroup]->setPlainText(c_window->constrains[curGroup]);

}


void seedSelectionWindow::chagnedGroupNum(int a){
    if(a>groupN){
        for(int i=groupN;i<a;i++){
            listLayout->addWidget(groupBox[i]);
            groupBox[i]->show();
        }
    }
    else{
        for(int i=groupN-1;i>=a;i--){
            listLayout->removeWidget(groupBox[i]);
            groupBox[i]->hide();
        }
    }

    groupN=a;
//    curSelectGroup->setMaximum(a-1);

}
void seedSelectionWindow::constrainChanged(){
    for(int i=0;i<groupN;i++){
        c_window->constrains[i]=tEdit[i]->toPlainText();
    }
}
void seedSelectionWindow::paintEvent(QPaintEvent *){

    if(c_window->seedChanged){
        c_window->generateFlow();

        for(int i=0;i<groupN;i++){
            for(int j=spineList[i]->count()-1;j>=0;j--){
                QListWidgetItem *t=spineList[i]->item(j);
                if(t!=NULL){
                    spineList[i]->removeItemWidget(t);
                    delete t;
                }
            }
            for(int j=0;j<c_window->selectedSpines[i].length();j++){
                spineList[i]->addItem(c_window->spineNames[c_window->selectedSpines[i][j]]);
            }
        }
        if(c_window->lookSelectedSpine->isChecked()){
            c_window->checkSpineEnable();
            c_window->synchronization();

        }
        c_window->seedChanged=false;
    }


}

void seedSelectionWindow::focusItem(QListWidgetItem *a){
    if(c_window->clicked)return;

    for(int i=0;i<c_window->spineNames.length();i++){
        if(c_window->spineNames[i]==a->text()){
            c_window->focusSpine=i;
            c_window->synchronization();
            break;
        }
    }
}
void seedSelectionWindow::clickItem(QListWidgetItem *a){
    if(c_window->focusSpine!=-1 && !c_window->clicked){
        c_window->clicked=true;
        c_window->viewChange(c_window->focusSpine);
        c_window->synchronization();
    }
}

void seedSelectionWindow::pushDeleteButton(){
    int curButton;
    for(int i=0;i<groupN;i++){
        if(deleteSpine[i]->underMouse()){
            curButton=i;
            break;
        }
    }

    QList<QListWidgetItem*> ts=spineList[curButton]->selectedItems();
    for(int i=ts.length()-1;i>=0;i--){
        int curRow=spineList[curButton]->row(ts[i]);
        spineList[curButton]->removeItemWidget(ts[i]);
        delete ts[i];
        if(curRow>=0){
            int spineNum=c_window->selectedSpines[curButton].at(curRow);
//            QListWidgetItem *t=spineList[curButton]->item(curRow);
//            spineList[curButton]->removeItemWidget(t);
//            delete t;
            c_window->deleteSpineFromGroup(spineNum,curButton);
        }
    }

}

void seedSelectionWindow::pushAddAllButton(){

    if(c_window->clicked){
//        if(c_window->indexOfSelectedSpines[c_window->focusSpine]!=-1){
//            QListWidgetItem *t=spineList[c_window->indexOfSelectedSpines[c_window->focusSpine]]->
//                    item(c_window->selectedSpines[c_window->indexOfSelectedSpines[c_window->focusSpine]].indexOf(c_window->focusSpine));
//            spineList[c_window->indexOfSelectedSpines[c_window->focusSpine]]->removeItemWidget(t);
//            delete t;
//        }
        c_window->addSpineToGroup(c_window->focusSpine,curSelectGroup->currentIndex());
//        spineList[curSelectGroup->value()]->addItem(c_window->spineNames[c_window->focusSpine]);
    }
    else{

        for(int i=0;i<c_window->spineNames.length();i++){
            if(c_window->spineEnable[i]){
//                if(c_window->indexOfSelectedSpines[i]!=-1){
//                    QListWidgetItem *t=spineList[c_window->indexOfSelectedSpines[i]]->
//                            item(c_window->selectedSpines[c_window->indexOfSelectedSpines[i]].indexOf(i));
//                    spineList[c_window->indexOfSelectedSpines[i]]->removeItemWidget(t);
//                    delete t;
//                }
                c_window->addSpineToGroup(i,curSelectGroup->currentIndex());
//                spineList[curSelectGroup->value()]->addItem(c_window->spineNames[i]);
            }
        }
    }
}

seedSelectionWindow::~seedSelectionWindow(){

}


QSize seedSelectionWindow::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize seedSelectionWindow::sizeHint() const
{
    return QSize(800, 800);
}

void seedSelectionWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape){
    }
}

void seedSelectionWindow::mouseMoveEvent(QMouseEvent *event)
{
    mousePos=event->pos();
}
void seedSelectionWindow::changeSelectionGroup(int a){
    c_window->currentSelectGroup=a;
    if(c_window->lookSelectedSpine->isChecked()){
        c_window->checkSpineEnable();
        c_window->synchronization();
    }
}
