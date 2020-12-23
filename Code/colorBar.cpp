///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "colorBar.h"
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
#include <QSlider>



colorBar::colorBar(Window *p, QWidget *parent)
    : QWidget(parent)
{
    c_window=p;
    minV=0;
    maxV=1;
    ControlMin=false;
    ControlMax=false;
}

colorBar::~colorBar(){
}

void colorBar::resizeEvent(QResizeEvent *event){
    //    int w=event->size().width();
    //    int h=event->size().height();
    //    this->resize(w,h);
}

QSize colorBar::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize colorBar::sizeHint() const
{
    return QSize(800, 800);
}

void colorBar::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,true);
    //     painter.drawPixmap(0,0,this->width(),this->height(),background);//resolution

//    float minV=c_window->SetMin->value()/100.0;
//    float maxV=c_window->SetMax->value()/100.0;



    //(0,0,0.5) (0.2,0.7,0.6) (1,1,0)
    // 0             0.5         1

    QPen t_pen;
    QBrush t_brush(Qt::SolidPattern);


    QLinearGradient gradient;
    gradient.setStart(0,this->height()/2.0);
    gradient.setFinalStop(this->width(),this->height()/2.0);
    gradient.setColorAt(minV,QColor(0,0,128));
    gradient.setColorAt((maxV+minV)/2.0,QColor(0.2*255,0.7*255,0.6*255));
    gradient.setColorAt(maxV, QColor(255,255,0));

    t_pen=QPen(gradient,this->height());
    painter.setPen(t_pen);
    painter.drawLine(0,this->height()/2.0,this->width(),this->height()/2.0);

    t_pen=QPen();
    t_pen.setColor(QColor(0,0,0));
    t_pen.setWidth(1);
    painter.setPen(t_pen);
    painter.drawLine(this->width()/2,this->height(),this->width()/2,0);

    painter.setPen(Qt::NoPen);

    t_brush.setColor(QColor(255,150,150));
    painter.setBrush(t_brush);
    painter.drawEllipse(QPoint(this->width()*minV,this->height()*0.5),8,int(this->height()*0.5));

    painter.drawEllipse(QPoint(this->width()*maxV,this->height()*0.5),8,int(this->height()*0.5));

    t_pen.setColor(QColor(120,120,120));
    t_pen.setWidth(1);
    painter.setPen(t_pen);

    char tt[10];
    sprintf(tt,"%.2f",minV);
    painter.drawText(QPoint(this->width()*minV,this->height()*0.5),QString(tt));
    sprintf(tt,"%.2f",maxV);
    painter.drawText(QPoint(this->width()*maxV,this->height()*0.5),QString(tt));

}


void colorBar::timerEvent(QTimerEvent *event){
}
void colorBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        if(abs(event->x()-this->width()*minV)<10){
            ControlMin=true;
            ControlMax=false;
        }
        if(abs(event->x()-this->width()*maxV)<10 && abs(event->x()-this->width()*maxV) < abs(event->x()-this->width()*minV)){
            ControlMin=false;
            ControlMax=true;
        }
    }

    m_lastPos = event->pos();


}
void colorBar::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        ControlMin=false;
        ControlMax=false;
//        if(c_window->curvatureIndex!=-1){
//            c_window->featureRanges[c_window->curvatureIndex]=QVector2D(0,0.001);
//            emit changeDone();
//            c_window->synchronization();




//            c_window->updateAvg();

//        }

    }
}
void colorBar::mouseMoveEvent(QMouseEvent *event)
{

    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();


    if (event->buttons() & Qt::LeftButton) {
        if(ControlMin){
            minV+=float(dx)/this->width();
            if(minV<0)minV=0;
            if(minV>=maxV)minV=maxV-0.01;
            emit minValueChanged(minV);
            update();
        }
        else if(ControlMax){
            maxV+=float(dx)/this->width();
            if(maxV>1)maxV=1;
            if(minV>=maxV)maxV=minV+0.01;
            emit maxValueChanged(maxV);
            update();
        }

    }
    if (event->buttons() & Qt::RightButton) {
    }
    if (event->buttons() & Qt::MiddleButton) {
    }
    m_lastPos = event->pos();

}

