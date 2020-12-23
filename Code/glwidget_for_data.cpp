///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////


#include "glwidget_for_data.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QProcess>
#include <QColorDialog>
#include <window.h>
#include <QVector2D>
#include <QPainter>
#include <QLineEdit>


#define PI 3.14159265358979323846


QVector3D GlWidgetForData::cross_product(QVector3D v1,QVector3D v2){
    QVector3D v;
    v.setX(v1.y()*v2.z()-v2.y()*v1.z());
    v.setY(v1.z()*v2.x()-v2.z()*v1.x());
    v.setZ(v1.x()*v2.y()-v2.x()*v1.y());
    return v;
}

GlWidgetForData::GlWidgetForData(Window *p, QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(0)
{

    p_fps="";
    fps_start=-1;
    fps=0;

    c_window=p;

    EYE=5;
    sampling_rate=1;

    phong_enable=0;

    center1=QVector3D(0,0,0);
    eye_position=QVector3D(0,0,EYE);
    up=QVector3D(0,1,0);
    shift=QVector3D(0,0,0);
    scale=1;


    ambi=0.0,diffu=1.0,specu=0.1,shin=8;

    tog=1;
    startTimer(10);
    QWidget::setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true);

//    filename=f_name;

//    w=c_window->w;
//    h=c_window->h;
//    d=c_window->d;
//    type=c_window->type;

    //background_color=QVector4D(0.95,0.95,0.95,1);
    background_color=QVector4D(0,0,0,1);

    l_t=0.5;

}

GlWidgetForData::~GlWidgetForData()
{
    cleanup();
}

QSize GlWidgetForData::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize GlWidgetForData::sizeHint() const
{
    return QSize(1600, 1600);
}



void GlWidgetForData::cleanup()
{
    makeCurrent();
    delete m_program;
    m_program = 0;
    doneCurrent();
}



void GlWidgetForData::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GlWidgetForData::cleanup);
    initializeOpenGLFunctions();
    glClearColor(1, 1, 1,1);

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile("Shaders/vshader_3D_view.glsl");

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile("Shaders/fshader_3D_view.glsl");


    m_program = new QOpenGLShaderProgram;
    m_program->addShader(vshader);
    m_program->addShader(fshader);

    m_program->link();
    m_program->bind();
    con_tex=m_program->uniformLocation("tex");
    con_tex_label=m_program->uniformLocation("tex_label");

    con_eye_pos=m_program->uniformLocation("eye_position");
    con_box_min=m_program->uniformLocation("box_min");
    con_box_max=m_program->uniformLocation("box_max");
    con_up=m_program->uniformLocation("up");
    con_sample=m_program->uniformLocation("sample");

    con_phong_enable=m_program->uniformLocation("phong_enable");

    con_l_t=m_program->uniformLocation("l_t");
    con_specu=m_program->uniformLocation("Uspecu");
    con_diffu=m_program->uniformLocation("Udiffu");
    con_ambi=m_program->uniformLocation("Uambi");
    con_shin=m_program->uniformLocation("Ushin");

    con_sampling_rate=m_program->uniformLocation("sampling_rate");

    con_window_size=m_program->uniformLocation("window_size");

    con_roi_center=m_program->uniformLocation("roi_center");
    con_roi_size=m_program->uniformLocation("roi_size");

    roi_center=QVector3D(0,0,0);

    con_v_size=m_program->uniformLocation("v_size");
    //m_program->setUniformValue(con_v_size,QVector3D(w,h,d));

    con_color_table=m_program->uniformLocation("color_table");
    con_label_color=m_program->uniformLocation("label_color_table");

    con_background_color=m_program->uniformLocation("background_color");

    for(int i=0;i<256;i++){
        color_table[i]=QVector4D(float(i)/255.0,float(i)/255.0,float(i)/255.0,float(i)/255.0);
    }
    for(int i=256;i<512;i++){
        color_table[i]=QVector4D(0,0,0,0.1);
    }

    glGenTextures(1,&color_tex);
    glBindTexture(GL_TEXTURE_1D,color_tex);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,512,0,GL_RGBA,GL_FLOAT,color_table);

    glEnable(GL_TEXTURE_1D);
    glEnable(GL_TEXTURE_3D);



    //QVector3D t(w,h,d);
    //const_box_min=-t.normalized();
    //const_box_max=t.normalized();

    m_program->release();

}
void GlWidgetForData::ReadLabelColor(){
//    int len=c_window->labels.size();
//    if(len!=0){

//        for(int i=0;i<len;i++){
//            color_table[c_window->labels[i]->value+256]=
//                    QVector4D(float(c_window->labels[i]->color.red())/255.0,
//                              float(c_window->labels[i]->color.green())/255.0,
//                              float(c_window->labels[i]->color.blue())/255.0,
//                              float(c_window->labels[i]->alpha));
//        }
//    }

    if(c_window->focusSpine!=-1){
        for(int i=256;i<512;i++){
            color_table[i]=QVector4D(0,0,0,0);
        }
        for(int i=0;i<dataList[c_window->curDataIndex]->spineIndex.length();i++){
            int curS=dataList[c_window->curDataIndex]->spineIndex[i];
            int focusLabel=dataList[c_window->curDataIndex]->labelIndex[i];
            if(curS==c_window->focusSpine){

                if(c_window->coloringType==0){
                    color_table[focusLabel+256]=QVector4D(c_window->spineColors[curS].redF(),
                                                          c_window->spineColors[curS].greenF(),
                                                          c_window->spineColors[curS].blueF(),1);
                }
                else if(c_window->coloringType==1){
                    color_table[focusLabel+256]=QVector4D(c_window->clusterColors[c_window->clusterLabel[curS]].redF(),
                                                        c_window->clusterColors[c_window->clusterLabel[curS]].greenF(),
                                                        c_window->clusterColors[c_window->clusterLabel[curS]].blueF(),1);
                }
                else if(c_window->coloringType==2){
                    if(c_window->indexOfSelectedSpines[curS]==-1){
                        color_table[focusLabel+256]=QVector4D(c_window->spineColors[curS].redF(),
                                                              c_window->spineColors[curS].greenF(),
                                                              c_window->spineColors[curS].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0*0.2);
                    }
                    else  color_table[focusLabel+256]=QVector4D(c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].redF(),
                                                                c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].greenF(),
                                                                c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].blueF(),1);

                }
                else if(c_window->coloringType==3 && c_window->typeEnable){
                    color_table[focusLabel+256]=QVector4D(c_window->typeColors[c_window->typesInt[curS]].redF(),
                                                        c_window->typeColors[c_window->typesInt[curS]].greenF(),
                                                        c_window->typeColors[c_window->typesInt[curS]].blueF(),1);
                }
                else if(c_window->coloringType==4){
                    QColor t=c_window->groups[c_window->spineGroups[curS]]->color;

                    color_table[focusLabel+256]=QVector4D(t.redF(),
                                                        t.greenF(),
                                                        t.blueF(),1);

                }


//                color_table[focusLabel+256]=QVector4D(float(c_window->spineColors[curS].red())/255.0,
//                                                      float(c_window->spineColors[curS].green())/255.0,
//                                                      float(c_window->spineColors[curS].blue())/255.0,
//                                                      1);
            }
            else{

                if(c_window->coloringType==0){
                    color_table[focusLabel+256]=QVector4D(c_window->spineColors[curS].redF(),
                                                          c_window->spineColors[curS].greenF(),
                                                          c_window->spineColors[curS].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0*0.2);
                }
                else if(c_window->coloringType==1){
                    color_table[focusLabel+256]=QVector4D(c_window->clusterColors[c_window->clusterLabel[curS]].redF(),
                                                        c_window->clusterColors[c_window->clusterLabel[curS]].greenF(),
                                                        c_window->clusterColors[c_window->clusterLabel[curS]].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0*0.2);
                }
                else if(c_window->coloringType==2){
                    if(c_window->indexOfSelectedSpines[curS]==-1){
                        color_table[focusLabel+256]=QVector4D(c_window->spineColors[curS].redF(),
                                                              c_window->spineColors[curS].greenF(),
                                                              c_window->spineColors[curS].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0*0.2);
                    }
                    else  color_table[focusLabel+256]=QVector4D(c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].redF(),
                                                                c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].greenF(),
                                                                c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0*0.2);

                }
                else if(c_window->coloringType==3 && c_window->typeEnable){
                    color_table[focusLabel+256]=QVector4D(c_window->typeColors[c_window->typesInt[curS]].redF(),
                                                        c_window->typeColors[c_window->typesInt[curS]].greenF(),
                                                        c_window->typeColors[c_window->typesInt[curS]].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0*0.2);
                }
                else if(c_window->coloringType==4){
                    QColor t=c_window->groups[c_window->spineGroups[curS]]->color;

                    color_table[focusLabel+256]=QVector4D(t.redF(),
                                                        t.greenF(),
                                                        t.blueF(),float(c_window->imageLabelOpacityControl->value())/255.0*0.2);

                }

            }
        }
    }
    else{
        for(int i=256;i<512;i++){
            color_table[i]=QVector4D(0,0,0,0);
        }
        for(int i=0;i<dataList[c_window->curDataIndex]->spineIndex.length();i++){
            int curS=dataList[c_window->curDataIndex]->spineIndex[i];
            int focusLabel=dataList[c_window->curDataIndex]->labelIndex[i];

            if(c_window->coloringType==0){
                color_table[focusLabel+256]=QVector4D(c_window->spineColors[curS].redF(),
                                                      c_window->spineColors[curS].greenF(),
                                                      c_window->spineColors[curS].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0);
            }
            else if(c_window->coloringType==1){
                color_table[focusLabel+256]=QVector4D(c_window->clusterColors[c_window->clusterLabel[curS]].redF(),
                                                    c_window->clusterColors[c_window->clusterLabel[curS]].greenF(),
                                                    c_window->clusterColors[c_window->clusterLabel[curS]].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0);
            }
            else if(c_window->coloringType==2){
                if(c_window->indexOfSelectedSpines[curS]==-1){
                    color_table[focusLabel+256]=QVector4D(c_window->spineColors[curS].redF(),
                                                          c_window->spineColors[curS].greenF(),
                                                          c_window->spineColors[curS].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0*0.2);
                }
                else  color_table[focusLabel+256]=QVector4D(c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].redF(),
                                                            c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].greenF(),
                                                            c_window->clusterColors[c_window->indexOfSelectedSpines[curS]].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0);

            }
            else if(c_window->coloringType==3 && c_window->typeEnable){
                color_table[focusLabel+256]=QVector4D(c_window->typeColors[c_window->typesInt[curS]].redF(),
                                                    c_window->typeColors[c_window->typesInt[curS]].greenF(),
                                                    c_window->typeColors[c_window->typesInt[curS]].blueF(),float(c_window->imageLabelOpacityControl->value())/255.0);
            }
            else if(c_window->coloringType==4){
                QColor t=c_window->groups[c_window->spineGroups[curS]]->color;

                color_table[focusLabel+256]=QVector4D(t.redF(),
                                                    t.greenF(),
                                                    t.blueF(),float(c_window->imageLabelOpacityControl->value())/255.0);

            }


//            color_table[focusLabel+256]=QVector4D(float(c_window->spineColors[curS].red())/255.0,
//                                                  float(c_window->spineColors[curS].green())/255.0,
//                                                  float(c_window->spineColors[curS].blue())/255.0,
//                                                  0.3);
        }
    }
    glBindTexture(GL_TEXTURE_1D,color_tex);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,512,GL_RGBA,GL_FLOAT,color_table);

}
void GlWidgetForData::ReadColorTable(){
    float c=float(c_window->imageContrastControl->value())/255.0; //0~1
    c=pow(c,3)*10; //0~10
    float minv=1.0/(1+exp(-c*(-6)));
    float maxv=1.0/(1+exp(-c*6));

    float B=float(c_window->imageBrightControl->value())/255.0*2;
    float C=float(c_window->imageContrastControl->value())/255.0*10;
    float M=float(c_window->imageContrastPosControl->value())/255.0-0.5;
    float minv2=1/(1+exp(0.5*C));
    float maxv2=1/(1+exp(-0.5*C));



    for(int i=0;i<256;i++){

//        float t=float(i)/255.0;
//        float a=float(c_window->imageBrightControl->value())/255.0; //0~1
//        float b=float(c_window->imageContrastControl->value())/128.0 - 1;//-1~1
//        b=b*3; //-3~3
//        float r=0;
//        if(t<a){
//            r=t/a;
//            r=pow(r,pow(2,b));
//            r=r*a;
//        }
//        else{
//            t=(t-a)/(1-a);
//            r=pow(r,pow(2,-b));
//            r=r*(1-a)+a;
//        }
//        if(r<0)r=0;
//        if(r>1)r=1;
//        color_table[i]=QVector4D(t,t,t,r);

        float t=float(i)/255.0; //0~1
        float a=float(c_window->imageContrastPosControl->value())/255.0;
        float v=i + (c_window->imageBrightControl->value()-128)*2;
        v/=255;


        t=t*12-6;// -6 ~ 6
        t=t - (c_window->imageContrastPosControl->value()-128.0)/128.0 *6;
        t=1.0/(1+exp(-c*t)); //0~1
        t=(t-minv)/(maxv-minv);

        t=t*float(c_window->imageBrightControl->value())/255.0*2;

        if(t<0)t=0;
        if(t>1)t=1;
//        t=1/(1+exp(-C*((t-0.5-M)*12)));
//        t=(t-minv2)/(maxv2-minv2);
        t=t*B;


        color_table[i]=QVector4D(t,t,t,t);
    }


//    float r,g,b,a;
//    FILE *f;
//    f=fopen("color_table.txt","r");
//    for(int i=0;i<256;i++){
//        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
//        color_table[i]=QVector4D(r,g,b,a);
//    }
//    fclose(f);

//    glBindTexture(GL_TEXTURE_1D,color_tex);
//    glTexSubImage1D(GL_TEXTURE_1D,0,0,512,GL_RGBA,GL_FLOAT,color_table);
}
void GlWidgetForData::interpolation_type_change(int a){

}
void GlWidgetForData::paintGL()
{
//    qDebug()<<"start glwidget for data";

    fps++;

    if(c_window->curDataIndex==-1){
        glBegin(GL_LINES);
            glColor3f(0.6,0.6,0.6);
            glVertex3f(-0.99,-0.99,1);
            glVertex3f(-0.99,0.99,1);
            glVertex3f(-0.99,0.99,1);
            glVertex3f(0.99,0.99,1);
            glVertex3f(0.99,0.99,1);
            glVertex3f(0.99,-0.99,1);
            glVertex3f(0.99,-0.99,1);
            glVertex3f(-0.99,-0.99,1);

        glEnd();

        return;
    }


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if(dataList[c_window->curDataIndex]->basePath==""){
        if(c_window->curDataIndex!=prevDataIndex){
            prevDataIndex=c_window->curDataIndex;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_3D,0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D,0);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_1D,0);

        }
        ReadColorTable();
        int tw=dataList[c_window->curDataIndex]->w;
        int th=dataList[c_window->curDataIndex]->h;
        int td=dataList[c_window->curDataIndex]->d;
        int curStack=(td-1)*sliceSliderPos;

        glBegin(GL_QUADS);
        for(int iy=0;iy<th;iy++){
            for(int ix=0;ix<tw;ix++){
                float curv=float(dataList[c_window->curDataIndex]->data16[curStack*tw*th+iy*tw+ix])/65535;
                glColor3f(color_table[int(curv*255)].w(),color_table[int(curv*255)].w(),color_table[int(curv*255)].w());


                if(tw<th){
                    float startx=float(ix-tw*0.5)/500*scale*float(this->width())/this->height();
                    float stopx=float(ix+1-tw*0.5)/500*scale*float(this->width())/this->height();
                    float starty=float(iy-th*0.5)/500*scale;
                    float stopy=float(iy+1-th*0.5)/500*scale;
                    glVertex3f(starty,startx,1);
                    glVertex3f(stopy,startx,1);
                    glVertex3f(stopy,stopx,1);
                    glVertex3f(starty,stopx,1);

                }
                else{
                    float startx=float(ix-tw*0.5)/500*scale;
                    float stopx=float(ix+1-tw*0.5)/500*scale;
                    float starty=float(iy-th*0.5)/500*scale*float(this->width())/this->height();
                    float stopy=float(iy+1-th*0.5)/500*scale*float(this->width())/this->height();
                    glVertex3f(startx,starty,1);
                    glVertex3f(startx,stopy,1);
                    glVertex3f(stopx,stopy,1);
                    glVertex3f(stopx,starty,1);
                }
            }
        }
        glEnd();


        QPainter painter(this);


        if(croping){
            float startx;
            float stopx;
            float starty;
            float stopy;

            if(tw<th){
                startx=float(cropStart.x()-tw*0.5)/500*scale*float(this->width())/this->height();
                stopx=float(cropEnd.x()+1-tw*0.5)/500*scale*float(this->width())/this->height();
                starty=float(cropStart.y()-th*0.5)/500*scale;
                stopy=float(cropEnd.y()+1-th*0.5)/500*scale;

            }
            else{
                startx=float(cropStart.x()-tw*0.5)/500*scale;
                stopx=float(cropEnd.x()+1-tw*0.5)/500*scale;
                starty=float(cropStart.y()-th*0.5)/500*scale*float(this->width())/this->height();
                stopy=float(cropEnd.y()+1-th*0.5)/500*scale*float(this->width())/this->height();

            }

            glBegin(GL_LINES);
            glLineWidth(3);
            glColor3f(0,1,1);
            glVertex3f(startx,starty,1);
            glVertex3f(startx,stopy,1);
            glVertex3f(startx,stopy,1);
            glVertex3f(stopx,stopy,1);
            glVertex3f(stopx,stopy,1);
            glVertex3f(stopx,starty,1);
            glVertex3f(stopx,starty,1);
            glVertex3f(startx,starty,1);
            glEnd();
        }
        else if(cropWait){
            float startx;
            float stopx;
            float starty;
            float stopy;

            if(tw<th){
                startx=float(cropStart.x()-tw*0.5)/500*scale*float(this->width())/this->height();
                stopx=float(cropEnd.x()+1-tw*0.5)/500*scale*float(this->width())/this->height();
                starty=float(cropStart.y()-th*0.5)/500*scale;
                stopy=float(cropEnd.y()+1-th*0.5)/500*scale;

            }
            else{
                startx=float(cropStart.x()-tw*0.5)/500*scale;
                stopx=float(cropEnd.x()+1-tw*0.5)/500*scale;
                starty=float(cropStart.y()-th*0.5)/500*scale*float(this->width())/this->height();
                stopy=float(cropEnd.y()+1-th*0.5)/500*scale*float(this->width())/this->height();

            }

            glBegin(GL_LINES);
            glLineWidth(3);
            glColor3f(0,1,0);
            glVertex3f(startx,starty,1);
            glVertex3f(startx,stopy,1);
            glVertex3f(startx,stopy,1);
            glVertex3f(stopx,stopy,1);
            glVertex3f(stopx,stopy,1);
            glVertex3f(stopx,starty,1);
            glVertex3f(stopx,starty,1);
            glVertex3f(startx,starty,1);
            glEnd();

            float txx=startx>stopx?startx:stopx;
            float tyy=starty<stopy?starty:stopy;
            txx=(txx+1)*this->width()*0.5;
            tyy=this->height()-(tyy+1)*this->height()*0.5;

            QPen t;
            t.setColor(QColor(0,255,0));
            painter.setPen(t);
            QBrush tb(Qt::SolidPattern);

            tb.setColor(QColor(0,255,0));
            painter.setBrush(tb);
            painter.drawRect(txx,tyy,60,20);

            QFont tfont;
            tfont.setBold(true);
            painter.setFont(tfont);
            t.setColor(QColor(0,100,0));
            t.setWidth(5);
            painter.setPen(t);
            painter.drawText(txx,tyy,60,20,Qt::AlignCenter, "Crop");



        }


        QPen t;
        t.setColor(QColor(220,220,220));
        t.setWidth(5);
        painter.setPen(t);
        painter.drawLine(this->width()*0.1,this->height()*0.9,this->width()*0.9,this->height()*0.9);

        t.setColor(QColor(51,102,200));
        t.setWidth(17);
        //t.setCapStyle(Qt::RoundCap);
        painter.setPen(t);
        painter.drawPoint(this->width()*0.1 + this->width()*0.8*sliceSliderPos,this->height()*0.9);


        t.setColor(QColor(51,102,255));
        t.setWidth(15);
        //t.setCapStyle(Qt::RoundCap);
        painter.setPen(t);
        painter.drawPoint(this->width()*0.1 + this->width()*0.8*sliceSliderPos,this->height()*0.9);

        QFont tfont;
        tfont.setBold(true);
        painter.setFont(tfont);
        t.setColor(QColor(51,102,255));
        painter.setPen(t);
        painter.drawText(this->width()*0.1-15,this->height()*0.9+5, "0");
        painter.drawText(this->width()*0.9+10,this->height()*0.9+5, QString::number(td));
        painter.drawText(this->width()*0.1 + this->width()*0.8*sliceSliderPos-5,this->height()*0.9-10, QString::number(curStack));

        painter.setPen(QColor(255-background_color.x()*255,255-background_color.y()*255,255-background_color.z()*255));
        painter.drawText(30, 30, "FPS: " + p_fps);
        painter.end();
    }
    else{

        QVector3D box_t(dataList[c_window->curDataIndex]->w,dataList[c_window->curDataIndex]->h,dataList[c_window->curDataIndex]->d);

        const_box_min=-box_t.normalized();
        const_box_max=box_t.normalized();
        QVector3D right=cross_product(up,center1-eye_position).normalized();

        box_min=(const_box_min+shift)*scale;
        box_max=(const_box_max+shift)*scale;

        sample=sqrt(float(dataList[c_window->curDataIndex]->w*dataList[c_window->curDataIndex]->w
                +dataList[c_window->curDataIndex]->h*dataList[c_window->curDataIndex]->h
                +dataList[c_window->curDataIndex]->d*dataList[c_window->curDataIndex]->d));


        m_program->bind();

        if(c_window->curDataIndex!=prevDataIndex){
            c_window->focusSpine=-1;
            prevDataIndex=c_window->curDataIndex;
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(con_tex,0);
            glBindTexture(GL_TEXTURE_3D,dataList[c_window->curDataIndex]->tex_3D);

            glActiveTexture(GL_TEXTURE1);
            glUniform1i(con_tex_label,1);
            glBindTexture(GL_TEXTURE_3D,dataList[c_window->curDataIndex]->tex_label);

            glActiveTexture(GL_TEXTURE2);
            glUniform1i(con_color_table,2);
            glBindTexture(GL_TEXTURE_1D,color_tex);

        }


        ReadColorTable();
        ReadLabelColor();
        m_program->setUniformValue(con_window_size,QVector2D(this->width()/800.0,this->height()/800.0));
        m_program->setUniformValue(con_eye_pos,eye_position);
        m_program->setUniformValue(con_up,up);
        m_program->setUniformValue(con_box_min,box_min);
        m_program->setUniformValue(con_box_max,box_max);

        m_program->setUniformValue(con_sample,sample/scale);

        m_program->setUniformValue(con_diffu,diffu);
        m_program->setUniformValue(con_ambi,ambi);
        m_program->setUniformValue(con_specu,specu);
        m_program->setUniformValue(con_shin,shin);

        m_program->setUniformValue(con_l_t,l_t);
        m_program->setUniformValue(con_background_color,background_color);

        m_program->setUniformValue(con_phong_enable,phong_enable);

        m_program->setUniformValue(con_sampling_rate,sampling_rate);
        m_program->setUniformValue(con_roi_center,roi_center);
        m_program->setUniformValue(con_roi_size,roi_size);


        glBegin(GL_QUADS);
            glVertex3f(-1,-1,1);
            glVertex3f(-1,1,1);
            glVertex3f(1,1,1);
            glVertex3f(1,-1,1);
        glEnd();

        m_program->release();

        QPainter painter(this);
        painter.setPen(QColor(255-background_color.x()*255,255-background_color.y()*255,255-background_color.z()*255));
        painter.drawText(30, 30, "FPS: " + p_fps);
        painter.end();
    }




    //qDebug()<<"end glwidget for data";


}

void GlWidgetForData::resizeGL(int w, int h)
{
//    int t=w>h?w:h;
//    this->resize(t,t);
}

void GlWidgetForData::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    mousePressedPos=event->pos();

    if(dataList[c_window->curDataIndex]->basePath==""){
        sliderPressed=false;
        int tp=this->width()*0.1 + this->width()*0.8*sliceSliderPos;
        if(abs(event->pos().x()-tp)<8 && abs(event->pos().y()-this->height()*0.9)<8){
            sliderPressed=true;
        }
        else if(event->buttons() & Qt::LeftButton){
            if(cropWait){
                int tw=dataList[c_window->curDataIndex]->w;
                int th=dataList[c_window->curDataIndex]->h;
                int td=dataList[c_window->curDataIndex]->d;

                float startx;
                float stopx;
                float starty;
                float stopy;

                if(tw<th){
                    startx=float(cropStart.x()-tw*0.5)/500*scale*float(this->width())/this->height();
                    stopx=float(cropEnd.x()+1-tw*0.5)/500*scale*float(this->width())/this->height();
                    starty=float(cropStart.y()-th*0.5)/500*scale;
                    stopy=float(cropEnd.y()+1-th*0.5)/500*scale;

                }
                else{
                    startx=float(cropStart.x()-tw*0.5)/500*scale;
                    stopx=float(cropEnd.x()+1-tw*0.5)/500*scale;
                    starty=float(cropStart.y()-th*0.5)/500*scale*float(this->width())/this->height();
                    stopy=float(cropEnd.y()+1-th*0.5)/500*scale*float(this->width())/this->height();

                }

                float txx=startx>stopx?startx:stopx;
                float tyy=starty<stopy?starty:stopy;
                txx=(txx+1)*this->width()*0.5;
                tyy=this->height()-(tyy+1)*this->height()*0.5;

                if(event->pos().x()-txx>=0 && event->pos().x()-txx<=60
                        && event->pos().y()-tyy>=0 && event->pos().y()-tyy<=20){
                    croping=false;
                    cropWait=false;
                    cropCurrentData(event->pos());
                    update();
                    return;
                }
            }
            croping=true;
            cropWait=false;
            int tw=dataList[c_window->curDataIndex]->w;
            int th=dataList[c_window->curDataIndex]->h;

            if(tw<th){
                cropStart=QPointF( (float(event->pos().x())/this->width()*2-1) * 500/scale/float(this->width())*this->height() + tw*0.5,
                                   (1-float(event->pos().y())/this->height()*2) * 500/scale + th*0.5);
                cropEnd=cropStart;
            }
            else{
                cropStart=QPointF( (float(event->pos().x())/this->width()*2-1) * 500/scale + tw*0.5,
                                   (1-float(event->pos().y())/this->height()*2) * 500/scale/float(this->width())*this->height() + th*0.5);
                cropEnd=cropStart;

            }
        }
    }

}

void GlWidgetForData::mouseReleaseEvent(QMouseEvent *event)
{
    sliderPressed=false;
    QPoint t=mousePressedPos-event->pos();
    if(abs(t.x())+abs(t.y())<1){
        if(croping){
            cropWait=false;
            croping=false;
        }
        else if(c_window->clicked==false){
            int focusedObject=getObjectCoordfromMousePoint(QVector2D(event->x(),event->y()));
            if(focusedObject!=-1){
                c_window->clicked=true;
                c_window->focusSpine=focusedObject;
                c_window->synchronization();
            }
        }
        else if(c_window->clicked==true){
            c_window->clicked=false;
            c_window->focusSpine=-1;
            c_window->synchronization();
        }
    }
    else{
        if(croping){
            cropWait=true;
            croping=false;
        }
    }

}
void GlWidgetForData::mouseMoveEvent(QMouseEvent *event)
{
    setFocus();

    if(sliderPressed && dataList[c_window->curDataIndex]->basePath==""){
        sliceSliderPos=(event->x()-this->width()*0.1)/(this->width()*0.8);
        if(sliceSliderPos<0)sliceSliderPos=0;
        if(sliceSliderPos>1)sliceSliderPos=1;
        update();
        m_lastPos=event->pos();
        return;
    }

    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if(!c_window->clicked){
        c_window->focusSpine=getObjectCoordfromMousePoint(QVector2D(event->x(),event->y()));
        c_window->synchronization();
    }

    if (event->buttons() & Qt::LeftButton ) {
        if(dataList[c_window->curDataIndex]->basePath!=""){
            QVector3D ttt=cross_product(up,center1-eye_position).normalized();
            eye_position=(eye_position+ttt*dx/70/scale*EYE).normalized()*EYE;

            ttt=cross_product(up,center1-eye_position).normalized();
            eye_position=(eye_position+up*dy/70/scale*EYE).normalized()*EYE;

            up=cross_product(center1-eye_position,ttt).normalized();
        }
        else{
            if(croping){
                int tw=dataList[c_window->curDataIndex]->w;
                int th=dataList[c_window->curDataIndex]->h;

                if(tw<th){
                    cropEnd=QPointF( (float(event->pos().x())/this->width()*2-1) * 500/scale/float(this->width())*this->height() + tw*0.5,
                                       (1-float(event->pos().y())/this->height()*2) * 500/scale + th*0.5);
                }
                else{
                    cropEnd=QPointF( (float(event->pos().x())/this->width()*2-1) * 500/scale + tw*0.5,
                                       (1-float(event->pos().y())/this->height()*2) * 500/scale/float(this->width())*this->height() + th*0.5);
                }
            }

        }
   }
    else if (event->buttons() & Qt::RightButton) {
        if(scale>0.02){
            float t=1.0*(-dy)/1000*scale*EYE;
            scale+=t;
            shift+=shift.normalized()*t/scale/100.0;
        }
        else{
            scale=0.021;
        }
   }
    else if(event->buttons() & Qt::MiddleButton){
        QVector3D ttt=cross_product(up,center1-eye_position).normalized();
        shift+=ttt*(float(-dx)/400/scale)+up*(float(-dy)/400/scale);
    }
    update();
    m_lastPos=event->pos();

}

void GlWidgetForData::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
//        c_window->roi_size+=numSteps.x()+numSteps.y();
//        if(c_window->roi_size<1)c_window->roi_size=1;
    }

    event->accept();
}

void GlWidgetForData::timerEvent(QTimerEvent *event){
    if(tog==0){
        QVector3D ttt=cross_product(up,center1-eye_position).normalized();
        eye_position=(eye_position+ttt*0.005*EYE).normalized()*EYE;
    }


    if(fps_start==-1){
        fps_start=GetTickCount();
    }
    else{
        if(GetTickCount()-fps_start>1000){
            char itoa_t[10];
            p_fps=QString(itoa(fps,itoa_t,10));
            fps=0;
            fps_start=GetTickCount();
        }
    }

    update();
}
void GlWidgetForData::keyPressEvent(QKeyEvent *event)
{

    if(event->key()==Qt::Key_R){
        center1=QVector3D(0,0,0);
        eye_position=QVector3D(0,0,EYE);
        up=QVector3D(0,1,0);
        shift=QVector2D(0,0);
        scale=1;
    }


    if(event->key()==Qt::Key_Down){
        shift+=up*(1.0/50/scale);

    }
    if(event->key()==Qt::Key_Up){
        shift-=up*(1.0/50/scale);
    }
    if(event->key()==Qt::Key_Right){
        QVector3D ttt=cross_product(up,center1-eye_position).normalized();
        shift+=ttt*(1.0/50/scale);

    }
    if(event->key()==Qt::Key_Left){
        QVector3D ttt=cross_product(up,center1-eye_position).normalized();
        shift-=ttt*(1.0/50/scale);

    }


}
void GlWidgetForData::keyReleaseEvent(QKeyEvent *event)
{
}


void GlWidgetForData::cropCurrentData(QPoint pos){
    int tw=dataList[c_window->curDataIndex]->w;
    int th=dataList[c_window->curDataIndex]->h;
    int td=dataList[c_window->curDataIndex]->d;

    int newStartx=cropStart.x()<cropEnd.x()?cropStart.x():cropEnd.x();
    int newEndx=cropStart.x()<cropEnd.x()?cropEnd.x():cropStart.x();
    int newStarty=cropStart.y()<cropEnd.y()?cropStart.y():cropEnd.y();
    int newEndy=cropStart.y()<cropEnd.y()?cropEnd.y():cropStart.y();


    if(newStartx<0)newStartx=0;
    if(newEndx<0)newEndx=0;
    if(newStarty<0)newStarty=0;
    if(newEndy<0)newEndy=0;
    if(newStartx>tw)newStartx=tw;
    if(newEndx>tw)newEndx=tw;
    if(newStarty>th)newStarty=th;
    if(newEndy>th)newEndy=th;
    int newW=(newEndx-newStartx);
    qDebug()<<newStartx<<newEndx<<newStarty<<newEndy;
    while(newW%4!=0){
        newEndx--;
        newW=(newEndx-newStartx);
    }
    qDebug()<<newStartx<<newEndx<<newStarty<<newEndy;
    int newH=(newEndy-newStarty);
    unsigned short *newData=new unsigned short[newW*newH*td];
    for(int iz=0;iz<td;iz++){
        for(int iy=newStarty;iy<newEndy;iy++){
            for(int ix=newStartx;ix<newEndx;ix++){
                newData[iz*newW*newH+(iy-newStarty)*newW+ix-newStartx]=
                        dataList[c_window->curDataIndex]->data16[iz*tw*th+iy*tw+ix];
            }
        }
    }
    qDebug()<<"finish1";
    delete []dataList[c_window->curDataIndex]->data16;
    dataList[c_window->curDataIndex]->data16=newData;
    dataList[c_window->curDataIndex]->w=newW;
    dataList[c_window->curDataIndex]->h=newH;
    qDebug()<<"finish2";


}
void GlWidgetForData::cut_plane(int r){
}


void GlWidgetForData::set_z_scale(int n){
    z_scale=n/100.0;
}



void GlWidgetForData::ambi_change(int a){
    ambi=float(a)/50;
}

void GlWidgetForData::diff_change(int a){
    diffu=float(a)/100;
}

void GlWidgetForData::spec_change(int a){
    specu=float(a)/100;
}
void GlWidgetForData::shin_change(int a){
    shin=a;
}
void GlWidgetForData::l_t_change(int a){
    l_t=float(a)/100;
}

void GlWidgetForData::change_R1(int a){
}

void GlWidgetForData::change_R2(int a){
}

void GlWidgetForData::change_cut_d(int a){

}
void GlWidgetForData::change_cut_enable(bool a){
}
void GlWidgetForData::change_cut_alpha(int a){
}
void GlWidgetForData::change_phong_enable(bool a){
    phong_enable=a;
}
void GlWidgetForData::change_sampling_rate(int a){
    sampling_rate=float(a)/100;

}
int GlWidgetForData::getObjectCoordfromMousePoint(QVector2D mousePoint){



//    return QVector3D(0,0,0);



    if(c_window->curDataIndex==-1)return -1;
    if(dataList[c_window->curDataIndex]->basePath=="")return -1;

    int w=dataList[c_window->curDataIndex]->w;
    int h=dataList[c_window->curDataIndex]->h;
    int d=dataList[c_window->curDataIndex]->d;

    QVector2D global_mousePoint=QVector2D(mousePoint.x()/this->width()*2-1,-mousePoint.y()/this->height()*2+1);

    QVector3D right=cross_product(-eye_position,up).normalized();



    QVector3D view_eye_position=eye_position+right;
    QVector3D view_right=cross_product(-view_eye_position,up).normalized();
    view_eye_position=view_eye_position+up;
    QVector3D view_up=cross_product(view_right,-view_eye_position).normalized();

    QVector3D t_pixel_position=view_eye_position*0.5+
                               view_right*global_mousePoint.x()*this->width()/800.0+
                               view_up*global_mousePoint.y()*this->height()/800.0;

    QVector3D direction=-view_eye_position.normalized();
    float alpha=0;

    int cn=0;
    QVector3D a[2];
    float k;
    QVector3D point;
    if(cn<2 && direction.x()!=0){
        k=(box_min.x()-t_pixel_position.x())/direction.x();
        point=t_pixel_position+k*direction;
        if(point.y()>=box_min.y() && point.y()<=box_max.y()
            && point.z()>=box_min.z() && point.z()<=box_max.z()){
                a[cn++]=point;
        }
    }
    if(cn<2&& direction.x()!=0){
        k=(box_max.x()-t_pixel_position.x())/direction.x();
        point=t_pixel_position+k*direction;
        if(point.y()>=box_min.y() && point.y()<=box_max.y()
            && point.z()>=box_min.z() && point.z()<=box_max.z()){
                a[cn++]=point;
        }
    }
    if(cn<2 && direction.y()!=0){
        k=(box_min.y()-t_pixel_position.y())/direction.y();
        point=t_pixel_position+k*direction;
        if(point.x()>=box_min.x() && point.x()<=box_max.x()
             && point.z()>=box_min.z() && point.z()<=box_max.z()){
                 a[cn++]=point;
        }
    }
    if(cn<2 && direction.y()!=0){
        k=(box_max.y()-t_pixel_position.y())/direction.y();
        point=t_pixel_position+k*direction;
        if(point.x()>=box_min.x() && point.x()<=box_max.x()
             && point.z()>=box_min.z() && point.z()<=box_max.z()){
                 a[cn++]=point;
        }
    }
    if(cn<2 && direction.z()!=0){
        k=(box_min.z()-t_pixel_position.z())/direction.z();
        point=t_pixel_position+k*direction;
        if(point.x()>=box_min.x() && point.x()<=box_max.x()
            && point.y()>=box_min.y() && point.y()<=box_max.y()){
                 a[cn++]=point;
        }
    }
    if(cn<2 && direction.z()!=0){
        k=(box_max.z()-t_pixel_position.z())/direction.z();
        point=t_pixel_position+k*direction;
        if(point.x()>=box_min.x() && point.x()<=box_max.x()
             && point.y()>=box_min.y() && point.y()<=box_max.y()){
                 a[cn++]=point;
        }
    }
    if(cn!=2){
        return -1;
    }
    if((a[0]-view_eye_position).length()>(a[1]-view_eye_position).length()){
        QVector3D t=a[0];
        a[0]=a[1];
        a[1]=t;
    }

    int sampling_num=int((a[1]-a[0]).length()*sample/scale*sampling_rate);
    if(sampling_num==0){
        return -1;
    }
    QVector3D normalize_box=QVector3D(1.0/(box_max.x()-box_min.x()),1.0/(box_max.y()-box_min.y()),1.0/(box_max.z()-box_min.z()));
    QVector3D dir=(a[1]-a[0])/sampling_num;
    QVector3D cur_location=a[0];

    for(int i=0;i<sampling_num;i++){
        cur_location=cur_location+dir;
        QVector3D object_location=(cur_location-box_min)*normalize_box*QVector3D(w-1,h-1,d-1);
        int data_location=int(object_location.z())*w*h+int(object_location.y())*w+int(object_location.x());
        int value=dataList[c_window->curDataIndex]->label_data[data_location];

        if(value!=0){
            int len=dataList[c_window->curDataIndex]->labelIndex.size();
            for(int j=0;j<len;j++){
                if(dataList[c_window->curDataIndex]->labelIndex[j]==value){
                    return dataList[c_window->curDataIndex]->spineIndex[j];
                }
            }
        }
    }
    return -1;
}
void GlWidgetForData::loadTifFile(QString basePath,int groupIndex){

    dataset *newData=new dataset;


    //load data
    TinyTIFFReaderFile* tiffr=NULL;
    if(groupIndex==-1){
        tiffr=TinyTIFFReader_open(basePath.toStdString().c_str());
    }
    else tiffr=TinyTIFFReader_open((basePath+"data.tif").toStdString().c_str());
    if (!tiffr) {
        //qDebug()<<"    ERROR reading (not existent, not accessible or no TIFF file)";
    } else {
        //if (TinyTIFFReader_wasError(tiffr)) qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr);
        //qDebug()<<"    ImageDescription:"<< TinyTIFFReader_getImageDescription(tiffr).c_str();

        uint32_t width=TinyTIFFReader_getWidth(tiffr);
        uint32_t height=TinyTIFFReader_getHeight(tiffr);

        newData->type=TinyTIFFReader_getBitsPerSample(tiffr);
        //qDebug()<<"    type: "<<newData->type;

        uint32_t frames=TinyTIFFReader_countFrames(tiffr);
        //qDebug()<<"    frames: "<<frames;

        newData->w=width,newData->h=height,newData->d=frames;


        if(newData->type==8){
            newData->data8=new unsigned char[width*height*frames]();
        }
        else if(newData->type==16){
            newData->data16=new unsigned short[width*height*frames]();
        }


        //if (TinyTIFFReader_wasError(tiffr)) qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr);
        uint32_t frame=0;
        do {
            bool ok=true;
            //if (width>0 && height>0) qDebug()<<"    size of frame "<<frame<<": "<<width<<"x"<<height;
            //else { qDebug()<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height; ok=false; }
            if (ok) {
                if(newData->type==8)TinyTIFFReader_getSampleData(tiffr, &newData->data8[frame*width*height], 0);
                if(newData->type==16)TinyTIFFReader_getSampleData(tiffr, &newData->data16[frame*width*height], 0);
                //if (TinyTIFFReader_wasError(tiffr)) { ok=false; qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr); }

                frame++;

            }
        } while (TinyTIFFReader_readNext(tiffr)); // iterate over all frames
        //qDebug()<<"    read "<<frame<<" frames";
    }
    TinyTIFFReader_close(tiffr);

    if(groupIndex==-1){

        if(newData->w<newData->h){
            if(newData->type==8){
                unsigned char *t2=new unsigned char[newData->w*newData->h*newData->d]();
                for(int z=0;z<newData->d;z++){
                    for(int y=0;y<newData->h;y++){
                        for(int x=0;x<newData->w;x++){
                            unsigned char t3=newData->data8[z*newData->h*newData->w+y*newData->w+x];
                            t2[z*newData->h*newData->w+x*newData->h+y]=t3;
                        }
                    }
                }
                delete []newData->data8;
                newData->data8=t2;
                int tww=newData->w;
                newData->w=newData->h;
                newData->h=tww;

            }
            else{
                unsigned short *t2=new unsigned short[newData->w*newData->h*newData->d]();
                for(int z=0;z<newData->d;z++){
                    for(int y=0;y<newData->h;y++){
                        for(int x=0;x<newData->w;x++){
                            unsigned short t3=newData->data16[z*newData->h*newData->w+y*newData->w+x];
                            t2[z*newData->h*newData->w+x*newData->h+y]=t3;
                        }
                    }
                }
                delete []newData->data16;
                newData->data16=t2;
                int tww=newData->w;
                newData->w=newData->h;
                newData->h=tww;

            }
        }

        if(newData->w%4!=0){
            int ad=4-newData->w%4;
            newData->w+=ad;
            if(newData->type==8){
                unsigned char *t2=new unsigned char[newData->w*newData->h*newData->d]();
                for(int z=0;z<newData->d;z++){
                    for(int y=0;y<newData->h;y++){
                        for(int x=0;x<newData->w-ad;x++){
                            unsigned char t3=newData->data8[z*newData->h*(newData->w-ad)+y*(newData->w-ad)+x];
                            t2[z*newData->h*newData->w+y*newData->w+x]=t3;
                        }
                    }
                }
                delete []newData->data8;
                newData->data8=t2;
            }
            else{
                unsigned short *t2=new unsigned short[newData->w*newData->h*newData->d]();
                for(int z=0;z<newData->d;z++){
                    for(int y=0;y<newData->h;y++){
                        for(int x=0;x<newData->w-ad;x++){
                            unsigned short t3=newData->data16[z*newData->h*(newData->w-ad)+y*(newData->w-ad)+x];
                            t2[z*newData->h*newData->w+y*newData->w+x]=t3;
                        }
                    }
                }
                delete []newData->data16;
                newData->data16=t2;

            }
        }

        int maxValue=1;
        int s=newData->d*newData->h*newData->w;
        for(int ii=0;ii<s;ii++){
            if(newData->data16[ii]>maxValue){
                maxValue=newData->data16[ii];
            }
        }
        for(int ii=0;ii<s;ii++){
            newData->data16[ii]=float(newData->data16[ii])/maxValue*65535;
        }


        newData->label_data=new unsigned char[newData->w*newData->h*newData->d]();


        newData->dataIndex=c_window->globalDataIndex;
        c_window->globalDataIndex++;

        newData->basePath="";
        dataList.push_back(newData);


        glGenTextures(1,&newData->tex_3D);
        glBindTexture(GL_TEXTURE_3D,newData->tex_3D);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

        if(newData->type==8){
            //glTexImage3D(GL_TEXTURE_3D,0,GL_RED,w,h,d,0,GL_RED,GL_UNSIGNED_BYTE,c_window->data8);
            glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_BYTE,newData->data8);

        }
        else if(newData->type==16){
            glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_SHORT,newData->data16);

        }

        glGenTextures(1,&newData->tex_label);
        glBindTexture(GL_TEXTURE_3D,newData->tex_label);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
        glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_BYTE,newData->label_data);


        return;
    }

    //load label
    tiffr=NULL;
    tiffr=TinyTIFFReader_open((basePath+"label.tif").toStdString().c_str());
    if (!tiffr) {
        //qDebug()<<"    ERROR reading (not existent, not accessible or no TIFF file)";
    } else {
        //if (TinyTIFFReader_wasError(tiffr)) qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr);
        //qDebug()<<"    ImageDescription:"<< TinyTIFFReader_getImageDescription(tiffr).c_str();

        uint32_t width=TinyTIFFReader_getWidth(tiffr);
        uint32_t height=TinyTIFFReader_getHeight(tiffr);

        uint32_t frames=TinyTIFFReader_countFrames(tiffr);
        //qDebug()<<"    frames: "<<frames;

        newData->label_data=new unsigned char[width*height*frames]();



        //if (TinyTIFFReader_wasError(tiffr)) qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr);
        uint32_t frame=0;
        do {
            bool ok=true;
            //if (width>0 && height>0) qDebug()<<"    size of frame "<<frame<<": "<<width<<"x"<<height;
            //else { qDebug()<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height; ok=false; }
            if (ok) {
                TinyTIFFReader_getSampleData(tiffr, &newData->label_data[frame*width*height], 0);
               // if (TinyTIFFReader_wasError(tiffr)) { ok=false; qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr); }

                frame++;

            }
        } while (TinyTIFFReader_readNext(tiffr)); // iterate over all frames
        //qDebug()<<"    read "<<frame<<" frames";
    }
    TinyTIFFReader_close(tiffr);

    if(newData->w%4!=0){
        int ad=4-newData->w%4;
        newData->w+=ad;
        unsigned char *t=new unsigned char[newData->w*newData->h*newData->d]();
        for(int z=0;z<newData->d;z++){
            for(int y=0;y<newData->h;y++){
                for(int x=0;x<newData->w-ad;x++){
                    t[z*newData->h*newData->w+y*newData->w+x]=newData->label_data[z*newData->h*(newData->w-ad)+y*(newData->w-ad)+x];
                }
            }
        }
        delete []newData->label_data;
        newData->label_data=t;

//        for(int z=0;z<newData->d;z++){
//            for(int y=0;y<newData->h;y++){
//                for(int x=0;x<newData->w-ad;x++){
//                    int tt=newData->label_data[z*newData->h*(newData->w)+y*(newData->w)+x];
//                    if(tt!=0)qDebug()<<tt;
//                }
//            }
//        }
        if(newData->type==8){
            unsigned char *t2=new unsigned char[newData->w*newData->h*newData->d]();
            for(int z=0;z<newData->d;z++){
                for(int y=0;y<newData->h;y++){
                    for(int x=0;x<newData->w-ad;x++){
                        t2[z*newData->h*newData->w+y*newData->w+x]=newData->data8[z*newData->h*(newData->w-ad)+y*(newData->w-ad)+x];
                    }
                }
            }
            delete []newData->data8;
            newData->data8=t2;

        }
        else{
            unsigned short *t2=new unsigned short[newData->w*newData->h*newData->d]();
            for(int z=0;z<newData->d;z++){
                for(int y=0;y<newData->h;y++){
                    for(int x=0;x<newData->w-ad;x++){
                        t2[z*newData->h*newData->w+y*newData->w+x]=newData->data16[z*newData->h*(newData->w-ad)+y*(newData->w-ad)+x];
                    }
                }
            }
            delete []newData->data16;
            newData->data16=t2;

        }
    }


    //load spine info

    QFile File(basePath+"spine/spineInfo.csv");
    //qDebug()<<basePath+"spine/spineInfo.csv";

    if (File.open(QIODevice::ReadOnly))
    {
        QTextStream input(&File);
        QString headerLine = input.readLine();
        //qDebug()<<headerLine;

        QStringList headers=headerLine.split(QChar(','),QString::KeepEmptyParts);
        while (!input.atEnd())
        {
            QString line = input.readLine();
            //qDebug()<<line;

            QStringList words=line.split(QChar(','),QString::KeepEmptyParts);
            c_window->spineColors.push_back(QColor(qrand()%206+50,qrand()%206+50,qrand()%206+50));
            c_window->spineNames.push_back(words[0]);
            c_window->spineGroups.push_back(groupIndex);

            c_window->groups[groupIndex]->numSpine++;
            c_window->data.push_back(QVector<float>());
            c_window->spineCorrect.push_back(true);

            c_window->indexOfSelectedSpines.push_back(-1);
            //qDebug()<<basePath+"spine/"+words[0];

            GLWidget *cur=new GLWidget(c_window->spines.size(),basePath+"spine/"+words[0],c_window);
            connect(c_window->colorbar,SIGNAL(minValueChanged(float)),cur,SLOT(setCurvatureMin(float)));
            connect(c_window->colorbar,SIGNAL(maxValueChanged(float)),cur,SLOT(setCurvatureMax(float)));
            connect(c_window->curvatureType,SIGNAL(currentIndexChanged(int)),cur,SLOT(setCurvatureType(int)));
            connect(cur,SIGNAL(viewChange(int)),c_window,SLOT(viewChange(int)));
            connect(cur,SIGNAL(synchronization()),c_window,SLOT(synchronization()));


            c_window->spines.push_back(cur);

            //qDebug()<<"end make gl";
            c_window->spineEnable.push_back(true);
            c_window->clusterLabel.push_back(0);

            c_window->tsneResult.push_back(QVector2D(0,0));


            newData->spineIndex.push_back(c_window->spines.size()-1);
            newData->spineColors.push_back(c_window->spineColors.back());

            int index=0;
            for(int i=1;i<words.count();i++){
                //qDebug()<<words[i];
                if(c_window->featureNames[i-1]=="Type"){
                    c_window->types.push_back(QString(words[i]));
                    //qDebug()<<"type";
                    if(QString(words[i])=="None"){
                        c_window->typesInt.push_back(0);
                    }
                    else if(QString(words[i])=="Mushroom"){
                        c_window->typesInt.push_back(1);
                    }
                    else if(QString(words[i])=="Thin"){
                        c_window->typesInt.push_back(2);
                    }
                    else if(QString(words[i])=="Stubby"){
                        c_window->typesInt.push_back(3);
                    }
                    else{
                        c_window->typesInt.push_back(4);
                    }

                }
                else{
                    float value=QString(words[i]).toFloat();
                    c_window->groups[groupIndex]->totalFeature[index]+=value;
                    c_window->data[c_window->data.length()-1].push_back(value);
                    if(value<c_window->featureRanges[index].x())c_window->featureRanges[index].setX(value);
                    if(value>c_window->featureRanges[index].y())c_window->featureRanges[index].setY(value);
                    index++;
                }
            }
        }

        File.close();
    }


    //qDebug()<<"end1";



    float lmax=-1000;
    float lmin=1000;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureNames[i]=="hMin" ||
                c_window->featureNames[i]=="hMax" ||
                c_window->featureNames[i]=="nMin" ||
                c_window->featureNames[i]=="nMax" ){
            if(lmin>c_window->featureRanges[i].x())lmin=c_window->featureRanges[i].x();
            if(lmax<c_window->featureRanges[i].y())lmax=c_window->featureRanges[i].y();
        }
    }
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureNames[i]=="hMin" ||
                c_window->featureNames[i]=="hMax" ||
                c_window->featureNames[i]=="nMin" ||
                c_window->featureNames[i]=="nMax" ){
            c_window->featureRanges[i].setX(lmin);
            c_window->featureRanges[i].setY(lmax);
        }
    }
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureRanges[i].x()==c_window->featureRanges[i].y()){
            c_window->featureRanges[i].setX(c_window->featureRanges[i].x()-0.0001);
            c_window->featureRanges[i].setY(c_window->featureRanges[i].y()+0.0001);
        }
    }




    QFile File2(basePath+"spineLabel.csv");
    //qDebug()<<basePath+"spineLabel.csv";

    if (File2.open(QIODevice::ReadOnly))
    {
        QTextStream input(&File2);
        QString headerLine = input.readLine();
        QStringList headers=headerLine.split(QChar(','),QString::KeepEmptyParts);
        while (!input.atEnd())
        {
            QString line = input.readLine();
            QStringList words=line.split(QChar(','),QString::KeepEmptyParts);
            int value=QString(words[1]).toFloat();
            //qDebug()<<value;
            newData->labelIndex.push_back(value);
        }

        File2.close();
    }



    glGenTextures(1,&newData->tex_3D);
    glBindTexture(GL_TEXTURE_3D,newData->tex_3D);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    if(newData->type==8){
        //glTexImage3D(GL_TEXTURE_3D,0,GL_RED,w,h,d,0,GL_RED,GL_UNSIGNED_BYTE,c_window->data8);
        glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_BYTE,newData->data8);

    }
    else if(newData->type==16){
        glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_SHORT,newData->data16);

    }

    glGenTextures(1,&newData->tex_label);
    glBindTexture(GL_TEXTURE_3D,newData->tex_label);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_BYTE,newData->label_data);



    newData->dataIndex=c_window->globalDataIndex;
    c_window->globalDataIndex++;

    newData->basePath=basePath;

    dataList.push_back(newData);
    //qDebug()<<"end load data";
}



void GlWidgetForData::changeDataset(QString basePath,int groupIndex,int dataIndex){

    qDebug()<<"change dataset start";
    dataset *newData=dataList[dataIndex];

    delete []newData->data16;
    delete []newData->label_data;
    qDebug()<<"remove prev data";

    //load data
    TinyTIFFReaderFile* tiffr=NULL;
    tiffr=TinyTIFFReader_open((basePath+"data.tif").toStdString().c_str());
    if (!tiffr) {
        //qDebug()<<"    ERROR reading (not existent, not accessible or no TIFF file)";
    } else {
        //if (TinyTIFFReader_wasError(tiffr)) qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr);
        //qDebug()<<"    ImageDescription:"<< TinyTIFFReader_getImageDescription(tiffr).c_str();

        uint32_t width=TinyTIFFReader_getWidth(tiffr);
        uint32_t height=TinyTIFFReader_getHeight(tiffr);

        newData->type=TinyTIFFReader_getBitsPerSample(tiffr);
        //qDebug()<<"    type: "<<newData->type;

        uint32_t frames=TinyTIFFReader_countFrames(tiffr);
        //qDebug()<<"    frames: "<<frames;

        newData->w=width,newData->h=height,newData->d=frames;


        if(newData->type==8){
            newData->data8=new unsigned char[width*height*frames]();
        }
        else if(newData->type==16){
            newData->data16=new unsigned short[width*height*frames]();
        }


        //if (TinyTIFFReader_wasError(tiffr)) qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr);
        uint32_t frame=0;
        do {
            bool ok=true;
            //if (width>0 && height>0) qDebug()<<"    size of frame "<<frame<<": "<<width<<"x"<<height;
            //else { qDebug()<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height; ok=false; }
            if (ok) {
                if(newData->type==8)TinyTIFFReader_getSampleData(tiffr, &newData->data8[frame*width*height], 0);
                if(newData->type==16)TinyTIFFReader_getSampleData(tiffr, &newData->data16[frame*width*height], 0);
                //if (TinyTIFFReader_wasError(tiffr)) { ok=false; qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr); }

                frame++;

            }
        } while (TinyTIFFReader_readNext(tiffr)); // iterate over all frames
        //qDebug()<<"    read "<<frame<<" frames";
    }
    TinyTIFFReader_close(tiffr);
    qDebug()<<"finish load data";


    //load label
    tiffr=NULL;
    tiffr=TinyTIFFReader_open((basePath+"label.tif").toStdString().c_str());
    if (!tiffr) {
        //qDebug()<<"    ERROR reading (not existent, not accessible or no TIFF file)";
    } else {
        //if (TinyTIFFReader_wasError(tiffr)) qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr);
        //qDebug()<<"    ImageDescription:"<< TinyTIFFReader_getImageDescription(tiffr).c_str();

        uint32_t width=TinyTIFFReader_getWidth(tiffr);
        uint32_t height=TinyTIFFReader_getHeight(tiffr);

        uint32_t frames=TinyTIFFReader_countFrames(tiffr);
        //qDebug()<<"    frames: "<<frames;

        newData->label_data=new unsigned char[width*height*frames]();



        //if (TinyTIFFReader_wasError(tiffr)) qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr);
        uint32_t frame=0;
        do {
            bool ok=true;
            //if (width>0 && height>0) qDebug()<<"    size of frame "<<frame<<": "<<width<<"x"<<height;
            //else { qDebug()<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height; ok=false; }
            if (ok) {
                TinyTIFFReader_getSampleData(tiffr, &newData->label_data[frame*width*height], 0);
               // if (TinyTIFFReader_wasError(tiffr)) { ok=false; qDebug()<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr); }

                frame++;

            }
        } while (TinyTIFFReader_readNext(tiffr)); // iterate over all frames
        //qDebug()<<"    read "<<frame<<" frames";
    }
    TinyTIFFReader_close(tiffr);
    qDebug()<<"finish load label";

    if(newData->w%4!=0){
        int ad=4-newData->w%4;
        newData->w+=ad;
        unsigned char *t=new unsigned char[newData->w*newData->h*newData->d]();
        for(int z=0;z<newData->d;z++){
            for(int y=0;y<newData->h;y++){
                for(int x=0;x<newData->w-ad;x++){
                    t[z*newData->h*newData->w+y*newData->w+x]=newData->label_data[z*newData->h*(newData->w-ad)+y*(newData->w-ad)+x];
                }
            }
        }
        delete []newData->label_data;
        newData->label_data=t;

//        for(int z=0;z<newData->d;z++){
//            for(int y=0;y<newData->h;y++){
//                for(int x=0;x<newData->w-ad;x++){
//                    int tt=newData->label_data[z*newData->h*(newData->w)+y*(newData->w)+x];
//                    if(tt!=0)qDebug()<<tt;
//                }
//            }
//        }
        if(newData->type==8){
            unsigned char *t2=new unsigned char[newData->w*newData->h*newData->d]();
            for(int z=0;z<newData->d;z++){
                for(int y=0;y<newData->h;y++){
                    for(int x=0;x<newData->w-ad;x++){
                        t2[z*newData->h*newData->w+y*newData->w+x]=newData->data8[z*newData->h*(newData->w-ad)+y*(newData->w-ad)+x];
                    }
                }
            }
            delete []newData->data8;
            newData->data8=t2;

        }
        else{
            unsigned short *t2=new unsigned short[newData->w*newData->h*newData->d]();
            for(int z=0;z<newData->d;z++){
                for(int y=0;y<newData->h;y++){
                    for(int x=0;x<newData->w-ad;x++){
                        t2[z*newData->h*newData->w+y*newData->w+x]=newData->data16[z*newData->h*(newData->w-ad)+y*(newData->w-ad)+x];
                    }
                }
            }
            delete []newData->data16;
            newData->data16=t2;

        }
    }

    qDebug()<<"finish arrange data";

    //load spine info

    QFile File(basePath+"spine/spineInfo.csv");
    //qDebug()<<basePath+"spine/spineInfo.csv";

    if (File.open(QIODevice::ReadOnly))
    {
        QTextStream input(&File);
        QString headerLine = input.readLine();
        //qDebug()<<headerLine;

        QStringList headers=headerLine.split(QChar(','),QString::KeepEmptyParts);
        while (!input.atEnd())
        {
            QString line = input.readLine();
            //qDebug()<<line;

            QStringList words=line.split(QChar(','),QString::KeepEmptyParts);
            c_window->spineColors.push_back(QColor(qrand()%206+50,qrand()%206+50,qrand()%206+50));
            c_window->spineNames.push_back(words[0]);
            c_window->spineGroups.push_back(groupIndex);

            c_window->groups[groupIndex]->numSpine++;
            c_window->data.push_back(QVector<float>());
            c_window->spineCorrect.push_back(true);

            c_window->indexOfSelectedSpines.push_back(-1);
            //qDebug()<<basePath+"spine/"+words[0];

            GLWidget *cur=new GLWidget(c_window->spines.size(),basePath+"spine/"+words[0],c_window);
            connect(c_window->colorbar,SIGNAL(minValueChanged(float)),cur,SLOT(setCurvatureMin(float)));
            connect(c_window->colorbar,SIGNAL(maxValueChanged(float)),cur,SLOT(setCurvatureMax(float)));
            connect(c_window->curvatureType,SIGNAL(currentIndexChanged(int)),cur,SLOT(setCurvatureType(int)));
            connect(cur,SIGNAL(viewChange(int)),c_window,SLOT(viewChange(int)));
            connect(cur,SIGNAL(synchronization()),c_window,SLOT(synchronization()));


            c_window->spines.push_back(cur);

            //qDebug()<<"end make gl";
            c_window->spineEnable.push_back(true);
            c_window->clusterLabel.push_back(0);

            c_window->tsneResult.push_back(QVector2D(0,0));


            newData->spineIndex.push_back(c_window->spines.size()-1);
            newData->spineColors.push_back(c_window->spineColors.back());

            int index=0;
            for(int i=1;i<words.count();i++){
                //qDebug()<<words[i];
                if(c_window->featureNames[i-1]=="Type"){
                    c_window->types.push_back(QString(words[i]));
                    //qDebug()<<"type";
                    if(QString(words[i])=="None"){
                        c_window->typesInt.push_back(0);
                    }
                    else if(QString(words[i])=="Mushroom"){
                        c_window->typesInt.push_back(1);
                    }
                    else if(QString(words[i])=="Thin"){
                        c_window->typesInt.push_back(2);
                    }
                    else if(QString(words[i])=="Stubby"){
                        c_window->typesInt.push_back(3);
                    }
                    else{
                        c_window->typesInt.push_back(4);
                    }

                }
                else{
                    float value=QString(words[i]).toFloat();
                    c_window->groups[groupIndex]->totalFeature[index]+=value;
                    c_window->data[c_window->data.length()-1].push_back(value);
                    if(value<c_window->featureRanges[index].x())c_window->featureRanges[index].setX(value);
                    if(value>c_window->featureRanges[index].y())c_window->featureRanges[index].setY(value);
                    index++;
                }
            }
        }

        File.close();
    }

    qDebug()<<"finish spine info";

    //qDebug()<<"end1";



    float lmax=-1000;
    float lmin=1000;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureNames[i]=="hMin" ||
                c_window->featureNames[i]=="hMax" ||
                c_window->featureNames[i]=="nMin" ||
                c_window->featureNames[i]=="nMax" ){
            if(lmin>c_window->featureRanges[i].x())lmin=c_window->featureRanges[i].x();
            if(lmax<c_window->featureRanges[i].y())lmax=c_window->featureRanges[i].y();
        }
    }
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureNames[i]=="hMin" ||
                c_window->featureNames[i]=="hMax" ||
                c_window->featureNames[i]=="nMin" ||
                c_window->featureNames[i]=="nMax" ){
            c_window->featureRanges[i].setX(lmin);
            c_window->featureRanges[i].setY(lmax);
        }
    }
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureRanges[i].x()==c_window->featureRanges[i].y()){
            c_window->featureRanges[i].setX(c_window->featureRanges[i].x()-0.0001);
            c_window->featureRanges[i].setY(c_window->featureRanges[i].y()+0.0001);
        }
    }



    qDebug()<<"start spine label";

    QFile File2(basePath+"spineLabel.csv");
    //qDebug()<<basePath+"spineLabel.csv";

    if (File2.open(QIODevice::ReadOnly))
    {
        QTextStream input(&File2);
        QString headerLine = input.readLine();
        QStringList headers=headerLine.split(QChar(','),QString::KeepEmptyParts);
        while (!input.atEnd())
        {
            QString line = input.readLine();
            QStringList words=line.split(QChar(','),QString::KeepEmptyParts);
            int value=QString(words[1]).toFloat();
            //qDebug()<<value;
            newData->labelIndex.push_back(value);
        }

        File2.close();
    }

    qDebug()<<"finish spine label";

    glDeleteTextures(1,&newData->tex_3D);
    qDebug()<<"remove prev tex";

    glGenTextures(1,&newData->tex_3D);
    glBindTexture(GL_TEXTURE_3D,newData->tex_3D);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    if(newData->type==8){
        //glTexImage3D(GL_TEXTURE_3D,0,GL_RED,w,h,d,0,GL_RED,GL_UNSIGNED_BYTE,c_window->data8);
        glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_BYTE,newData->data8);

    }
    else if(newData->type==16){
        glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_SHORT,newData->data16);

    }

    qDebug()<<"finish load data tex";

    glDeleteTextures(1,&newData->tex_label);
    qDebug()<<"remove prev tex";

    glGenTextures(1,&newData->tex_label);
    glBindTexture(GL_TEXTURE_3D,newData->tex_label);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D,0,GL_RED,newData->w,newData->h,newData->d,0,GL_RED,GL_UNSIGNED_BYTE,newData->label_data);

    qDebug()<<"finish load label tex";


    newData->dataIndex=dataIndex;

    newData->basePath=basePath;
    prevDataIndex=-1;
    update();
    //qDebug()<<"end load data";
}

