///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "imageControlGraph.h"
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




imageControlGraph::imageControlGraph(Window *p, QWidget *parent)
    : QWidget(parent)
{


    c_window=p;

    setBackgroundRole(QPalette::Base);
    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);


}

imageControlGraph::~imageControlGraph(){
}

void imageControlGraph::resizeEvent(QResizeEvent *event){

}

QSize imageControlGraph::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize imageControlGraph::sizeHint() const
{
    return QSize(800, 800);
}

void imageControlGraph::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,true);

    QPen t_pen;
    QBrush t_brush(Qt::SolidPattern);
    t_brush.setColor(QColor(0,0,0));

    painter.setBrush(t_brush);
    painter.drawRect(0,0,this->width(),this->height());

    t_pen.setWidth(1);
    t_pen.setColor(QColor(255,255,255));
    painter.setPen(t_pen);

    for(int i=1;i<256;i++){
        QPoint startPos;
        QPoint endPos;
        startPos.setX(float(this->width())/256*(i-1));
        endPos.setX(float(this->width())/256*i);

        startPos.setY(this->height()-this->height()*c_window->dataRendering->color_table[i-1].x());
        endPos.setY(this->height()-this->height()*c_window->dataRendering->color_table[i].x());

        painter.drawLine(startPos,endPos);
    }

    t_pen.setWidth(3);
    t_pen.setStyle(Qt::DashLine);
    painter.setPen(t_pen);
    painter.drawLine(0,this->height()-this->height()*float(c_window->imageBrightControl->value())/255.0,
                     this->width(),this->height()-this->height()*float(c_window->imageBrightControl->value())/255.0);

    painter.drawLine(this->width()*float(c_window->imageContrastPosControl->value())/255.0,0,
                     this->width()*float(c_window->imageContrastPosControl->value())/255.0,this->height());

}

void imageControlGraph::mousePressEvent(QMouseEvent *event)
{

}
void imageControlGraph::mouseReleaseEvent(QMouseEvent *event){

}
void imageControlGraph::mouseMoveEvent(QMouseEvent *event)
{


}

void imageControlGraph::keyPressEvent(QKeyEvent *event)
{

}


void imageControlGraph::timerEvent(QTimerEvent *event){
}


