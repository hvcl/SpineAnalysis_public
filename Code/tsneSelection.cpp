///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "tsneSelection.h"
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
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
TSNE_worker::TSNE_worker(Window *p){
    c_window=p;
}

void TSNE_worker::selective_run(){


    int dataLen=0;
    int featureNum=0;
    FILE *Input=fopen("tsneInput.raw","wb");
    for(int i=0;i<c_window->data.length();i++){
        if(c_window->spineEnable[i]){
            dataLen++;
            featureNum=0;
            for(int j=0;j<c_window->data[i].length();j++){
                if(c_window->featureEnables[j]){
                    featureNum++;
                    float v=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                    fwrite(&v,4,1,Input);
                }
            }
        }
    }
    fclose(Input);

    char tt[30];
    std::string query;
    query="python tsne2.py ";
    query+=itoa(dataLen,tt,10);
    query+=" ";
    query+=itoa(featureNum,tt,10);

    query+=" > tsneLog";

    qDebug()<<QString(query.c_str())<<endl;

    system(query.c_str());
    //system("start \"\" cmd /c C:/\"Program Files\"/Wireshark/tshark.exe -a duration:130 -i 3 -T fields -e frame.number -e frame.time -e _ws.col.Info ^> c:/test/output.csv");



    emit selectiveResultReady();
}


void TSNE_worker::run(){


    int featureNum=0;
    FILE *Input=fopen("tsneInput.raw","wb");
    for(int i=0;i<c_window->data.length();i++){
        featureNum=0;
        for(int j=0;j<c_window->data[i].length();j++){
            if(c_window->featureEnables[j]){
                featureNum++;
                float v=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                fwrite(&v,4,1,Input);
            }
        }
    }
    fclose(Input);

    char tt[30];
    std::string query;
    if(c_window->projection->currentIndex()==0){
        query="python pca.py ";

    }
    else if(c_window->projection->currentIndex()==1){
        query="python tsne.py ";
    }
    else{
        query="python umap_test.py ";
    }
    query+=itoa(c_window->data.length(),tt,10);
    query+=" ";
    query+=itoa(featureNum,tt,10);

    query+=" > tsneLog";

    qDebug()<<QString(query.c_str())<<endl;


    system(query.c_str());
    //system("start \"\" cmd /c C:/\"Program Files\"/Wireshark/tshark.exe -a duration:130 -i 3 -T fields -e frame.number -e frame.time -e _ws.col.Info ^> c:/test/output.csv");



    emit resultReady();
}


void TSNE_worker::TrendPCArun(){


    int featureNum=0;
    FILE *Input=fopen("tsneInput.raw","wb");
    for(int i=0;i<c_window->data.length();i++){
        featureNum=0;
        for(int j=0;j<c_window->data[i].length();j++){
            if(c_window->featureEnables[j]){
                featureNum++;
                float v=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                fwrite(&v,4,1,Input);
            }
        }
    }
    fclose(Input);

    char tt[30];
    std::string query;
    query="python pca1D.py ";
    query+=itoa(c_window->data.length(),tt,10);
    query+=" ";
    query+=itoa(featureNum,tt,10);

    query+=" > tsneLog";

    qDebug()<<QString(query.c_str())<<endl;


    system(query.c_str());
    //system("start \"\" cmd /c C:/\"Program Files\"/Wireshark/tshark.exe -a duration:130 -i 3 -T fields -e frame.number -e frame.time -e _ws.col.Info ^> c:/test/output.csv");



    emit resultTrendPCAReady();
}


void TSNE_worker::TrendtSNErun(){


    int featureNum=0;
    FILE *Input=fopen("tsneInput.raw","wb");
    for(int i=0;i<c_window->data.length();i++){
        featureNum=0;
        for(int j=0;j<c_window->data[i].length();j++){
            if(c_window->featureEnables[j]){
                featureNum++;
                float v=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                fwrite(&v,4,1,Input);
            }
        }
    }
    fclose(Input);

    char tt[30];
    std::string query;
    query="python tsne1D.py ";
    query+=itoa(c_window->data.length(),tt,10);
    query+=" ";
    query+=itoa(featureNum,tt,10);

    query+=" > tsneLog";

    qDebug()<<QString(query.c_str())<<endl;


    system(query.c_str());
    //system("start \"\" cmd /c C:/\"Program Files\"/Wireshark/tshark.exe -a duration:130 -i 3 -T fields -e frame.number -e frame.time -e _ws.col.Info ^> c:/test/output.csv");



    emit resultTrendtSNEReady();
}


void tsneSelection::changedResult(){

    FILE *Result=fopen("tsneResult.raw","rb");

    maxValue=QVector2D(-1000,-1000);
    minValue=QVector2D(1000,1000);
    for(int i=0;i<c_window->data.length();i++){
        float t[2];
        fread(t,4,2,Result);
        c_window->tsneResult[i].setX(t[0]);
        c_window->tsneResult[i].setY(t[1]);
        if(c_window->tsneResult[i].x()>maxValue.x()){
            maxValue.setX(c_window->tsneResult[i].x());
        }
        if(c_window->tsneResult[i].y()>maxValue.y()){
            maxValue.setY(c_window->tsneResult[i].y());
        }
        if(c_window->tsneResult[i].x()<minValue.x()){
            minValue.setX(c_window->tsneResult[i].x());
        }
        if(c_window->tsneResult[i].y()<minValue.y()){
            minValue.setY(c_window->tsneResult[i].y());
        }
    }

    fclose(Result);


    for(int i=0;i<c_window->data.length();i++){
        float xCoord=(c_window->tsneResult[i].x()-minValue.x())/(maxValue.x()-minValue.x());
        float yCoord= (c_window->tsneResult[i].y()-minValue.y())/(maxValue.y()-minValue.y());

        c_window->tsneResult[i]=QVector2D(xCoord,yCoord);
    }
    c_window->synchronization();
    c_window->arrangeRanderPart();
    //c_window->preOrdering();
//    c_window->arranging=true;

}

void tsneSelection::changedTrendPCAResult(){

    FILE *Result=fopen("tsneResult.raw","rb");

    maxValue=QVector2D(-1000,-1000);
    minValue=QVector2D(1000,1000);
    for(int i=0;i<c_window->data.length();i++){
        float t[2];
        fread(t,4,2,Result);
        c_window->tsneResult[i].setX(t[0]);
        c_window->tsneResult[i].setY(t[1]);
        if(c_window->tsneResult[i].x()>maxValue.x()){
            maxValue.setX(c_window->tsneResult[i].x());
        }
        if(c_window->tsneResult[i].y()>maxValue.y()){
            maxValue.setY(c_window->tsneResult[i].y());
        }
        if(c_window->tsneResult[i].x()<minValue.x()){
            minValue.setX(c_window->tsneResult[i].x());
        }
        if(c_window->tsneResult[i].y()<minValue.y()){
            minValue.setY(c_window->tsneResult[i].y());
        }
    }

    fclose(Result);


    for(int i=0;i<c_window->data.length();i++){
        float xCoord=(c_window->tsneResult[i].x()-minValue.x())/(maxValue.x()-minValue.x());
        float yCoord= (c_window->tsneResult[i].y()-minValue.y())/(maxValue.y()-minValue.y());

        c_window->tsneResult[i]=QVector2D(xCoord,yCoord);
    }
    c_window->synchronization();

}

void tsneSelection::changedTrendtSNEResult(){

    FILE *Result=fopen("tsneResult.raw","rb");

    maxValue=QVector2D(-1000,-1000);
    minValue=QVector2D(1000,1000);
    for(int i=0;i<c_window->data.length();i++){
        float t;
        fread(&t,4,1,Result);
        c_window->tsneResult[i].setX(t);
        c_window->tsneResult[i].setY(t);
        if(c_window->tsneResult[i].x()>maxValue.x()){
            maxValue.setX(c_window->tsneResult[i].x());
        }
        if(c_window->tsneResult[i].y()>maxValue.y()){
            maxValue.setY(c_window->tsneResult[i].y());
        }
        if(c_window->tsneResult[i].x()<minValue.x()){
            minValue.setX(c_window->tsneResult[i].x());
        }
        if(c_window->tsneResult[i].y()<minValue.y()){
            minValue.setY(c_window->tsneResult[i].y());
        }
    }

    fclose(Result);


    for(int i=0;i<c_window->data.length();i++){
        float xCoord=(c_window->tsneResult[i].x()-minValue.x())/(maxValue.x()-minValue.x());
        float yCoord= 0.5;

        c_window->tsneResult[i]=QVector2D(xCoord,yCoord);
    }
    c_window->synchronization();

}


void tsneSelection::changedSelectiveResult(){

    FILE *Result=fopen("tsneResult.raw","rb");

    maxValue=QVector2D(-1000,-1000);
    minValue=QVector2D(1000,1000);
    for(int i=0;i<c_window->data.length();i++){
        if(c_window->spineEnable[i]){
            float t[2];
            fread(t,4,2,Result);
            c_window->tsneResult[i].setX(t[0]);
            c_window->tsneResult[i].setY(t[1]);
            if(c_window->tsneResult[i].x()>maxValue.x()){
                maxValue.setX(c_window->tsneResult[i].x());
            }
            if(c_window->tsneResult[i].y()>maxValue.y()){
                maxValue.setY(c_window->tsneResult[i].y());
            }
            if(c_window->tsneResult[i].x()<minValue.x()){
                minValue.setX(c_window->tsneResult[i].x());
            }
            if(c_window->tsneResult[i].y()<minValue.y()){
                minValue.setY(c_window->tsneResult[i].y());
            }
        }
        else{
            c_window->tsneResult[i].setX(-1000);
            c_window->tsneResult[i].setY(-1000);
        }
    }

    fclose(Result);


    for(int i=0;i<c_window->data.length();i++){
        float xCoord=(c_window->tsneResult[i].x()-minValue.x())/(maxValue.x()-minValue.x());
        float yCoord= (c_window->tsneResult[i].y()-minValue.y())/(maxValue.y()-minValue.y());

        c_window->tsneResult[i]=QVector2D(xCoord,yCoord);
    }
    c_window->synchronization();

}

tsneSelection::tsneSelection(Window *p, QWidget *parent)
    : QWidget(parent)
{


    IsSelectGroupMode=false;


    c_window=p;

    doSelect=false;

    FILE *Result=fopen("tsneResult.raw","rb");
    if(Result==NULL){
        FILE *Input=fopen("tsneInput.raw","wb");
        for(int i=0;i<c_window->data.length();i++){
            for(int j=0;j<c_window->data[i].length();j++){
//                  fwrite(&c_window->data[i][j],4,1,Input);
                float v=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                fwrite(&v,4,1,Input);
            }
        }
        fclose(Input);

        char tt[30];
        std::string query;
        query="python tsne.py ";
        query+=itoa(c_window->data.length(),tt,10);
        query+=" ";
        query+=itoa(c_window->data[0].length(),tt,10);

        query+=" > tsneLog";
        system(query.c_str());


        Result=fopen("tsneResult.raw","rb");
    }

    maxValue=QVector2D(-1000,-1000);
    minValue=QVector2D(1000,1000);
    for(int i=0;i<c_window->data.length();i++){
        float t[2];
        fread(t,4,2,Result);
        c_window->tsneResult[i].setX(t[0]);
        c_window->tsneResult[i].setY(t[1]);
        if(c_window->tsneResult[i].x()>maxValue.x()){
            maxValue.setX(c_window->tsneResult[i].x());
        }
        if(c_window->tsneResult[i].y()>maxValue.y()){
            maxValue.setY(c_window->tsneResult[i].y());
        }
        if(c_window->tsneResult[i].x()<minValue.x()){
            minValue.setX(c_window->tsneResult[i].x());
        }
        if(c_window->tsneResult[i].y()<minValue.y()){
            minValue.setY(c_window->tsneResult[i].y());
        }
    }

    fclose(Result);


    for(int i=0;i<c_window->data.length();i++){
        float xCoord=(c_window->tsneResult[i].x()-minValue.x())/(maxValue.x()-minValue.x());
        float yCoord= (c_window->tsneResult[i].y()-minValue.y())/(maxValue.y()-minValue.y());

        c_window->tsneResult[i]=QVector2D(xCoord,yCoord);
    }

    TSNE_worker *tsneWorker=new TSNE_worker(c_window);
    tsneWorker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, tsneWorker, &QObject::deleteLater);
    connect(c_window,SIGNAL(runTSNE()),tsneWorker,SLOT(run()));
    connect(tsneWorker,SIGNAL(selectiveResultReady()),this,SLOT(changedSelectiveResult()));
    connect(c_window->SelectiveTsne,SIGNAL(pressed()),tsneWorker,SLOT(selective_run()));

    connect(tsneWorker,SIGNAL(resultReady()),this,SLOT(changedResult()));
    connect(c_window->AlltSNE,SIGNAL(pressed()),tsneWorker,SLOT(run()));


    connect(tsneWorker,SIGNAL(resultTrendPCAReady()),this,SLOT(changedTrendPCAResult()));
    connect(c_window->AllPCAForTrend,SIGNAL(pressed()),tsneWorker,SLOT(TrendPCArun()));

    connect(tsneWorker,SIGNAL(resultTrendtSNEReady()),this,SLOT(changedTrendtSNEResult()));
    connect(c_window->AlltSNEForTrend,SIGNAL(pressed()),tsneWorker,SLOT(TrendtSNErun()));



    workerThread.start();

    setBackgroundRole(QPalette::Base);
    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);


}

tsneSelection::~tsneSelection(){
    workerThread.quit();
    workerThread.wait();
}

void tsneSelection::resizeEvent(QResizeEvent *event){
    int w=event->size().width();
    int h=event->size().height();
    int t=w>h?h:w;
    this->resize(w,h);
}

QSize tsneSelection::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize tsneSelection::sizeHint() const
{
    return QSize(800, 800);
}

void tsneSelection::paintEvent(QPaintEvent *)
{

    graphStart=QVector2D(this->width()*0.1,this->height()*0.9);
    graphEnd=QVector2D(this->width()*0.9,this->height()*0.1);


    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,true);
    //     painter.drawPixmap(0,0,this->width(),this->height(),background);//resolution

    QPen t_pen;
    QBrush t_brush(Qt::SolidPattern);

    ////Draw frame
    ///
    ///
    t_pen.setColor(QColor(150,150,150));
    t_pen.setWidth(2*c_window->WINDOW_SCALE);
    painter.setPen(t_pen);
    painter.drawLine(this->width()*0.02,this->height()*0.02,this->width()*0.98,this->height()*0.02);
    painter.drawLine(this->width()*0.98,this->height()*0.02,this->width()*0.98,this->height()*0.98);
    painter.drawLine(this->width()*0.98,this->height()*0.98,this->width()*0.02,this->height()*0.98);
    painter.drawLine(this->width()*0.02,this->height()*0.98,this->width()*0.02,this->height()*0.02);

    t_pen.setColor(QColor(200,200,200));
    t_pen.setWidthF(c_window->WINDOW_SCALE);
    t_pen.setStyle(Qt::DashLine);
    painter.setPen(t_pen);
    for(int i=0;i<=10;i++){
        painter.drawLine(this->width()*0.02,graphStart.y()+(graphEnd.y()-graphStart.y())/10.0*i,this->width()*0.98,graphStart.y()+(graphEnd.y()-graphStart.y())/10.0*i);
    }
    for(int i=0;i<=10;i++){
        painter.drawLine(graphStart.x()+(graphEnd.x()-graphStart.x())/10.0*i,this->height()*0.02,graphStart.x()+(graphEnd.x()-graphStart.x())/10.0*i,this->height()*0.98);
    }


    ////Draw each point

    if(c_window->focusSpine==-1){
        t_pen.setStyle(Qt::SolidLine);
        for(int i=0;i<c_window->data.length();i++){

            t_pen.setWidth(7*c_window->WINDOW_SCALE);
            if(c_window->coloringType==0){
                t_pen.setColor(c_window->spineColors[i]);
            }
            else if(c_window->coloringType==1){
                t_pen.setColor(c_window->clusterColors[c_window->clusterLabel[i]]);
            }
            else if(c_window->coloringType==2){
                if(c_window->indexOfSelectedSpines[i]==-1){
                    t_pen.setColor(QColor(50,50,50,30));
                }
                else t_pen.setColor(c_window->clusterColors[c_window->indexOfSelectedSpines[i]]);
            }
            else if(c_window->coloringType==3 && c_window->typeEnable){
                t_pen.setColor(c_window->typeColors[c_window->typesInt[i]]);
            }
            else if(c_window->coloringType==4){
                t_pen.setColor(c_window->groups[c_window->spineGroups[i]]->color);
            }
            if(c_window->spineEnable[i]==false){
                t_pen.setColor(QColor(50,50,50,30));
            }
            painter.setPen(t_pen);

            painter.drawEllipse(QPointF(graphStart.x()+c_window->tsneResult[i].x()*(graphEnd.x()-graphStart.x())
                                        ,graphStart.y()+c_window->tsneResult[i].y()*(graphEnd.y()-graphStart.y())),7*c_window->WINDOW_SCALE,7*c_window->WINDOW_SCALE);
        }
    }
    else{
        t_pen.setStyle(Qt::SolidLine);
        for(int i=0;i<c_window->data.length();i++){

            t_pen.setWidth(7*c_window->WINDOW_SCALE);
            t_pen.setColor(QColor(50,50,50,30));
            painter.setPen(t_pen);
            painter.drawEllipse(QPointF(graphStart.x()+c_window->tsneResult[i].x()*(graphEnd.x()-graphStart.x())
                                        ,graphStart.y()+c_window->tsneResult[i].y()*(graphEnd.y()-graphStart.y())),7*c_window->WINDOW_SCALE,7*c_window->WINDOW_SCALE);
        }


        t_pen.setWidth(7*c_window->WINDOW_SCALE);

        if(c_window->coloringType==0){
            t_pen.setColor(c_window->spineColors[c_window->focusSpine]);
        }
        else if(c_window->coloringType==1){
            t_pen.setColor(c_window->clusterColors[c_window->clusterLabel[c_window->focusSpine]]);
        }
        else if(c_window->coloringType==2){
            if(c_window->indexOfSelectedSpines[c_window->focusSpine]==-1){
                t_pen.setColor(QColor(50,50,50,30));
            }
            else t_pen.setColor(c_window->clusterColors[c_window->indexOfSelectedSpines[c_window->focusSpine]]);
        }
        else if(c_window->coloringType==3 && c_window->typeEnable){
            t_pen.setColor(c_window->typeColors[c_window->typesInt[c_window->focusSpine]]);
        }
        else if(c_window->coloringType==4){
            t_pen.setColor(c_window->groups[c_window->spineGroups[c_window->focusSpine]]->color);
        }

        painter.setPen(t_pen);
        painter.drawEllipse(QPointF(graphStart.x()+c_window->tsneResult[c_window->focusSpine].x()*(graphEnd.x()-graphStart.x())
                                    ,graphStart.y()+c_window->tsneResult[c_window->focusSpine].y()*(graphEnd.y()-graphStart.y())),7*c_window->WINDOW_SCALE,7*c_window->WINDOW_SCALE);


    }


    ////Draw Selection frame & selection information
    if(c_window->AreaSelection){
        t_pen.setColor(QColor(0,0,255,40));
        t_pen.setWidth(5*c_window->WINDOW_SCALE);
        painter.setPen(Qt::NoPen);
        t_brush.setColor(QColor(0,0,255,40));
        painter.setBrush(t_brush);

        int x1=graphStart.x()+c_window->selectionAreaStart.x()*(graphEnd.x()-graphStart.x());
        int x2=graphStart.x()+c_window->selectionAreaEnd.x()*(graphEnd.x()-graphStart.x());
        int y1=graphStart.y()+c_window->selectionAreaStart.y()*(graphEnd.y()-graphStart.y());
        int y2=graphStart.y()+c_window->selectionAreaEnd.y()*(graphEnd.y()-graphStart.y());
//        if(x2<x1){
//            int t=x1;
//            x1=x2;
//            x2=x1;
//        }
//        if(y2<y1){
//            int t=y1;
//            y1=y2;
//            y2=y1;
//        }
        painter.drawRect(x1,y1,x2-x1,y2-y1);

    }
//    if(selectionFrame!=-1){
//        t_pen.setColor(QColor(0,255,0,40));
//        t_pen.setWidth(50);
//        t_pen.setCapStyle(Qt::RoundCap);
//        painter.setPen(t_pen);
//        painter.drawLine(axisStarts[selectionFrame].x(),axisStarts[selectionFrame].y()
//                         ,axisEnds[selectionFrame].x(),axisEnds[selectionFrame].y());

//    }

//    for(int i=0;i<c_window->featureNum;i++){
//        if(c_window->selection[i]){
//            t_pen.setColor(QColor(35,255,159,200));
//            t_pen.setWidth(20);
//            t_pen.setCapStyle(Qt::RoundCap);
//            painter.setPen(t_pen);
//            QPointF p1(axisStarts[i].x(),axisStarts[i].y());
//            QPointF p2(axisEnds[i].x(),axisEnds[i].y());
//            painter.drawLine(p1*(1-c_window->selectionRange[i].x())+p2*c_window->selectionRange[i].x(),p1*(1-c_window->selectionRange[i].y())+p2*c_window->selectionRange[i].y());

//        }
//    }

//    ////Draw Label
//    t_pen.setColor(QColor(0,0,0));
//    painter.setPen(t_pen);
//    for(int i=0;i<c_window->featureNum;i++){
//        painter.drawText(axisEnds[i].x(),axisEnds[i].y()-this->height()*0.05,c_window->featureNames[i]);

//    }



    ////Draw group change option if IsSelectGroupMode is true

    if(IsSelectGroupMode==true){
        painter.setPen(Qt::NoPen);
        //t_brush.setColor(QColor(0,0,255,40));


        //t_pen.setWidth(30);

        int groupN=c_window->GroupNumSet->value();
        for(int i=0;i<groupN;i++){

            //t_brush.setColor(c_window->clusterColors[i]);
            t_pen.setColor(c_window->clusterColors[i]);
            t_pen.setWidth(25*c_window->WINDOW_SCALE);
            t_pen.setCapStyle(Qt::FlatCap);

            painter.setBrush(Qt::NoBrush);
            painter.setPen(t_pen);

            QPointF t=QPointF(graphStart.x()+c_window->tsneResult[selectNum].x()*(graphEnd.x()-graphStart.x())
                    ,graphStart.y()+c_window->tsneResult[selectNum].y()*(graphEnd.y()-graphStart.y()));
            painter.drawArc(t.x()-30,t.y()-30,60,60,360*16/groupN * i - 360*8/groupN + 16*3,360*16/groupN - 16*6);

            QFont tFont;
            tFont.setBold(true);
            painter.setFont(tFont);
            t_pen.setColor(QColor(255,255,255));
            painter.setPen(t_pen);
            painter.setBrush(Qt::NoBrush);

            QPointF t2=QPointF(cos(-M_PI/groupN * i * 2),sin(-M_PI/groupN * i * 2))*30 + t;

            char tc[10];
            sprintf(tc,"G%d",i);
            painter.drawText(t2.x()-10,t2.y()+5,tr(tc));

        }

    }


//    int gridSize=sqrt(c_window->enabledSpineNum);
//    t_pen.setColor(QColor(255,0,0));
//    t_pen.setWidth(10*c_window->WINDOW_SCALE);
//    painter.setPen(t_pen);

//    for(int i=0;i<gridSize;i++){
//        for(int j=0;j<gridSize;j++){
//            float x=graphStart.x()+c_window->nodePosition[j][i].x()*(graphEnd.x()-graphStart.x());
//            float y=graphStart.y()+c_window->nodePosition[j][i].y()*(graphEnd.y()-graphStart.x());
//            painter.drawPoint(x,y);
//        }
//    }

}

void tsneSelection::mousePressEvent(QMouseEvent *event)
{
    if(event->x()<0 || event->x()>this->width() || event->y()<0 || event->y()>this->height())return;

    lastPos = event->pos();
    button=event->button();

    if(c_window->groupChangeMode->isChecked()){
        if(IsSelectGroupMode){

            int groupN=c_window->GroupNumSet->value();
            for(int i=0;i<groupN;i++){


                QVector2D t=QVector2D(graphStart.x()+c_window->tsneResult[selectNum].x()*(graphEnd.x()-graphStart.x())
                        ,graphStart.y()+c_window->tsneResult[selectNum].y()*(graphEnd.y()-graphStart.y()));
                //painter.drawArc(t.x()-30,t.y()-30,60,60,360*16/groupN * i - 360*8/groupN + 16*3,360*16/groupN - 16*6);

                QVector2D t2=QVector2D(cos(-M_PI/groupN * i * 2),sin(-M_PI/groupN * i * 2))*30 + t;

                QVector2D x=QVector2D(lastPos.x(),lastPos.y());
                float l=(x-t).length();

                float l2=(x-t2).length();

                float angle=acos((l*l+30*30-l2*l2)/(2*l*30));

                if(abs(angle)<M_PI/groupN && l<43 && l>17){
                    c_window->clusterLabel[selectNum]=i;
                    break;
                }

            }
            IsSelectGroupMode=false;
            update();
            emit synchronization();
            return;
        }
        else{
            if(c_window->focusSpine!=-1){
                IsSelectGroupMode=true;
                selectNum=c_window->focusSpine;
                doSelect=false;
                update();
                emit synchronization();
                return;
            }
        }
    }


    if(c_window->spineEnableMode->isChecked()){
        if(c_window->focusSpine!=-1){
            c_window->displaySpines[c_window->focusSpine]=true;
        }
        return;
    }


    if(event->button()==Qt::LeftButton){

        if(c_window->focusSpine!=-1){
            if(c_window->IsShift){
                c_window->addSpineToGroup(c_window->focusSpine,c_window->currentSelectGroup);
            }
            else{
                c_window->clicked=true;
                emit viewChange(c_window->focusSpine);
            }
        }

    }
    else if(event->button()==Qt::MidButton){

    }
    else if(event->button() == Qt::RightButton){

        if(doMoving==false){

            if(c_window->AreaSelection){
                QVector2D t(float(event->x()-graphStart.x())/(graphEnd.x()-graphStart.x()),float(event->y()-graphStart.y())/(graphEnd.y()-graphStart.y()));
                QVector2D mint(fmin(c_window->selectionAreaStart.x(),c_window->selectionAreaEnd.x()),fmin(c_window->selectionAreaStart.y(),c_window->selectionAreaEnd.y()));
                QVector2D maxt(fmax(c_window->selectionAreaStart.x(),c_window->selectionAreaEnd.x()),fmax(c_window->selectionAreaStart.y(),c_window->selectionAreaEnd.y()));
                if(t.x()<maxt.x() && t.y()<maxt.y() && t.x()>mint.x() && t.y()>mint.y()){
                    doMoving=true;
                    prevP=t;
                    c_window->focusSpine=-1;
                    c_window->AreaSelection=true;
                    if(c_window->clicked==true){
                        c_window->focusSpine=-1;
                        c_window->clicked=false;

                        emit viewChange(-1);
                    }


                    return;
                }
            }
        }

        if(doSelect==false){



            doSelect=true;
            c_window->focusSpine=-1;
            c_window->AreaSelection=true;

            if(c_window->clicked==true){
                c_window->focusSpine=-1;
                c_window->clicked=false;

                emit viewChange(-1);
            }
            c_window->selectionAreaStart=QVector2D(float(event->x()-graphStart.x())/(graphEnd.x()-graphStart.x()),float(event->y()-graphStart.y())/(graphEnd.y()-graphStart.y()));
            c_window->selectionAreaEnd=QVector2D(float(event->x()-graphStart.x())/(graphEnd.x()-graphStart.x()),float(event->y()-graphStart.y())/(graphEnd.y()-graphStart.y()));
        }
    }
    update();
    emit synchronization();


}
void tsneSelection::mouseReleaseEvent(QMouseEvent *event){

    if(event->button() == Qt::RightButton){

        if(doMoving==true){
            doMoving=false;
        }


        if(doSelect==true){
            doSelect=false;
            if((c_window->selectionAreaEnd-c_window->selectionAreaStart).length()<0.01){
                c_window->AreaSelection=false;
            }
            c_window->checkSpineEnable();
            update();
            emit synchronization();

            ////Todo:
            ////update spine grid using selection info
        }

//        if((c_window->selectionAreaEnd-c_window->selectionAreaStart).length()<0.01){
//            QDateTime utc = QDateTime::currentDateTimeUtc();

//                if(utc.toTime_t()-clickTime>1 && c_window->focusSpine!=-1){
//                    IsSelectGroupMode=true;
//                    selectNum=c_window->focusSpine;
//                }
//                else{
//                    IsSelectGroupMode=false;
//                }
//                update();

//        }
    }
}
void tsneSelection::mouseMoveEvent(QMouseEvent *event)
{
    setFocus();

    if(IsSelectGroupMode)return;


    if(event->x()<0 || event->x()>this->width() || event->y()<0 || event->y()>this->height())return;

    float tx=event->x();
    float ty=event->y();


    if(doMoving){
        QVector2D curP=QVector2D(float(event->x()-graphStart.x())/(graphEnd.x()-graphStart.x()),float(event->y()-graphStart.y())/(graphEnd.y()-graphStart.y()));

        c_window->selectionAreaStart=c_window->selectionAreaStart+curP-prevP;
        c_window->selectionAreaEnd=c_window->selectionAreaEnd+curP-prevP;

        prevP=curP;
        c_window->checkSpineEnable();
        update();
        emit synchronization();
        return;
    }


    if(doSelect){

        QVector2D curP=QVector2D(float(event->x()-graphStart.x())/(graphEnd.x()-graphStart.x()),float(event->y()-graphStart.y())/(graphEnd.y()-graphStart.y()));
        if(curP.x()<-0.05 || curP.x()>1.05 || curP.y()<-0.05 || curP.y()>1.05){
            doSelect=false;
            if((c_window->selectionAreaEnd-c_window->selectionAreaStart).length()<0.01){
                c_window->AreaSelection=false;
            }
        }
        else{
            c_window->selectionAreaEnd=curP;
        }
        c_window->checkSpineEnable();
        update();
        emit synchronization();

        return;
    }





    if(c_window->clicked)return;

    float minDis=10000;
    int prev=c_window->focusSpine;
    c_window->focusSpine=-1;

    if(!c_window->isFocus)return;

    for(int i=0;i<c_window->data.length();i++){
        if(c_window->spineEnable[i]==false){
            continue;
        }

        QVector2D tP(graphStart.x()+c_window->tsneResult[i].x()*(graphEnd.x()-graphStart.x())
                                    ,graphStart.y()+c_window->tsneResult[i].y()*(graphEnd.y()-graphStart.y()));


        float dis=(QVector2D(tx,ty)-tP).length();
        if(dis<minDis && dis<20){
            minDis=dis;
            c_window->focusSpine=i;
        }
    }
    if(prev!=c_window->focusSpine){
        c_window->changeTitle();

        update();
        emit synchronization();
    }

}

void tsneSelection::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape){
        if(c_window->clicked==true){
            c_window->clicked=false;
            c_window->focusSpine=-1;
            emit viewChange(-1);
        }
    }
    if(event->key()==Qt::Key_Shift){
        c_window->IsShift=true;
    }
}

void tsneSelection::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Shift){
        c_window->IsShift=false;
    }
}

void tsneSelection::timerEvent(QTimerEvent *event){
}


void tsneSelection::readAnalysisFile(){


}
