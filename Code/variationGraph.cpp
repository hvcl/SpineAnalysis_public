///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "variationGraph.h"
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




variationGraph::variationGraph(Window *p, QWidget *parent)
    : QWidget(parent)
{


    c_window=p;

    setBackgroundRole(QPalette::Base);
    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);


}

variationGraph::~variationGraph(){
}

void variationGraph::resizeEvent(QResizeEvent *event){

}

QSize variationGraph::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize variationGraph::sizeHint() const
{
    return QSize(800, 800);
}

void variationGraph::paintEvent(QPaintEvent *)
{



    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,true);

    QPen t_pen;
    QBrush t_brush(Qt::SolidPattern);



    int enabledFeatureNum=0;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){
            enabledFeatureNum++;
        }
    }
    t_pen.setWidth(this->width()*0.75/enabledFeatureNum);
    t_pen.setCapStyle(Qt::FlatCap);
    t_pen.setColor(QColor(245,245,245));
    painter.setPen(t_pen);

    int nn=0;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){
            painter.drawLine(this->width()*0.1+(this->width()*0.8)/(enabledFeatureNum-1)*nn,0,
                             this->width()*0.1+(this->width()*0.8)/(enabledFeatureNum-1)*nn,this->height());
            nn++;
        }

    }


    QFont t_font;
    t_font.setBold(true);
    t_font.setUnderline(true);
    t_pen.setColor(QColor(50,50,50));

    painter.setFont(t_font);
    painter.setPen(t_pen);

    nn=0;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){
            painter.drawText(this->width()*0.1+(this->width()*0.8)/(enabledFeatureNum-1)*nn-5*c_window->featureNames[i].length(),this->height()*0.1,c_window->featureNames[i]);
            nn++;
        }

    }
    t_pen.setColor(QColor(150,150,150));
    t_pen.setWidth(5);
    t_pen.setCapStyle(Qt::RoundCap);

    painter.setPen(t_pen);
    painter.drawLine(0,this->height()*0.55,this->width(),this->height()*0.55);

    int pre=c_window->preGroup->currentIndex()-1;
    int post=c_window->postGroup->currentIndex()-1;
    if(pre>=0 && post>=0){
        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->groups[pre]->numSpine==0){
                variation[i]=0;
                preTotal[i]=0;
            }
            else if(c_window->groups[post]->numSpine==0){
                variation[i]=0;
                preTotal[i]=c_window->groups[pre]->totalFeature[i]/c_window->groups[pre]->numSpine;
            }
            else{
                variation[i]=c_window->groups[post]->totalFeature[i]/c_window->groups[post]->numSpine
                        - c_window->groups[pre]->totalFeature[i]/c_window->groups[pre]->numSpine;
                preTotal[i]=c_window->groups[pre]->totalFeature[i]/c_window->groups[pre]->numSpine;
            }
        }
        maxV=0;
        for(int j=0;j<c_window->featureNum;j++){
            if(c_window->featureEnables[j]){
                if(preTotal[j]==0)continue;
               if(abs(variation[j]/preTotal[j])>maxV)maxV=abs(variation[j]/preTotal[j]);
            }

        }
        if(maxV<1.0)maxV=1.0;



        t_pen.setWidth(30);
        t_pen.setCapStyle(Qt::FlatCap);

        nn=0;
        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->featureEnables[i]){

                float value=0;
                if(variation[i]<0){
                    t_pen.setColor(QColor(255,100,100));
                }
                else{
                    t_pen.setColor(QColor(59,145,111));
                }
                if(preTotal[i]!=0)value=variation[i]/preTotal[i];
                painter.setPen(t_pen);
                painter.drawLine(this->width()*0.1+(this->width()*0.8)/(enabledFeatureNum-1)*nn,this->height()*0.55,
                                 this->width()*0.1+(this->width()*0.8)/(enabledFeatureNum-1)*nn,this->height()*0.55-this->height()*0.4*value/maxV);
                nn++;
            }
        }


        nn=0;
        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->featureEnables[i]){

                float value=0;
                int margin;
                if(variation[i]<0){
                    margin=15;
                }
                else{
                    margin=-5;
                }
                if(preTotal[i]!=0)value=variation[i]/preTotal[i];

                t_font.setUnderline(false);
                t_pen.setColor(QColor(100,100,100));
                painter.setPen(t_pen);
                painter.setFont(t_font);
                char tt[30];
                sprintf(tt,"%.1f%%",value*100);
                painter.drawText(this->width()*0.1+(this->width()*0.8)/(enabledFeatureNum-1)*nn-3.5*strlen(tt),this->height()*0.55-this->height()*0.4*value/maxV+margin,QString(tt));
                nn++;
            }

        }

    }



}

void variationGraph::mousePressEvent(QMouseEvent *event)
{

}
void variationGraph::mouseReleaseEvent(QMouseEvent *event){

}
void variationGraph::mouseMoveEvent(QMouseEvent *event)
{


}

void variationGraph::keyPressEvent(QKeyEvent *event)
{

}


void variationGraph::timerEvent(QTimerEvent *event){
}


void variationGraph::readAnalysisFile(){


}
