///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "circularCoordinate.h"
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

CircularCoordinate::CircularCoordinate(Window *p, QWidget *parent)
    : QWidget(parent)
{

    c_window=p;
    readAnalysisFile();

    c_window->focusSpine=-1;
    c_window->clicked=false;

    selectionFrame=-1;
    doSelect=false;



    for(int i=0;i<c_window->featureNum;i++){
        c_window->selection.push_back(false);
        c_window->selectionRange.push_back(QVector2D(0,0));
    }
    for(int i=0;i<30;i++){
        featurePoints.push_back(QVector2D(0,0));
    }

    setBackgroundRole(QPalette::Base);
    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);


}

CircularCoordinate::~CircularCoordinate(){
}

void CircularCoordinate::resizeEvent(QResizeEvent *event){
    //    int w=event->size().width();
    //    int h=event->size().height();
    //    this->resize(w,h);
}

QSize CircularCoordinate::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize CircularCoordinate::sizeHint() const
{
    return QSize(800, 800);
}

void CircularCoordinate::paintEvent(QPaintEvent *)
{

    //qDebug()<<"start circular coord";

    float lineW=4*c_window->WINDOW_SCALE;
    float pointW=3*c_window->WINDOW_SCALE;


    int enabledFeatureNum=0;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i])enabledFeatureNum++;
    }


    ////Get coordinate
    midPoint=QVector2D(this->size().width()/2,this->size().height()/2);

    int cnt=0;
    float r=midPoint.x()<midPoint.y()?midPoint.x()*0.8:midPoint.y()*0.8;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){
            featurePoints[i]=QVector2D(sin(2*M_PI/enabledFeatureNum*cnt),cos(2*M_PI/enabledFeatureNum*cnt))*r+midPoint;
            cnt++;
        }
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,true);
    //     painter.drawPixmap(0,0,this->width(),this->height(),background);//resolution

    QPen t_pen;
    QBrush t_brush(Qt::SolidPattern);

    ////Draw frame

    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){

            QLinearGradient gradient;
            gradient.setStart(featurePoints[i].x()*0.1+midPoint.x()*0.9,featurePoints[i].y()*0.1+midPoint.y()*0.9);
            gradient.setFinalStop(featurePoints[i].x(),featurePoints[i].y());
            gradient.setColorAt(0.0,QColor(200,200,200));
            gradient.setColorAt(1.0, QColor(100,100,100));
            t_pen=QPen(gradient,2*c_window->WINDOW_SCALE);

            t_pen.setStyle(Qt::DotLine);
            painter.setPen(t_pen);
            painter.drawLine(featurePoints[i].x()*0.1+midPoint.x()*0.9,featurePoints[i].y()*0.1+midPoint.y()*0.9
                             ,featurePoints[i].x(),featurePoints[i].y());

        }
    }
    t_pen.setStyle(Qt::DotLine);
    t_pen.setWidth(2*c_window->WINDOW_SCALE);
    t_pen.setColor(QColor(100,100,100));
    painter.setPen(t_pen);
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){
            for(int j=(i+1);;j++){
                if(c_window->featureEnables[j%c_window->featureNum]){
                    painter.drawLine(featurePoints[i].x(),featurePoints[i].y(),
                                     featurePoints[j%c_window->featureNum].x(),featurePoints[j%c_window->featureNum].y());
                    break;
                }
            }
        }
    }

    t_pen.setColor(QColor(200,200,200));
    painter.setPen(t_pen);
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){
            for(int j=(i+1);;j++){
                if(c_window->featureEnables[j%c_window->featureNum]){
                    painter.drawLine(featurePoints[i].x()*0.1+midPoint.x()*0.9,featurePoints[i].y()*0.1+midPoint.y()*0.9,
                                     featurePoints[j%c_window->featureNum].x()*0.1+midPoint.x()*0.9,featurePoints[j%c_window->featureNum].y()*0.1+midPoint.y()*0.9);
                    break;
                }
            }
        }
    }



    QVector3D meanColor(0,0,0);
    int cntColor=0;

    if(c_window->totalFrame->currentIndex()==1){
        ////Draw each spine graph

        if(c_window->focusSpine==-1){
            t_pen.setStyle(Qt::SolidLine);
            for(int i=0;i<c_window->data.length();i++){
                if(c_window->spineEnable[i]==false){

                    for(int j=0;j<c_window->featureNum;j++){
                        if(c_window->featureEnables[j]){
                            float value1,value2;
                            QPointF p1,p2;

                            value1=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                            p1=QPointF(featurePoints[j].x()*value1+(featurePoints[j].x()*0.1+midPoint.x()*0.9)*(1-value1),
                                       featurePoints[j].y()*value1+(featurePoints[j].y()*0.1+midPoint.y()*0.9)*(1-value1));

                            for(int k=j+1;;k++){
                                if(c_window->featureEnables[k%c_window->featureNum]){
                                    value2=(c_window->data[i][k%c_window->featureNum]-c_window->featureRanges[k%c_window->featureNum].x())/(c_window->featureRanges[k%c_window->featureNum].y()-c_window->featureRanges[k%c_window->featureNum].x());
                                    p2=QPointF(featurePoints[k%c_window->featureNum].x()*value2+(featurePoints[k%c_window->featureNum].x()*0.1+midPoint.x()*0.9)*(1-value2),
                                            featurePoints[k%c_window->featureNum].y()*value2+(featurePoints[k%c_window->featureNum].y()*0.1+midPoint.y()*0.9)*(1-value2));
                                    break;
                                }
                            }

                            t_pen.setWidth(lineW);
                            t_pen.setColor(QColor(50,50,50,30));
                            painter.setPen(t_pen);
    //                        painter.drawEllipse(p1,pointW,pointW);
    //                        painter.drawLine(p1,p2);

                        }
                    }
                }
            }


            for(int i=0;i<c_window->data.length();i++){
                if(c_window->spineEnable[i]){

                    for(int j=0;j<c_window->featureNum;j++){
                        if(c_window->featureEnables[j]){
                            float value1,value2;
                            QPointF p1,p2;

                            value1=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                            p1=QPointF(featurePoints[j].x()*value1+(featurePoints[j].x()*0.1+midPoint.x()*0.9)*(1-value1),
                                       featurePoints[j].y()*value1+(featurePoints[j].y()*0.1+midPoint.y()*0.9)*(1-value1));

                            for(int k=j+1;;k++){
                                if(c_window->featureEnables[k%c_window->featureNum]){
                                    value2=(c_window->data[i][k%c_window->featureNum]-c_window->featureRanges[k%c_window->featureNum].x())/(c_window->featureRanges[k%c_window->featureNum].y()-c_window->featureRanges[k%c_window->featureNum].x());
                                    p2=QPointF(featurePoints[k%c_window->featureNum].x()*value2+(featurePoints[k%c_window->featureNum].x()*0.1+midPoint.x()*0.9)*(1-value2),
                                            featurePoints[k%c_window->featureNum].y()*value2+(featurePoints[k%c_window->featureNum].y()*0.1+midPoint.y()*0.9)*(1-value2));
                                    break;
                                }
                            }

                            t_pen.setWidth(lineW);

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

                            painter.setPen(t_pen);
    //                        painter.drawEllipse(p1,pointW,pointW);
    //                        painter.drawLine(p1,p2);

                        }
                    }
                    meanColor+=QVector3D(t_pen.color().red(),t_pen.color().green(),t_pen.color().blue());
                    cntColor++;
                }
            }
        }
        else{
            t_pen.setStyle(Qt::SolidLine);
            for(int i=0;i<c_window->data.length();i++){
                for(int j=0;j<c_window->featureNum;j++){
                    if(c_window->featureEnables[j]){

                        float value1,value2;
                        QPointF p1,p2;

                        value1=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                        p1=QPointF(featurePoints[j].x()*value1+(featurePoints[j].x()*0.1+midPoint.x()*0.9)*(1-value1),
                                   featurePoints[j].y()*value1+(featurePoints[j].y()*0.1+midPoint.y()*0.9)*(1-value1));

                        for(int k=j+1;;k++){
                            if(c_window->featureEnables[k%c_window->featureNum]){
                                value2=(c_window->data[i][k%c_window->featureNum]-c_window->featureRanges[k%c_window->featureNum].x())/(c_window->featureRanges[k%c_window->featureNum].y()-c_window->featureRanges[k%c_window->featureNum].x());
                                p2=QPointF(featurePoints[k%c_window->featureNum].x()*value2+(featurePoints[k%c_window->featureNum].x()*0.1+midPoint.x()*0.9)*(1-value2),
                                        featurePoints[k%c_window->featureNum].y()*value2+(featurePoints[k%c_window->featureNum].y()*0.1+midPoint.y()*0.9)*(1-value2));
                                break;
                            }
                        }
                        t_pen.setWidth(lineW);
                        t_pen.setColor(QColor(50,50,50,30));
                        painter.setPen(t_pen);
    //                    painter.drawEllipse(p1,pointW,pointW);
    //                    painter.drawLine(p1,p2);
                    }
                }
            }

            for(int j=0;j<c_window->featureNum;j++){
                if(c_window->featureEnables[j]){

                    float value1,value2;
                    QPointF p1,p2;

                    value1=(c_window->data[c_window->focusSpine][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                    p1=QPointF(featurePoints[j].x()*value1+(featurePoints[j].x()*0.1+midPoint.x()*0.9)*(1-value1),
                               featurePoints[j].y()*value1+(featurePoints[j].y()*0.1+midPoint.y()*0.9)*(1-value1));

                    for(int k=j+1;;k++){
                        if(c_window->featureEnables[k%c_window->featureNum]){
                            value2=(c_window->data[c_window->focusSpine][k%c_window->featureNum]-c_window->featureRanges[k%c_window->featureNum].x())/(c_window->featureRanges[k%c_window->featureNum].y()-c_window->featureRanges[k%c_window->featureNum].x());
                            p2=QPointF(featurePoints[k%c_window->featureNum].x()*value2+(featurePoints[k%c_window->featureNum].x()*0.1+midPoint.x()*0.9)*(1-value2),
                                    featurePoints[k%c_window->featureNum].y()*value2+(featurePoints[k%c_window->featureNum].y()*0.1+midPoint.y()*0.9)*(1-value2));
                            break;
                        }
                    }
                    t_pen.setWidth(lineW);
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

    //                t_pen.setColor(c_window->spineColors[c_window->focusSpine]);
                    painter.setPen(t_pen);
    //                painter.drawEllipse(p1,pointW,pointW);
    //                painter.drawLine(p1,p2);
                }
            }

            meanColor+=QVector3D(t_pen.color().red(),t_pen.color().green(),t_pen.color().blue());
            cntColor++;
        }
    }


    ////Draw Flow
    ///

    for(int g=1;g<4;g++){
        //if(!c_window->clusterOnOff[g]->isChecked())continue;

        if(c_window->nFlow[g]==0)continue;
        t_pen.setColor(c_window->clusterColors[g]);
        t_brush.setColor(QColor(c_window->clusterColors[g].red(),c_window->clusterColors[g].green(),c_window->clusterColors[g].blue()
                                ,70));

        painter.setBrush(t_brush);



        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->featureEnables[i]){
                int front=i;
                int end=(i+1)%c_window->featureNum;
                while(1){
                    if(c_window->featureEnables[end])break;
                    end=(end+1)%c_window->featureNum;
                }
                float fMin;//=(c_window->minValues[front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
                float eMin;//=(c_window->minValues[end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());
                float fMax;//=(c_window->avgValues[front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
                float eMax;//=(c_window->avgValues[end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());
                fMax=(c_window->avgFlow[g][front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
                eMax=(c_window->avgFlow[g][end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());

                fMin=0,eMin=0;

                QPointF poly[4];
                poly[0]=QPointF(featurePoints[front].x()*fMin+(featurePoints[front].x()*0.1+midPoint.x()*0.9)*(1-fMin),
                                featurePoints[front].y()*fMin+(featurePoints[front].y()*0.1+midPoint.y()*0.9)*(1-fMin));
                poly[1]=QPointF(featurePoints[front].x()*fMax+(featurePoints[front].x()*0.1+midPoint.x()*0.9)*(1-fMax),
                                featurePoints[front].y()*fMax+(featurePoints[front].y()*0.1+midPoint.y()*0.9)*(1-fMax));
                poly[2]=QPointF(featurePoints[end].x()*eMax+(featurePoints[end].x()*0.1+midPoint.x()*0.9)*(1-eMax),
                                featurePoints[end].y()*eMax+(featurePoints[end].y()*0.1+midPoint.y()*0.9)*(1-eMax));
                poly[3]=QPointF(featurePoints[end].x()*eMin+(featurePoints[end].x()*0.1+midPoint.x()*0.9)*(1-eMin),
                                featurePoints[end].y()*eMin+(featurePoints[end].y()*0.1+midPoint.y()*0.9)*(1-eMin));

//                painter.setPen(t_pen);
//                painter.drawLine(poly[1],poly[2]);


                painter.setPen(Qt::NoPen);
                painter.drawPolygon(poly,4);


            }
        }

    }


    if(c_window->totalFrame->currentIndex()==0){
        int glen=c_window->groups.length();
        for(int i=0;i<glen;i++){
            if(!c_window->datagroupOnOff[i]->isChecked())continue;
            if(c_window->groups[i]->numSpine==0)continue;
            t_pen.setColor(c_window->groups[i]->color);
            t_pen.setWidth(lineW);
            t_pen.setStyle(Qt::SolidLine);

            t_brush.setColor(c_window->groups[i]->color);
            t_brush.setStyle(Qt::BDiagPattern);


            for(int j=0;j<c_window->featureNum;j++){
                if(c_window->featureEnables[j]){
                    int front=j;
                    int end=(j+1)%c_window->featureNum;
                    while(1){
                        if(c_window->featureEnables[end])break;
                        end=(end+1)%c_window->featureNum;

                    }
                    float fMin;
                    float eMin;
                    float fMax;
                    float eMax;

                    fMax=(c_window->groups[i]->totalFeature[front]/c_window->groups[i]->numSpine-c_window->featureRanges[front].x())
                            /(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
                    eMax=(c_window->groups[i]->totalFeature[end]/c_window->groups[i]->numSpine-c_window->featureRanges[end].x())
                            /(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());

                    fMin=0,eMin=0;

                    QPointF poly[4];
                    poly[0]=QPointF(featurePoints[front].x()*fMin+(featurePoints[front].x()*0.1+midPoint.x()*0.9)*(1-fMin),
                                    featurePoints[front].y()*fMin+(featurePoints[front].y()*0.1+midPoint.y()*0.9)*(1-fMin));
                    poly[1]=QPointF(featurePoints[front].x()*fMax+(featurePoints[front].x()*0.1+midPoint.x()*0.9)*(1-fMax),
                                    featurePoints[front].y()*fMax+(featurePoints[front].y()*0.1+midPoint.y()*0.9)*(1-fMax));
                    poly[2]=QPointF(featurePoints[end].x()*eMax+(featurePoints[end].x()*0.1+midPoint.x()*0.9)*(1-eMax),
                                    featurePoints[end].y()*eMax+(featurePoints[end].y()*0.1+midPoint.y()*0.9)*(1-eMax));
                    poly[3]=QPointF(featurePoints[end].x()*eMin+(featurePoints[end].x()*0.1+midPoint.x()*0.9)*(1-eMin),
                                    featurePoints[end].y()*eMin+(featurePoints[end].y()*0.1+midPoint.y()*0.9)*(1-eMin));
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(t_brush);
                    painter.drawPolygon(poly,4);
                    painter.setPen(t_pen);
                    painter.setBrush(Qt::NoBrush);
                    painter.drawLine(poly[1],poly[2]);
                }
            }
        }
        t_brush.setStyle(Qt::SolidPattern);

    }


    QString probability="";

    if(c_window->totalFrame->currentIndex()==1){
        ////Draw Shape
        if(cntColor!=0){

            QColor flowColor(0,0,0,255);
            float featureDistance[4];
            for(int i=0;i<4;i++){
                featureDistance[i]=0;
            }

            for(int g=0;g<4;g++){
                for(int i=0;i<c_window->featureNum;i++){
                    if(c_window->featureEnables[i]){
                        float a1=(c_window->avgFlow[g][i]-c_window->featureRanges[i].x())/(c_window->featureRanges[i].y()-c_window->featureRanges[i].x());
                        float b1=(c_window->avgValues[i]-c_window->featureRanges[i].x())/(c_window->featureRanges[i].y()-c_window->featureRanges[i].x());
                        if(c_window->focusSpine!=-1){
                            b1=(c_window->data[c_window->focusSpine][i]-c_window->featureRanges[i].x())/(c_window->featureRanges[i].y()-c_window->featureRanges[i].x());
                        }
                        featureDistance[g]+=(a1-b1)*(a1-b1);
                    }
                }
                featureDistance[g]=sqrt(featureDistance[g]);

//                for(int i=0;i<c_window->featureNum;i++){
//                    if(c_window->featureEnables[i]){
//                        int front=i;
//                        int end=(i+1)%c_window->featureNum;
//                        while(1){
//                            if(c_window->featureEnables[end])break;
//                            end=(end+1)%c_window->featureNum;
//                        }
//                        float a1=(c_window->avgFlow[g][front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
//                        float a2=(c_window->avgFlow[g][end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());

//                        float b1=(c_window->avgValues[front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
//                        float b2=(c_window->avgValues[end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());
//                        if(c_window->focusSpine!=-1){
//                            b1=(c_window->data[c_window->focusSpine][front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
//                            b2=(c_window->data[c_window->focusSpine][end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());
//                        }
//                        featureDistance[g]+=0.5*abs(a1*a2-b1*b2)*sin(2*M_PI/c_window->featureNum);
//                    }
//                }

            }
            //qDebug()<<featureDistance[0]<<" "<<featureDistance[1]<<" "<<featureDistance[2]<<" "<<featureDistance[3];

            float minFlow=10000;
            float maxFlow=-10000;
            for(int g=0;g<4;g++){
                if(minFlow>featureDistance[g])minFlow=featureDistance[g];
                if(maxFlow<featureDistance[g])maxFlow=featureDistance[g];
            }
            for(int g=0;g<4;g++){
                if(maxFlow!=0)featureDistance[g]=featureDistance[g]/maxFlow;
            }

            //qDebug()<<featureDistance[0]<<" "<<featureDistance[1]<<" "<<featureDistance[2]<<" "<<featureDistance[3];


            int index[4];
            for(int g=0;g<4;g++)index[g]=g;
            for(int i=0;i<4;i++){
                for(int j=i+1;j<4;j++){
                    if(featureDistance[index[i]]>featureDistance[index[j]]){
                        int t=index[i];
                        index[i]=index[j];
                        index[j]=t;
                    }
                }
            }

            float totalFlow=0;
            for(int g=0;g<4;g++){
                float t=(1-featureDistance[index[g]]);
                if(t+totalFlow>1.0){
                    t=1-totalFlow;
                    totalFlow=1;
                }
                else{
                    totalFlow+=t;
                }
                flowColor=QColor(flowColor.red()+c_window->clusterColors[index[g]].red()*t,
                                 flowColor.green()+c_window->clusterColors[index[g]].green()*t,
                                 flowColor.blue()+c_window->clusterColors[index[g]].blue()*t,255);


                if(index[g]==0)probability=probability+"Irregular: "+QString::number(t*100,'g',3)+"%\n";
                if(index[g]==1)probability=probability+"Mushroom: "+QString::number(t*100,'g',3)+"%\n";
                if(index[g]==2)probability=probability+"Stubby: "+QString::number(t*100,'g',3)+"%\n";
                if(index[g]==3)probability=probability+"Thin: "+QString::number(t*100,'g',3)+"%\n";
            }

            //qDebug()<<featureDistance[index[0]]<<" "<<featureDistance[index[1]]<<" "<<featureDistance[index[2]]<<" "<<featureDistance[index[3]];



            t_pen.setColor(flowColor);
            t_pen.setWidth(4*c_window->WINDOW_SCALE);

            t_brush.setColor(flowColor);
            t_brush.setStyle(Qt::BDiagPattern);

            painter.setPen(Qt::NoPen);

            int groupEnableNum=0;
            for(int i=0;i<c_window->GroupNumSet->value();i++){
                if(c_window->clusterOnOff[i]->isChecked())groupEnableNum++;
            }
    //        if(c_window->focusSpine==-1)
    //            t_brush.setColor(QColor(0,150,255,180));
            painter.setBrush(t_brush);

            for(int i=0;i<c_window->featureNum;i++){
                if(c_window->featureEnables[i]){
                    int front=i;
                    int end=(i+1)%c_window->featureNum;
                    while(1){
                        if(c_window->featureEnables[end])break;
                        end=(end+1)%c_window->featureNum;

                    }
                    float fMin;//=(c_window->minValues[front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
                    float eMin;//=(c_window->minValues[end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());
                    float fMax;//=(c_window->avgValues[front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
                    float eMax;//=(c_window->avgValues[end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());
                    if(c_window->focusSpine==-1){
                        fMax=(c_window->avgValues[front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
                        eMax=(c_window->avgValues[end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());
                    }
                    else{
                        fMax=(c_window->data[c_window->focusSpine][front]-c_window->featureRanges[front].x())/(c_window->featureRanges[front].y()-c_window->featureRanges[front].x());
                        eMax=(c_window->data[c_window->focusSpine][end]-c_window->featureRanges[end].x())/(c_window->featureRanges[end].y()-c_window->featureRanges[end].x());
                    }

                    fMin=0,eMin=0;

                    QPointF poly[4];
                    poly[0]=QPointF(featurePoints[front].x()*fMin+(featurePoints[front].x()*0.1+midPoint.x()*0.9)*(1-fMin),
                                    featurePoints[front].y()*fMin+(featurePoints[front].y()*0.1+midPoint.y()*0.9)*(1-fMin));
                    poly[1]=QPointF(featurePoints[front].x()*fMax+(featurePoints[front].x()*0.1+midPoint.x()*0.9)*(1-fMax),
                                    featurePoints[front].y()*fMax+(featurePoints[front].y()*0.1+midPoint.y()*0.9)*(1-fMax));
                    poly[2]=QPointF(featurePoints[end].x()*eMax+(featurePoints[end].x()*0.1+midPoint.x()*0.9)*(1-eMax),
                                    featurePoints[end].y()*eMax+(featurePoints[end].y()*0.1+midPoint.y()*0.9)*(1-eMax));
                    poly[3]=QPointF(featurePoints[end].x()*eMin+(featurePoints[end].x()*0.1+midPoint.x()*0.9)*(1-eMin),
                                    featurePoints[end].y()*eMin+(featurePoints[end].y()*0.1+midPoint.y()*0.9)*(1-eMin));

                    painter.drawPolygon(poly,4);
                    painter.setPen(t_pen);
                    painter.drawLine(poly[1],poly[2]);
                    painter.setPen(Qt::NoPen);


                }
            }



            painter.setBrush(Qt::NoBrush);
            painter.setPen(t_pen);



        }
    }
    t_brush.setStyle(Qt::SolidPattern);


//    ////Draw Selection frame & selection information
//    if(selectionFrame!=-1){
//        if(c_window->featureEnables[selectionFrame]){

//            t_pen.setColor(QColor(0,255,0,40));
//            t_pen.setWidth(50);
//            t_pen.setCapStyle(Qt::RoundCap);
//            painter.setPen(t_pen);
//            painter.drawLine(featurePoints[selectionFrame].x()*0.1+midPoint.x()*0.9,featurePoints[selectionFrame].y()*0.1+midPoint.y()*0.9
//                             ,featurePoints[selectionFrame].x(),featurePoints[selectionFrame].y());
//        }

//    }


//    for(int i=0;i<c_window->featureNum;i++){
//        if(c_window->featureEnables[i]){
//            if(c_window->selection[i]){
//                t_pen.setColor(QColor(35,255,159,200));
//                t_pen.setWidth(20);
//                t_pen.setCapStyle(Qt::RoundCap);
//                painter.setPen(t_pen);
//                QPointF p1(featurePoints[i].x()*0.1+midPoint.x()*0.9,featurePoints[i].y()*0.1+midPoint.y()*0.9);
//                QPointF p2(featurePoints[i].x(),featurePoints[i].y());
//                painter.drawLine(p1*(1-c_window->selectionRange[i].x())+p2*c_window->selectionRange[i].x(),p1*(1-c_window->selectionRange[i].y())+p2*c_window->selectionRange[i].y());

//            }
//        }
//    }

    ////Draw Label
    QFont tFont;
    tFont.setBold(true);
    painter.setFont(tFont);
    t_pen.setColor(QColor(0,0,0));
    painter.setPen(t_pen);

    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){
            float tx=featurePoints[i].x()*1.1-midPoint.x()*0.1;
            float ty=featurePoints[i].y()*1.1-midPoint.y()*0.1;
            painter.drawText(QRectF(tx-20*c_window->WINDOW_SCALE,ty-20*c_window->WINDOW_SCALE,40*c_window->WINDOW_SCALE,40*c_window->WINDOW_SCALE),Qt::AlignCenter,c_window->featureNames[i]);
        }
    }

    if(c_window->totalFrame->currentIndex()==1){
        painter.drawText(this->width()*0.55,this->height()*0.65,100*c_window->WINDOW_SCALE,100*c_window->WINDOW_SCALE,Qt::TextWordWrap,probability);
    }


//        t_pen.setWidth(2);
//        t_pen.setColor(QColor(160,160,160));
//        painter.setPen(t_pen);
//        painter.drawLine(0,this->height()*0.4,0,this->height()*0.6);
//        painter.drawLine(0,this->height()*0.4,this->width(),0);
//        painter.drawLine(this->width(),0,this->width(),this->height());
//        painter.drawLine(0,this->height()*0.6,this->width(),this->height());



//        painter.setPen(Qt::NoPen);

//        for(int i=0;i<size;i++){
//            t_brush.setColor(QColor(colors[i].x()*255,colors[i].y()*255,colors[i].z()*255));
//            painter.setBrush(t_brush);

//            QPoint tt[4];

//            tt[0]=QPoint((points[i]-pen_size)*this->width(),this->height()*0.6 * (1.0-(points[i]-pen_size)) + this->height() * (points[i]-pen_size));
//            tt[1]=QPoint((points[i]+pen_size)*this->width(),this->height()*0.6 * (1.0-(points[i]+pen_size)) + this->height() * (points[i]+pen_size));
//            tt[2]=QPoint((points[i]+pen_size)*this->width(),this->height()*0.4 * (1.0-(points[i]+pen_size)));
//            tt[3]=QPoint((points[i]-pen_size)*this->width(),this->height()*0.4 * (1.0-(points[i]-pen_size)));

//            painter.drawPolygon(tt,4);
//        }


//        char t_f[15];

//        for(int i=0;i<size;i++){
//            t_pen.setColor(QColor(colors[i].x()*150,colors[i].y()*150,colors[i].z()*150));
//            painter.setPen(t_pen);
//            float t_v=points[i]*max_value;
//            sprintf(t_f,"%.2f",t_v);
//            painter.drawText(points[i]*this->width(),this->height()*0.6 * (1.0-points[i]) + this->height() * points[i] + 20,tr(t_f));

//        }

    //qDebug()<<"end circular coord";


}

void CircularCoordinate::mousePressEvent(QMouseEvent *event)
{
//    if(event->x()<0 || event->x()>this->width() || event->y()<0 || event->y()>this->height())return;

//    lastPos = event->pos();
//    button=event->button();

//    if(c_window->spineEnableMode->isChecked()){
//        if(c_window->focusSpine!=-1){
//            c_window->displaySpines[c_window->focusSpine]=true;
//        }
//        return;
//    }


//    if(event->button()==Qt::LeftButton){
//        if(c_window->focusSpine!=-1){
//            if(c_window->IsShift){
//                c_window->addSpineToGroup(c_window->focusSpine,c_window->currentSelectGroup);
//            }
//            else{
//                c_window->clicked=true;
//                emit viewChange(c_window->focusSpine);
//            }
//        }
//    }
//    else if(event->button()==Qt::MidButton){
//    }
//    else if(event->button() == Qt::RightButton){
//        if(doSelect==false && selectionFrame!=-1){
//            doSelect=true;
//            c_window->selection[selectionFrame]=true;
//            c_window->focusSpine=-1;
//            if(c_window->clicked==true){
//                c_window->focusSpine=-1;
//                c_window->clicked=false;
//                emit viewChange(-1);
//            }
//            QVector2D p1(featurePoints[selectionFrame].x()*0.1+midPoint.x()*0.9,featurePoints[selectionFrame].y()*0.1+midPoint.y()*0.9);
//            float d1=(featurePoints[selectionFrame]-p1).length();
//            float d2=(p1-QVector2D(event->x(),event->y())).length();
//            float d3=(featurePoints[selectionFrame]-QVector2D(event->x(),event->y())).length();

//            startSelectValue=d2/d1;
//            if(d1<d3)startSelectValue=-startSelectValue;
//            c_window->selectionRange[selectionFrame]=QVector2D(d2/d1,d2/d1);
//        }
//    }
//    update();
//    emit synchronization();

}
void CircularCoordinate::mouseReleaseEvent(QMouseEvent *event){

//    if(event->button() == Qt::RightButton){
//        if(doSelect==true){
//            doSelect=false;
//            if(c_window->selectionRange[selectionFrame].y()-c_window->selectionRange[selectionFrame].x()<0.01){
//                c_window->selection[selectionFrame]=false;
//            }
//            c_window->checkSpineEnable();
//            update();
//            emit synchronization();
//            ////Todo:
//            ////update spine grid using selection info
//        }
//    }

}
void CircularCoordinate::mouseMoveEvent(QMouseEvent *event)
{
//    setFocus();

//    if(event->x()<0 || event->x()>this->width() || event->y()<0 || event->y()>this->height())return;

//    float tx=event->x();
//    float ty=event->y();

//    if(doSelect){
//        QPointF p1(featurePoints[selectionFrame].x()*0.1+midPoint.x()*0.9,featurePoints[selectionFrame].y()*0.1+midPoint.y()*0.9);
//        QPointF p2(featurePoints[selectionFrame].x(),featurePoints[selectionFrame].y());
//        if(abs(p1.x()-p2.x())<1.0){
//            if(((ty<=p1.y()+10 && ty>=p2.y()-10)||(ty<=p2.y()+10 && ty>=p1.y()-10))){
//                float dis=abs(tx-p1.x());
//                if(dis>25){
//                    doSelect=false;
//                    if(c_window->selectionRange[selectionFrame].y()-c_window->selectionRange[selectionFrame].x()<0.01){
//                        c_window->selection[selectionFrame]=false;
//                    }
//                    ////Todo:
//                    ////update spine grid using selection info
//                }
//                else{
//                    QVector2D p(featurePoints[selectionFrame].x()*0.1+midPoint.x()*0.9,featurePoints[selectionFrame].y()*0.1+midPoint.y()*0.9);
//                    float d1=(featurePoints[selectionFrame]-p).length();
//                    float d2=(p-QVector2D(event->x(),event->y())).length();
//                    float d3=(featurePoints[selectionFrame]-QVector2D(event->x(),event->y())).length();

//                    float v=d2/d1;
//                    if(d1<d3)v=-v;
//                    if(startSelectValue>v){
//                        c_window->selectionRange[selectionFrame].setX(v);
//                        c_window->selectionRange[selectionFrame].setY(startSelectValue);
//                    }
//                    if(startSelectValue<v){
//                        c_window->selectionRange[selectionFrame].setX(startSelectValue);
//                        c_window->selectionRange[selectionFrame].setY(v);
//                    }

//                }
//            }
//        }
//        else if(abs(p1.y()-p2.y())<1.0){
//            if(((tx<=p1.x()+10 && tx>=p2.x()-10)||(tx<=p2.x()+10 && tx>=p1.x()-10))){

//                float dis=abs(ty-p1.y());
//                if(dis>25){
//                    doSelect=false;
//                    if(c_window->selectionRange[selectionFrame].y()-c_window->selectionRange[selectionFrame].x()<0.01){
//                        c_window->selection[selectionFrame]=false;
//                    }
//                    ////Todo:
//                    ////update spine grid using selection info
//                }
//                else{
//                    QVector2D p(featurePoints[selectionFrame].x()*0.1+midPoint.x()*0.9,featurePoints[selectionFrame].y()*0.1+midPoint.y()*0.9);
//                    float d1=(featurePoints[selectionFrame]-p).length();
//                    float d2=(p-QVector2D(event->x(),event->y())).length();
//                    float d3=(featurePoints[selectionFrame]-QVector2D(event->x(),event->y())).length();

//                    float v=d2/d1;
//                    if(d1<d3)v=-v;
//                    if(startSelectValue>v){
//                        c_window->selectionRange[selectionFrame].setX(v);
//                        c_window->selectionRange[selectionFrame].setY(startSelectValue);
//                    }
//                    if(startSelectValue<v){
//                        c_window->selectionRange[selectionFrame].setX(startSelectValue);
//                        c_window->selectionRange[selectionFrame].setY(v);
//                    }

//                }
//            }
//        }
//        else{
//            if(((tx<=p1.x()+10 && tx>=p2.x()-10)||(tx<=p2.x()+10 && tx>=p1.x()-10))
//                    && ((ty<=p1.y()+10 && ty>=p2.y()-10)||(ty<=p2.y()+10 && ty>=p1.y()-10))){

//                float dis=abs(ty-p1.y()-((p2.y()-p1.y())/(p2.x()-p1.x()))*(tx-p1.x()))
//                        /sqrt(1+( (p2.y()-p1.y())/(p2.x()-p1.x()) )*( (p2.y()-p1.y())/(p2.x()-p1.x()) ));
//                if(dis>25){
//                    doSelect=false;
//                    if(c_window->selectionRange[selectionFrame].y()-c_window->selectionRange[selectionFrame].x()<0.01){
//                        c_window->selection[selectionFrame]=false;
//                    }
//                    ////Todo:
//                    ////update spine grid using selection info
//                }
//                else{
//                    QVector2D p(featurePoints[selectionFrame].x()*0.1+midPoint.x()*0.9,featurePoints[selectionFrame].y()*0.1+midPoint.y()*0.9);
//                    float d1=(featurePoints[selectionFrame]-p).length();
//                    float d2=(p-QVector2D(event->x(),event->y())).length();
//                    float d3=(featurePoints[selectionFrame]-QVector2D(event->x(),event->y())).length();

//                    float v=d2/d1;
//                    if(d1<d3)v=-v;
//                    if(startSelectValue>v){
//                        c_window->selectionRange[selectionFrame].setX(v);
//                        c_window->selectionRange[selectionFrame].setY(startSelectValue);
//                    }
//                    if(startSelectValue<v){
//                        c_window->selectionRange[selectionFrame].setX(startSelectValue);
//                        c_window->selectionRange[selectionFrame].setY(v);
//                    }

//                }
//            }
//        }
//        c_window->checkSpineEnable();
//        update();
//        emit synchronization();

//        return;
//    }


//    int prev=selectionFrame;
//    selectionFrame=-1;
//    float minDis=10000;
//    for(int i=0;i<c_window->featureNum;i++){

//        if(c_window->featureEnables[i]){
//            QPointF p1(featurePoints[i].x()*0.1+midPoint.x()*0.9,featurePoints[i].y()*0.1+midPoint.y()*0.9);
//            QPointF p2(featurePoints[i].x(),featurePoints[i].y());
//            if(abs(p1.x()-p2.x())<1.0){
//                if(((ty<=p1.y()+10 && ty>=p2.y()-10)||(ty<=p2.y()+10 && ty>=p1.y()-10))){
//                    float dis=abs(tx-p1.x());
//                    if(dis<minDis && dis<25){
//                        minDis=dis;
//                        selectionFrame=i;
//                    }
//                }
//            }
//            else if(abs(p1.y()-p2.y())<1.0){
//                if(((tx<=p1.x()+10 && tx>=p2.x()-10)||(tx<=p2.x()+10 && tx>=p1.x()-10))){

//                    float dis=abs(ty-p1.y());

//                    if(dis<minDis && dis<25){
//                        minDis=dis;
//                        selectionFrame=i;
//                    }
//                }
//            }
//            else{
//                if(((tx<=p1.x()+10 && tx>=p2.x()-10)||(tx<=p2.x()+10 && tx>=p1.x()-10))
//                        && ((ty<=p1.y()+10 && ty>=p2.y()-10)||(ty<=p2.y()+10 && ty>=p1.y()-10))){

//                    float dis=abs(ty-p1.y()-((p2.y()-p1.y())/(p2.x()-p1.x()))*(tx-p1.x()))
//                            /sqrt(1+( (p2.y()-p1.y())/(p2.x()-p1.x()) )*( (p2.y()-p1.y())/(p2.x()-p1.x()) ));

//                    if(dis<minDis && dis<25){
//                        minDis=dis;
//                        selectionFrame=i;
//                    }
//                }
//            }
//        }

//    }
//    if(prev!=selectionFrame){
//        update();
//        emit synchronization();
//    }


//    if(c_window->clicked)return;

//    minDis=10000;
//    prev=c_window->focusSpine;
//    c_window->focusSpine=-1;

//    if(!c_window->isFocus)return;
//    for(int i=0;i<c_window->data.length();i++){
//        if(c_window->spineEnable[i]==false){
//            continue;
//        }
//        for(int j=0;j<c_window->featureNum;j++){
//            if(c_window->featureEnables[j]){

//                float value1,value2;
//                QPointF p1,p2;

//                value1=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
//                p1=QPointF(featurePoints[j].x()*value1+(featurePoints[j].x()*0.1+midPoint.x()*0.9)*(1-value1),
//                           featurePoints[j].y()*value1+(featurePoints[j].y()*0.1+midPoint.y()*0.9)*(1-value1));

//                for(int k=j+1;;k++){
//                    if(c_window->featureEnables[k%c_window->featureNum]){
//                        value2=(c_window->data[i][k%c_window->featureNum]-c_window->featureRanges[k%c_window->featureNum].x())/(c_window->featureRanges[k%c_window->featureNum].y()-c_window->featureRanges[k%c_window->featureNum].x());
//                        p2=QPointF(featurePoints[k%c_window->featureNum].x()*value2+(featurePoints[k%c_window->featureNum].x()*0.1+midPoint.x()*0.9)*(1-value2),
//                                featurePoints[k%c_window->featureNum].y()*value2+(featurePoints[k%c_window->featureNum].y()*0.1+midPoint.y()*0.9)*(1-value2));
//                        break;
//                    }
//                }


//                if(((tx<=p1.x()+10 && tx>=p2.x()-10)||(tx<=p2.x()+10 && tx>=p1.x()-10))
//                        && ((ty<=p1.y()+10 && ty>=p2.y()-10)||(ty<=p2.y()+10 && ty>=p1.y()-10))){

//                    float dis=abs(ty-p1.y()-((p2.y()-p1.y())/(p2.x()-p1.x()))*(tx-p1.x()))
//                            /sqrt(1+( (p2.y()-p1.y())/(p2.x()-p1.x()) )*( (p2.y()-p1.y())/(p2.x()-p1.x()) ));

//                    if(dis<minDis && dis<25){
//                        minDis=dis;
//                        c_window->focusSpine=i;
//                    }
//                }
//            }
//        }
//    }
//    if(prev!=c_window->focusSpine){
//        c_window->changeTitle();

//        update();
//        emit synchronization();
//    }

}

void CircularCoordinate::keyPressEvent(QKeyEvent *event)
{
//    if(event->key()==Qt::Key_Escape){
//        if(c_window->clicked==true){
//            c_window->clicked=false;
//            c_window->focusSpine=-1;
//            emit viewChange(-1);
//        }
//    }

//    if(event->key()==Qt::Key_Shift){
//        c_window->IsShift=true;
//    }
}
void CircularCoordinate::keyReleaseEvent(QKeyEvent *event)
{
//    if(event->key()==Qt::Key_Shift){
//        c_window->IsShift=false;
//    }
}


void CircularCoordinate::timerEvent(QTimerEvent *event){
}


void CircularCoordinate::readAnalysisFile(){
    c_window->typeEnable=false;
    c_window->testTrainEnable=false;
    QFile File("spineFeatureSetting.csv");

    if (File.open(QIODevice::ReadOnly))
    {
        QTextStream input(&File);
        QString headerLine = input.readLine();
        QStringList headers=headerLine.split(QChar(','),QString::KeepEmptyParts);

//        c_window->featureNames=new QString[headers.count()-1];

        for(int i=0;i<headers.count()-1;i++){
            c_window->featureNames.push_back(QString(""));
        }

        for(int i=1;i<headers.count();i++){
            c_window->featureNames[i-1]=headers[i];
            if(headers[i]=="Type"){
                c_window->typeEnable=true;
            }
            else if(headers[i]=="TrainTest"){
                c_window->testTrainEnable=true;
            }
        }
        if(c_window->typeEnable && c_window->testTrainEnable){
            c_window->featureNum=headers.count()-3;
        }
        else if(c_window->testTrainEnable){
            c_window->featureNum=headers.count()-2;
        }

        else if(c_window->typeEnable){
            c_window->featureNum=headers.count()-2;
        }
        else{
            c_window->featureNum=headers.count()-1;
        }

        for(int i=0;i<c_window->featureNum;i++){
            c_window->featureRanges.push_back(QVector2D(1000,-1000));
        }

        File.close();
    }


//    if(c_window->typeEnable && c_window->testTrainEnable){
//        c_window->featureNames.pop_back();
//        c_window->featureNames.pop_back();

//    }
//    else if(c_window->testTrainEnable){
//        c_window->featureNames.pop_back();
//    }

//    else if(c_window->typeEnable){
//        c_window->featureNames.pop_back();
//    }

}

