///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "setFeatureUsing.h"
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
#include <QFrame>



setFeatureUsing::setFeatureUsing(Window *p, QWidget *parent)
    : QWidget(parent)
{


    setStyleSheet("QWidget {font: bold;}");
    c_window=p;


    QVBoxLayout *wholeFrame=new QVBoxLayout;
    features=new QHBoxLayout;

    formula=new QPlainTextEdit();
    formula->setFixedHeight(50);

    addFeature=new QPushButton("Add Feature");

    QHBoxLayout *af=new QHBoxLayout;
    af->addWidget(formula);
    af->addWidget(addFeature);
    connect(addFeature,SIGNAL(pressed()),this,SLOT(runAddFeature()));

    autoManual=new QPushButton;
    if(c_window->IsAutoControl){
        autoManual->setText("Auto Control");
    }
    else{
        autoManual->setText("Manual Control");
    }
    connect(autoManual,SIGNAL(pressed()),this,SLOT(setAutoManual()));

    for(int i=0;i<c_window->featureNum;i++){
        featureWeight.push_back(new QSlider(Qt::Vertical));
        featureWeight[i]->setRange(0,100);
        featureWeight[i]->setValue(int(c_window->featureWeights[i]*100));
        featureWeight[i]->setTickInterval(10);
        featureWeight[i]->setTickPosition(QSlider::TicksRight);

        connect(featureWeight[i],SIGNAL(valueChanged(int)),this,SLOT(setFeatureWeight(int)));

        connect(featureWeight[i],SIGNAL(sliderReleased()),this,SLOT(releaseSlider()));
        char t[20];
        sprintf(t,"%.2f",featureWeight[i]->value()/100.0);
        curValue.push_back(new QLabel(QString(t)));

        featureOnOff.push_back(new QCheckBox(c_window->featureNames[i]));
        featureOnOff[i]->setChecked(c_window->featureEnables[i]);



        if(featureOnOff[i]->isChecked()){
            featureOnOff[i]->setStyleSheet("QCheckBox {color: rgb(100,100,255); }");
            curValue[i]->setStyleSheet("QLabel {color: rgb(100,100,255); }");
            featureWeight[i]->setStyleSheet("QSlider::handle:vertical {background-color: rgb(100,100,255);}");

        }
        else{
            featureOnOff[i]->setStyleSheet("QCheckBox {color: rgb(100,100,100); }");
            curValue[i]->setStyleSheet("QLabel {color: rgb(100,100,100); }");
            featureWeight[i]->setStyleSheet("QSlider::handle:vertical {background-color: rgb(100,100,100);}");


        }


        connect(featureOnOff[i],SIGNAL(clicked(bool)),this,SLOT(setFeatureOnOff(bool)));


        QVBoxLayout *each=new QVBoxLayout;

        each->addWidget(featureWeight[i]);
        each->addWidget(curValue[i]);
        each->addWidget(featureOnOff[i]);


        features->addLayout(each);
    }


    QFrame *tbar=new QFrame;
    tbar->setFrameShape(QFrame::HLine);

    wholeFrame->addLayout(af);
//    wholeFrame->addWidget(autoManual,2);
    wholeFrame->addWidget(tbar);
    wholeFrame->addLayout(features,6);

    setLayout(wholeFrame);

    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);


    setWindowTitle("Feature Manager");


}

void setFeatureUsing::setAutoManual(){
    if(autoManual->text()=="Auto Control"){
        autoManual->setText("Manual Control");
        c_window->IsAutoControl=false;
    }
    else{
        autoManual->setText("Auto Control");
        c_window->IsAutoControl=true;
    }
}

void setFeatureUsing::setFeatureOnOff(bool a){
    for(int i=0;i<c_window->featureNum;i++){
        c_window->featureEnables[i]=featureOnOff[i]->isChecked();
        if(featureOnOff[i]->isChecked()){
            featureOnOff[i]->setStyleSheet("QCheckBox {color: rgb(100,100,255); }");
            curValue[i]->setStyleSheet("QLabel {color: rgb(100,100,255); }");
            featureWeight[i]->setStyleSheet("QSlider::handle:vertical {background-color: rgb(100,100,255);}");

        }
        else{
            featureOnOff[i]->setStyleSheet("QCheckBox {color: rgb(100,100,100); }");
            curValue[i]->setStyleSheet("QLabel {color: rgb(100,100,100); }");
            featureWeight[i]->setStyleSheet("QSlider::handle:vertical {background-color: rgb(100,100,100);}");


        }
    }
    c_window->synchronization();
    emit c_window->runTSNE();
}
void setFeatureUsing::setFeatureWeight(int a){
    char t[20];
    for(int i=0;i<c_window->featureNum;i++){
        c_window->featureWeights[i]=featureWeight[i]->value()/100.0;
        sprintf(t,"%.2f",featureWeight[i]->value()/100.0);
        curValue[i]->setText(QString(t));
    }

}


setFeatureUsing::~setFeatureUsing(){
}


QSize setFeatureUsing::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize setFeatureUsing::sizeHint() const
{
    return QSize(800, 800);
}

void setFeatureUsing::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape){
    }
}

void setFeatureUsing::releaseSlider(){
    emit c_window->runTSNE();
}

void setFeatureUsing::runAddFeature(){
    QString curFormula;
    QString newName="";
    QString usedFeatures[20];
    int calculationType[20];
    int calculationCnt=0;
    bool IsGood=true;

    curFormula=formula->toPlainText();

    int cur=0;
    while(1){
        if(cur==curFormula.length()){
            return;
        }
        if(curFormula[cur].toLatin1()=='='){
            qDebug()<<newName;
            cur++;
            break;
        }
        newName.append(curFormula[cur]);
        cur++;
    }


    QString curName="";
    while(1){
        if(cur==curFormula.length()){
            qDebug()<<curName;

            usedFeatures[calculationCnt]=curName;

            break;
        }
        if(curFormula[cur].toLatin1()=='+'){
            qDebug()<<curName;

            usedFeatures[calculationCnt]=curName;
            curName="";
            calculationType[calculationCnt]=0;
            calculationCnt++;
        }
        else if(curFormula[cur].toLatin1()=='-'){
            qDebug()<<curName;

            usedFeatures[calculationCnt]=curName;
            curName="";

            calculationType[calculationCnt]=1;
            calculationCnt++;
        }
        else if(curFormula[cur].toLatin1()=='*'){
            qDebug()<<curName;

            usedFeatures[calculationCnt]=curName;
            curName="";

            calculationType[calculationCnt]=2;
            calculationCnt++;
        }
        else if(curFormula[cur].toLatin1()=='/'){
            qDebug()<<curName;

            usedFeatures[calculationCnt]=curName;
            curName="";

            calculationType[calculationCnt]=3;
            calculationCnt++;
        }
        else{
            curName.append(curFormula[cur]);
        }
        cur++;
    }

    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureNames[i]==newName){
            return;
        }
    }

    qDebug()<<curFormula;

    for(int i=0;i<=calculationCnt;i++){
        int j=0;
        for(;j<c_window->featureNum;j++){
            if(usedFeatures[i]==c_window->featureNames[j]){
                break;
            }
        }
        if(j==c_window->featureNum)return;
    }

    qDebug()<<"good";

    for(int i=0;i<calculationCnt;i++){
        if(calculationType[i]<0 || calculationType[i]>3)return;
    }


    qDebug()<<c_window->featureNum;
    qDebug()<<c_window->featureRanges[c_window->featureNum-1].x();
    qDebug()<<c_window->featureRanges[c_window->featureNum-1].y();
    qDebug()<<c_window->featureRanges[c_window->featureNum-2].x();
    qDebug()<<c_window->featureRanges[c_window->featureNum-2].y();

    QVector2D ttt=c_window->featureRanges[c_window->featureNum-1];

    c_window->featureNum++;
    c_window->featureNames.push_back(newName);
    c_window->featureEnables.push_back(false);
    c_window->featureRanges.push_back(QVector2D(1000,-1000));
    c_window->featureRanges[c_window->featureNum-1]=QVector2D(1000,-1000);
    c_window->featureRanges[c_window->featureNum-2]=ttt;

    c_window->featureWeights.push_back(1.0);



    c_window->selection.push_back(false);
    c_window->selectionRange.push_back(QVector2D(0,0));

    qDebug()<<c_window->featureNum;
    qDebug()<<c_window->featureRanges[c_window->featureNum-1].x();
    qDebug()<<c_window->featureRanges[c_window->featureNum-1].y();
    qDebug()<<c_window->featureRanges[c_window->featureNum-2].x();
    qDebug()<<c_window->featureRanges[c_window->featureNum-2].y();

    float newValue;

    for(int i=0;i<c_window->data.length();i++){
        for(int j=0;j<c_window->featureNum;j++){
            if(usedFeatures[0]==c_window->featureNames[j]){
                newValue=c_window->data[i][j];
                break;
            }
        }

        for(int j=0;j<calculationCnt;j++){

            for(int k=0;k<c_window->featureNum;k++){
                if(usedFeatures[j+1]==c_window->featureNames[k]){
                    if(calculationType[j]==0){
                        newValue+=c_window->data[i][k];
                    }
                    else if(calculationType[j]==1){
                        newValue-=c_window->data[i][k];

                    }
                    else if(calculationType[j]==2){
                        newValue*=c_window->data[i][k];

                    }
                    else if(calculationType[j]==3){
                        if(c_window->data[i][k]!=0)newValue/=c_window->data[i][k];

                    }
                    break;
                }
            }
        }

        qDebug()<<newValue;
        c_window->data[i].push_back(newValue);
        if(c_window->featureRanges[c_window->featureNum-1].x()>newValue){
            c_window->featureRanges[c_window->featureNum-1].setX(newValue);
        }
        if(c_window->featureRanges[c_window->featureNum-1].y()<newValue){
            c_window->featureRanges[c_window->featureNum-1].setY(newValue);
        }
    }
    qDebug()<<c_window->featureNum;
    qDebug()<<c_window->featureRanges[c_window->featureNum-1].x();
    qDebug()<<c_window->featureRanges[c_window->featureNum-1].y();
    qDebug()<<c_window->featureRanges[c_window->featureNum-2].x();
    qDebug()<<c_window->featureRanges[c_window->featureNum-2].y();






    featureWeight.push_back(new QSlider(Qt::Vertical));
    featureWeight[c_window->featureNum-1]->setRange(0,100);
    featureWeight[c_window->featureNum-1]->setValue(int(c_window->featureWeights[c_window->featureNum-1]*100));
    featureWeight[c_window->featureNum-1]->setTickInterval(10);
    featureWeight[c_window->featureNum-1]->setTickPosition(QSlider::TicksRight);

    connect(featureWeight[c_window->featureNum-1],SIGNAL(valueChanged(int)),this,SLOT(setFeatureWeight(int)));

    connect(featureWeight[c_window->featureNum-1],SIGNAL(sliderReleased()),this,SLOT(releaseSlider()));
    char t[20];
    sprintf(t,"%.2f",featureWeight[c_window->featureNum-1]->value()/100.0);
    curValue.push_back(new QLabel(QString(t)));

    featureOnOff.push_back(new QCheckBox(c_window->featureNames[c_window->featureNum-1]));
    featureOnOff[c_window->featureNum-1]->setChecked(c_window->featureEnables[c_window->featureNum-1]);



    if(featureOnOff[c_window->featureNum-1]->isChecked()){
        featureOnOff[c_window->featureNum-1]->setStyleSheet("QCheckBox {color: rgb(100,100,255); }");
        curValue[c_window->featureNum-1]->setStyleSheet("QLabel {color: rgb(100,100,255); }");
        featureWeight[c_window->featureNum-1]->setStyleSheet("QSlider::handle:vertical {background-color: rgb(100,100,255);}");

    }
    else{
        featureOnOff[c_window->featureNum-1]->setStyleSheet("QCheckBox {color: rgb(100,100,100); }");
        curValue[c_window->featureNum-1]->setStyleSheet("QLabel {color: rgb(100,100,100); }");
        featureWeight[c_window->featureNum-1]->setStyleSheet("QSlider::handle:vertical {background-color: rgb(100,100,100);}");


    }


    connect(featureOnOff[c_window->featureNum-1],SIGNAL(clicked(bool)),this,SLOT(setFeatureOnOff(bool)));


    QVBoxLayout *each=new QVBoxLayout;

    each->addWidget(featureWeight[c_window->featureNum-1]);
    each->addWidget(curValue[c_window->featureNum-1]);
    each->addWidget(featureOnOff[c_window->featureNum-1]);


    features->addLayout(each);

    c_window->checkSpineEnable();

}
