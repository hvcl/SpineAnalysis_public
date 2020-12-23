///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "typeChangeGraph.h"
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




typeChangeGraph::typeChangeGraph(int g,Window *p, QWidget *parent)
    : QWidget(parent)
{

    group=g;

    c_window=p;

    setBackgroundRole(QPalette::Base);
    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);


}

typeChangeGraph::~typeChangeGraph(){
}

void typeChangeGraph::resizeEvent(QResizeEvent *event){

}

QSize typeChangeGraph::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize typeChangeGraph::sizeHint() const
{
    return QSize(800, 800);
}

void typeChangeGraph::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,true);

    QPen t_pen;
    QBrush t_brush(Qt::SolidPattern);

    t_pen.setColor(c_window->groups[group]->color);
    painter.setPen(t_pen);
    float r=this->width()*0.42;
    if(r>this->height()*0.42)r=this->height()*0.42;
    painter.drawRect(this->width()*0.5-r,this->height()*0.55-r,r*2,r*2);
    painter.drawText(this->width()*0.5-r,this->height()*0.55-r-15,r*2,15,Qt::AlignLeft,c_window->groups[group]->name->text());


    painter.setPen(Qt::NoPen);
    //t_brush.setColor(QColor(0,0,255,40));


    //t_pen.setWidth(30);

    int gcnt[4];
    int totalcnt=0;
    int enableGroupCnt=0;
    for(int i=0;i<4;i++)gcnt[i]=0;

    for(int i=0;i<c_window->groups[group]->dataList->count();i++){
        int index=c_window->groups[group]->dataList->item(i)->whatsThis().toInt();
        //qDebug()<<"index: "<<index;
        int len=c_window->dataRendering->dataList[index]->spineIndex.length();
        for(int j=0;j<len;j++){
            int spineIndex=c_window->dataRendering->dataList[index]->spineIndex[j];
            //qDebug()<<"spineIndex: "<<spineIndex;

            gcnt[c_window->clusterLabel[spineIndex]]++;
            totalcnt++;
        }
    }
    if(totalcnt!=0){
        for(int i=0;i<4;i++){
            if(gcnt[i]!=0)enableGroupCnt++;
        }

        int curDegree=0;
        for(int i=0;i<4;i++){
            if(gcnt[i]==0)continue;
            //t_brush.setColor(c_window->clusterColors[i]);
            t_pen.setColor(c_window->clusterColors[i]);

            float r=this->width()*0.4;
            if(r>this->height()*0.4)r=this->height()*0.4;
            t_pen.setWidth(r*0.8);
            t_pen.setCapStyle(Qt::FlatCap);

            painter.setBrush(Qt::NoBrush);
            painter.setPen(t_pen);
            QPointF t=QPointF(this->width()*0.5,this->height()*0.55);


            painter.drawArc(t.x()-r*0.6,t.y()-r*0.6,r*1.2,r*1.2,- 360*8/4 + curDegree,360.0*16*gcnt[i]/totalcnt);
            float midDegree=- 360*8/4 + curDegree+180.0*16*gcnt[i]/totalcnt;
            midDegree/=16;
            curDegree+=360.0*16*gcnt[i]/totalcnt;

        }
        curDegree=0;
        for(int i=0;i<4;i++){
            if(gcnt[i]==0)continue;

            float r=this->width()*0.4;
            if(r>this->height()*0.4)r=this->height()*0.4;
            QPointF t=QPointF(this->width()*0.5,this->height()*0.55);
            float midDegree=- 360*8/4 + curDegree+180.0*16*gcnt[i]/totalcnt;
            midDegree/=16;
            curDegree+=360.0*16*gcnt[i]/totalcnt;

            QFont tFont;
            tFont.setBold(true);
            painter.setFont(tFont);
            t_pen.setColor(QColor(255,255,255));
            painter.setPen(t_pen);
            painter.setBrush(Qt::NoBrush);

            QPointF t2=QPointF(cos(M_PI * (-midDegree)/180 ),sin(M_PI * (-midDegree)/180 ))*r*0.6 + t;

            QString str=c_window->clusterOnOff[i]->text();
            //str.remove(str.length()-3,3);
            str+="\n";
            str+=QString::number(float(gcnt[i])/totalcnt*100,'g',4) + "%";
            painter.drawText(t2.x()-40,t2.y()-30,80,60,Qt::AlignCenter,str);

        }
    }
}

void typeChangeGraph::mousePressEvent(QMouseEvent *event)
{

}
void typeChangeGraph::mouseReleaseEvent(QMouseEvent *event){

}
void typeChangeGraph::mouseMoveEvent(QMouseEvent *event)
{


}

void typeChangeGraph::keyPressEvent(QKeyEvent *event)
{

}


void typeChangeGraph::timerEvent(QTimerEvent *event){
}


