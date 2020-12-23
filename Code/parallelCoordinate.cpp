///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "parallelCoordinate.h"
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

ParallelCoordinate::ParallelCoordinate(Window *p, QWidget *parent)
    : QWidget(parent)
{

    c_window=p;

    selectionFrame=-1;
    doSelect=false;

    axisStarts=new QVector2D[30];
    axisEnds=new QVector2D[30];

    setBackgroundRole(QPalette::Base);
    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);


}

ParallelCoordinate::~ParallelCoordinate(){
}

void ParallelCoordinate::resizeEvent(QResizeEvent *event){
    //    int w=event->size().width();
    //    int h=event->size().height();
    //    this->resize(w,h);
}

QSize ParallelCoordinate::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize ParallelCoordinate::sizeHint() const
{
    return QSize(800, 800);
}

void ParallelCoordinate::paintEvent(QPaintEvent *)
{

//    qDebug()<<"start parallelcoord";


    float lineW=2*c_window->WINDOW_SCALE;
    float pointW=2*c_window->WINDOW_SCALE;

    int enabledFeatureNum=0;
    int endIndex=0;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i])enabledFeatureNum++,endIndex=i;
    }


    ////Get coordinate
    ///
    int cnt=0;
    if(enabledFeatureNum==1){
        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->featureEnables[i]){
                axisStarts[i]=QVector2D(this->width()*0.5,this->height()*0.9);
                axisEnds[i]=QVector2D(this->width()*0.5,this->height()*0.3);
            }
        }
    }
    else{
        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->featureEnables[i]){
                axisStarts[i]=QVector2D(this->width()*0.1+this->width()*0.8/(enabledFeatureNum-1)*cnt,this->height()*0.9);
                axisEnds[i]=QVector2D(this->width()*0.1+this->width()*0.8/(enabledFeatureNum-1)*cnt,this->height()*0.3);
                cnt++;
            }
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
            gradient.setStart(axisStarts[i].x(),axisStarts[i].y());
            gradient.setFinalStop(axisEnds[i].x(),axisEnds[i].y());
            gradient.setColorAt(0.0,QColor(200,200,200));
            gradient.setColorAt(1.0, QColor(100,100,100));
            t_pen=QPen(gradient,2);
            t_pen.setStyle(Qt::DotLine);
            painter.setPen(t_pen);
            painter.drawLine(axisStarts[i].x(),axisStarts[i].y()
                             ,axisEnds[i].x(),axisEnds[i].y());
        }
    }


    ////Draw flow
    for(int g=1;g<4;g++){
        QVector<QPointF> flow;
        int cnt=0;
        for(int j=0;j<c_window->featureNum;j++){
            if(c_window->featureEnables[j]){
                cnt+=2;
                float value1,value2;
                QPointF p1;

                value1=(c_window->maxFlow[g][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                value2=(c_window->minFlow[g][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());

                flow.push_back(QPointF(axisEnds[j].x()*value1+axisStarts[j].x()*(1-value1),
                           axisEnds[j].y()*value1+axisStarts[j].y()*(1-value1)));
                flow.push_front(QPointF(axisEnds[j].x()*value2+axisStarts[j].x()*(1-value2),
                                        axisEnds[j].y()*value2+axisStarts[j].y()*(1-value2)));


            }
        }
        t_brush.setColor(QColor(c_window->clusterColors[g].red(),c_window->clusterColors[g].green(),c_window->clusterColors[g].blue()
                                ,50));

        painter.setBrush(t_brush);
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(flow.data(),cnt);
        painter.setBrush(Qt::NoBrush);

    }



    ////Draw each spine graph

    if(c_window->focusSpine==-1){
        t_pen.setStyle(Qt::SolidLine);
        for(int i=0;i<c_window->data.length();i++){

            if(c_window->spineEnable[i]==false){
                for(int j=0;j<c_window->featureNum-1;j++){
                    if(c_window->featureEnables[j]){

                        float value1, value2;
                        QPointF p1,p2;

                        value1=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                        p1=QPointF(axisEnds[j].x()*value1+axisStarts[j].x()*(1-value1),
                                   axisEnds[j].y()*value1+axisStarts[j].y()*(1-value1));
                        int k;
                        for(k=j+1;k<c_window->featureNum;k++){
                            if(c_window->featureEnables[k]){
                                value2=(c_window->data[i][k]-c_window->featureRanges[k].x())/(c_window->featureRanges[k].y()-c_window->featureRanges[k].x());
                                p2=QPointF(axisEnds[k].x()*value2+axisStarts[k].x()*(1-value2),
                                           axisEnds[k].y()*value2+axisStarts[k].y()*(1-value2));
                                break;
                            }
                        }


                        t_pen.setWidth(lineW);
                        t_pen.setColor(QColor(50,50,50,15));
                        painter.setPen(t_pen);
                        painter.drawEllipse(p1,pointW,pointW);
                        painter.drawLine(p1,p2);
                        if(k==endIndex){
                            painter.drawEllipse(p2,pointW,pointW);
                            break;
                        }
                    }
                }
            }
        }


        for(int i=0;i<c_window->data.length();i++){
            if(c_window->spineEnable[i]){
                for(int j=0;j<c_window->featureNum-1;j++){
                    if(c_window->featureEnables[j]){

                        float value1, value2;
                        QPointF p1,p2;

                        value1=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                        p1=QPointF(axisEnds[j].x()*value1+axisStarts[j].x()*(1-value1),
                                   axisEnds[j].y()*value1+axisStarts[j].y()*(1-value1));
                        int k;
                        for(k=j+1;k<c_window->featureNum;k++){
                            if(c_window->featureEnables[k]){
                                value2=(c_window->data[i][k]-c_window->featureRanges[k].x())/(c_window->featureRanges[k].y()-c_window->featureRanges[k].x());
                                p2=QPointF(axisEnds[k].x()*value2+axisStarts[k].x()*(1-value2),
                                           axisEnds[k].y()*value2+axisStarts[k].y()*(1-value2));
                                break;
                            }
                        }


                        t_pen.setWidth(lineW);
//                        if(c_window->totalFrame->currentIndex()==0){
//                            t_pen.setColor(c_window->spineColors[i]);
//                        }
                        if(c_window->coloringType==0){
                            t_pen.setColor(c_window->spineColors[i]);
                        }
                        else if(c_window->coloringType==1){
                            t_pen.setColor(c_window->clusterColors[c_window->clusterLabel[i]]);
                        }
                        else if(c_window->coloringType==2){
                            if(c_window->indexOfSelectedSpines[i]==-1){
                                t_pen.setColor(QColor(50,50,50,15));
                            }
                            else t_pen.setColor(c_window->clusterColors[c_window->indexOfSelectedSpines[i]]);
                        }
                        else if(c_window->coloringType==3 && c_window->typeEnable){
                            t_pen.setColor(c_window->typeColors[c_window->typesInt[i]]);
                        }
                        else if(c_window->coloringType==4){
                            t_pen.setColor(c_window->groups[c_window->spineGroups[i]]->color);
                        }

                        //t_pen.setColor(c_window->spineColors[i]);
                        painter.setPen(t_pen);
                        painter.drawEllipse(p1,pointW,pointW);
                        painter.drawLine(p1,p2);
                        if(k==endIndex){
                            painter.drawEllipse(p2,pointW,pointW);
                            break;
                        }
                    }
                }
            }
        }
    }
    else{
        t_pen.setStyle(Qt::SolidLine);
        for(int i=0;i<c_window->data.length();i++){
            for(int j=0;j<c_window->featureNum-1;j++){
                if(c_window->featureEnables[j]){

                    float value1, value2;
                    QPointF p1,p2;

                    value1=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                    p1=QPointF(axisEnds[j].x()*value1+axisStarts[j].x()*(1-value1),
                               axisEnds[j].y()*value1+axisStarts[j].y()*(1-value1));
                    int k;
                    for(k=j+1;k<c_window->featureNum;k++){
                        if(c_window->featureEnables[k]){
                            value2=(c_window->data[i][k]-c_window->featureRanges[k].x())/(c_window->featureRanges[k].y()-c_window->featureRanges[k].x());
                            p2=QPointF(axisEnds[k].x()*value2+axisStarts[k].x()*(1-value2),
                                       axisEnds[k].y()*value2+axisStarts[k].y()*(1-value2));
                            break;
                        }
                    }


                    t_pen.setWidth(lineW);
                    t_pen.setColor(QColor(50,50,50,15));
                    painter.setPen(t_pen);
                    painter.drawEllipse(p1,pointW,pointW);
                    painter.drawLine(p1,p2);
                    if(k==endIndex){
                        painter.drawEllipse(p2,pointW,pointW);
                        break;
                    }
                }
            }
        }
        for(int j=0;j<c_window->featureNum-1;j++){
            if(c_window->featureEnables[j]){

                float value1, value2;
                QPointF p1,p2;

                value1=(c_window->data[c_window->focusSpine][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                p1=QPointF(axisEnds[j].x()*value1+axisStarts[j].x()*(1-value1),
                           axisEnds[j].y()*value1+axisStarts[j].y()*(1-value1));
                int k;
                for(k=j+1;k<c_window->featureNum;k++){
                    if(c_window->featureEnables[k]){
                        value2=(c_window->data[c_window->focusSpine][k]-c_window->featureRanges[k].x())/(c_window->featureRanges[k].y()-c_window->featureRanges[k].x());
                        p2=QPointF(axisEnds[k].x()*value2+axisStarts[k].x()*(1-value2),
                                   axisEnds[k].y()*value2+axisStarts[k].y()*(1-value2));
                        break;
                    }
                }


                t_pen.setWidth(lineW);
//                if(c_window->totalFrame->currentIndex()==0){
//                    t_pen.setColor(c_window->spineColors[c_window->focusSpine]);
//                }
                if(c_window->coloringType==0){
                    t_pen.setColor(c_window->spineColors[c_window->focusSpine]);
                }
                else if(c_window->coloringType==1){
                    t_pen.setColor(c_window->clusterColors[c_window->clusterLabel[c_window->focusSpine]]);
                }
                else if(c_window->coloringType==2){
                    if(c_window->indexOfSelectedSpines[c_window->focusSpine]==-1){
                        t_pen.setColor(QColor(50,50,50,15));
                    }
                    else t_pen.setColor(c_window->clusterColors[c_window->indexOfSelectedSpines[c_window->focusSpine]]);
                }
                else if(c_window->coloringType==3 && c_window->typeEnable){
                    t_pen.setColor(c_window->typeColors[c_window->typesInt[c_window->focusSpine]]);
                }
                else if(c_window->coloringType==4){
                    t_pen.setColor(c_window->groups[c_window->spineGroups[c_window->focusSpine]]->color);
                }

 //               t_pen.setColor(c_window->spineColors[c_window->focusSpine]);
                painter.setPen(t_pen);
                painter.drawEllipse(p1,pointW,pointW);
                painter.drawLine(p1,p2);
                if(k==endIndex){
                    painter.drawEllipse(p2,pointW,pointW);
                    break;
                }
            }
        }

    }


    if(c_window->totalFrame->currentIndex()==1){
        ////Draw Selection frame & selection information

        if(selectionFrame!=-1){
            if(c_window->featureEnables[selectionFrame]){
                t_pen.setColor(QColor(0,255,0,40));
                t_pen.setWidth(50*c_window->WINDOW_SCALE);
                t_pen.setCapStyle(Qt::RoundCap);
                painter.setPen(t_pen);
                painter.drawLine(axisStarts[selectionFrame].x(),axisStarts[selectionFrame].y()
                                 ,axisEnds[selectionFrame].x(),axisEnds[selectionFrame].y());
            }
        }

        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->featureEnables[i]){

                if(c_window->selection[i]){
                    t_pen.setColor(QColor(35,255,159,200));
                    t_pen.setWidth(20*c_window->WINDOW_SCALE);
                    t_pen.setCapStyle(Qt::RoundCap);
                    painter.setPen(t_pen);
                    QPointF p1(axisStarts[i].x(),axisStarts[i].y());
                    QPointF p2(axisEnds[i].x(),axisEnds[i].y());
                    painter.drawLine(p1*(1-c_window->selectionRange[i].x())+p2*c_window->selectionRange[i].x(),p1*(1-c_window->selectionRange[i].y())+p2*c_window->selectionRange[i].y());

                }
            }
        }
    }

    ////Draw Label
    t_pen.setColor(QColor(0,0,0));
    painter.setPen(t_pen);
    QFont tFont;
    tFont.setBold(true);
    painter.setFont(tFont);
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){
            painter.drawText(axisEnds[i].x()-c_window->featureNames[i].length()*5,axisEnds[i].y()-this->height()*0.05,c_window->featureNames[i]);
        }

    }
    tFont.setBold(false);
    painter.setFont(tFont);
    if(c_window->focusSpine==-1){
//        painter.drawText(0,axisEnds[0].y()-this->height()*0.1,tr("min"));
//        painter.drawText(0,axisEnds[0].y()-this->height()*0.15,tr("avg"));
//        painter.drawText(0,axisEnds[0].y()-this->height()*0.2,tr("max"));

        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->featureEnables[i]){
                char tt[10];
                sprintf(tt,"%0.2f",c_window->minValues[i]);
                painter.drawText(axisEnds[i].x()-10,axisStarts[i].y()+this->height()*0.05,tr(tt));
                //sprintf(tt,"%0.2f",c_window->avgValues[i]);
                //painter.drawText(axisEnds[i].x()-10,axisEnds[i].y()-this->height()*0.15,tr(tt));
                sprintf(tt,"%0.2f",c_window->maxValues[i]);
                painter.drawText(axisEnds[i].x()-10,axisEnds[i].y()-this->height()*0.1,tr(tt));
            }

        }
    }
    else{
        painter.drawText(0,axisEnds[0].y()-this->height()*0.1,tr("val"));

        for(int i=0;i<c_window->featureNum;i++){
            if(c_window->featureEnables[i]){
                char tt[10];
                sprintf(tt,"%0.2f",c_window->data[c_window->focusSpine][i]);
                painter.drawText(axisEnds[i].x()-10,axisEnds[i].y()-this->height()*0.1,tr(tt));
            }

        }
    }
//    qDebug()<<"end parallel";


}

void ParallelCoordinate::mousePressEvent(QMouseEvent *event)
{
    if(event->x()<0 || event->x()>this->width() || event->y()<0 || event->y()>this->height())return;

    lastPos = event->pos();
    button=event->button();

    if(c_window->totalFrame->currentIndex()==0){
        if(event->button()==Qt::LeftButton){
            if(c_window->clicked==false && c_window->focusSpine!=-1){
                c_window->clicked=true;
                emit synchronization();

            }
            else if(c_window->clicked==true){
                c_window->clicked=false;
                emit synchronization();
            }
            update();
        }

    }
    else{
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
            if(doSelect==false && selectionFrame!=-1){
                doSelect=true;
                c_window->selection[selectionFrame]=true;
                c_window->focusSpine=-1;
                if(c_window->clicked==true){
                    c_window->focusSpine=-1;
                    c_window->clicked=false;

                    emit viewChange(-1);
                }
                float d=(event->y()-axisStarts[selectionFrame].y())/(axisEnds[selectionFrame].y()-axisStarts[selectionFrame].y());
                c_window->selectionRange[selectionFrame]=QVector2D(d,d);
                startSelectValue=d;
            }
        }
        update();
        emit synchronization();
    }


}
void ParallelCoordinate::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        if(doSelect==true){
            doSelect=false;
            if(c_window->selectionRange[selectionFrame].y()-c_window->selectionRange[selectionFrame].x()<0.01){
                c_window->selection[selectionFrame]=false;
            }
            c_window->checkSpineEnable();
            update();
            emit synchronization();

            ////Todo:
            ////update spine grid using selection info
        }
    }
}
void ParallelCoordinate::mouseMoveEvent(QMouseEvent *event)
{
    setFocus();

    if(event->x()<0 || event->x()>this->width() || event->y()<0 || event->y()>this->height())return;

    float tx=event->x();
    float ty=event->y();

    if(doSelect){
        QPointF p1(axisStarts[selectionFrame].x(),axisStarts[selectionFrame].y());
        QPointF p2(axisEnds[selectionFrame].x(),axisEnds[selectionFrame].y());
        if(ty<=p1.y()+10 && ty>=p2.y()-10){
            float dis=abs(tx-p1.x());
            if(dis>25){
                doSelect=false;
                if(c_window->selectionRange[selectionFrame].y()-c_window->selectionRange[selectionFrame].x()<0.01){
                    c_window->selection[selectionFrame]=false;
                }
            }
            else{
                float v=(event->y()-axisStarts[selectionFrame].y())/(axisEnds[selectionFrame].y()-axisStarts[selectionFrame].y());

                if(startSelectValue>v){
                    c_window->selectionRange[selectionFrame].setX(v);
                    c_window->selectionRange[selectionFrame].setY(startSelectValue);
                }
                if(startSelectValue<v){
                    c_window->selectionRange[selectionFrame].setX(startSelectValue);
                    c_window->selectionRange[selectionFrame].setY(v);
                }

            }
        }
        c_window->checkSpineEnable();
        update();
        emit synchronization();

        return;
    }


    int prev=selectionFrame;
    selectionFrame=-1;
    float minDis=10000;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureEnables[i]){

            QPointF p1(axisStarts[i].x(),axisStarts[i].y());
            QPointF p2(axisEnds[i].x(),axisEnds[i].y());
            if(ty<=p1.y()+10 && ty>=p2.y()-10){
                float dis=abs(tx-p1.x());
                if(dis<minDis && dis<25){
                    minDis=dis;
                    selectionFrame=i;
                }
            }
        }
    }
    if(prev!=selectionFrame){
        update();
        emit synchronization();
    }



    if(c_window->clicked)return;

    minDis=10000;
    prev=c_window->focusSpine;
    c_window->focusSpine=-1;

    if(!c_window->isFocus)return;

    for(int i=0;i<c_window->data.length();i++){
        if(c_window->spineEnable[i]==false){
            continue;
        }
        for(int j=0;j<c_window->featureNum-1;j++){

            if(c_window->featureEnables[j]){

                float value1, value2;
                QPointF p1,p2;

                value1=(c_window->data[i][j]-c_window->featureRanges[j].x())/(c_window->featureRanges[j].y()-c_window->featureRanges[j].x());
                p1=QPointF(axisEnds[j].x()*value1+axisStarts[j].x()*(1-value1),
                           axisEnds[j].y()*value1+axisStarts[j].y()*(1-value1));
                int k;
                for(k=j+1;k<c_window->featureNum;k++){
                    if(c_window->featureEnables[k]){
                        value2=(c_window->data[i][k]-c_window->featureRanges[k].x())/(c_window->featureRanges[k].y()-c_window->featureRanges[k].x());
                        p2=QPointF(axisEnds[k].x()*value2+axisStarts[k].x()*(1-value2),
                                   axisEnds[k].y()*value2+axisStarts[k].y()*(1-value2));
                        break;
                    }
                }

                if(k!=c_window->featureNum){

                    if(((tx<=p1.x()+10 && tx>=p2.x()-10)||(tx<=p2.x()+10 && tx>=p1.x()-10))
                            && ((ty<=p1.y()+10 && ty>=p2.y()-10)||(ty<=p2.y()+10 && ty>=p1.y()-10))){

                        float dis=abs(ty-p1.y()-((p2.y()-p1.y())/(p2.x()-p1.x()))*(tx-p1.x()))
                                /sqrt(1+( (p2.y()-p1.y())/(p2.x()-p1.x()) )*( (p2.y()-p1.y())/(p2.x()-p1.x()) ));

                        if(dis<minDis && dis<25){
                            minDis=dis;
                            c_window->focusSpine=i;
                        }
                    }
                }
            }
        }
    }
    if(prev!=c_window->focusSpine){
        c_window->changeTitle();

        update();
        emit synchronization();
    }

}

void ParallelCoordinate::keyPressEvent(QKeyEvent *event)
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
void ParallelCoordinate::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Shift){
        c_window->IsShift=false;
    }
}

void ParallelCoordinate::timerEvent(QTimerEvent *event){
}


void ParallelCoordinate::readAnalysisFile(){


}
