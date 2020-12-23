///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////


#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QProcess>
#include <QColorDialog>
#include <window.h>
#include <QVector2D>
#include <QPainter>
#include <QCheckBox>


#define PI 3.14159265358979323846


GLWidget::GLWidget(int num,QString name,Window *p, QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(0)
{
    fileName=name;

    spineNum=num;
    gaussianType=false;

    curvMin=0;
    curvMax=1;
    p_fps="";
    fps_start=-1;
    fps=0;

    c_window=p;

    startTimer(10);
    QWidget::setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);

    Load3DFile(name);


    m_world.setToIdentity();
    m_world_scale_bar.setToIdentity();

    getGeometriValues();
    QVector3D axis=QVector3D::crossProduct(basePoint,QVector3D(0,-1,0));

    float a=(basePoint-QVector3D(0,-1,0)).length();
    float b=basePoint.length();
    float c=1;

    //a*a=b*b+c*c-2bccos
    //acos(b*b+c*c-a*a / 2bc)
    m_world.rotate(acos((b*b+1-a*a)/(2*b))*180/PI,axis.x(),axis.y(),axis.z());

    m_camera_o=m_world;
    //saveRotatedMesh("rotated/");
    //getFeatureValues();

}
void GLWidget::saveRotatedMesh(QString basePath){
    FILE *f=fopen((basePath+fileName).toStdString().c_str(),"w");

    QFile File(c_window->dataLocation+fileName);
    if (File.open(QIODevice::ReadOnly))
    {
        QTextStream input(&File);
        bool endHeader=false;
        while (!input.atEnd())
        {
            if(endHeader)break;
            QString line = input.readLine();
            fprintf(f,"%s\n",line.toStdString().c_str());
            QStringList words=line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
            for(int i=0;i<words.count();i++){
                if(words[i]=="end_header"){
                    endHeader=true;
                }
            }
        }
        File.close();
    }

    for(int i=0;i<nVertex;i++){
        QVector3D t(vertexs[i*3+0],vertexs[i*3+1],vertexs[i*3+2]);
        t=m_camera_o*t;
        fprintf(f,"%f %f %f\n",t.x(),t.y(),t.z());

    }
    for(int i=0;i<nFace;i++){
        fprintf(f,"3 %d %d %d\n",faces[i*3+0],faces[i*3+1],faces[i*3+2]);
    }

    fclose(f);

}
GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(5, 5);
}

QSize GLWidget::sizeHint() const
{
    return QSize(1600, 1600);
}



void GLWidget::cleanup()
{
    makeCurrent();
    delete m_program;
    m_program = 0;
    doneCurrent();
}



void GLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();
    glClearColor(1, 1, 1,1);


    m_program = new QOpenGLShaderProgram;
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile("Shaders/spineVertex.glsl");

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile("Shaders/spineFragment.glsl");

    m_program->addShader(vshader);
    m_program->addShader(fshader);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("curvature", 2);

    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_mvMatrixLoc2 = m_program->uniformLocation("mvMatrix2");

    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");
    m_pmodeLoc=m_program->uniformLocation("mode");

    m_curvMin=m_program->uniformLocation("curvMin");
    m_curvMax=m_program->uniformLocation("curvMax");
    m_gaussianType=m_program->uniformLocation("curvType");

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);





    // Our camera never changes in this example.
    m_camera.setToIdentity();
//    m_camera.translate(0, 0, 2);
    m_camera.lookAt(QVector3D(0,0,-2),QVector3D(0,0,0),QVector3D(0,1,0));

    m_program->setUniformValue(m_lightPosLoc, QVector3D(0,  0, 100));



    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vboData,nFace*8*3 * sizeof(float));

    // Store the vertex attribute bindings for the program.
    m_vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    m_vbo.release();




//    m_vao2.create();
//    QOpenGLVertexArrayObject::Binder vaoBinder2(&m_vao2);
//    m_vbo2.create();
//    m_vbo2.bind();
//    m_vbo2.allocate(vboData2,nFace2*8*3 * sizeof(float));

//    m_vbo2.bind();
//    QOpenGLFunctions *f2 = QOpenGLContext::currentContext()->functions();
//    f2->glEnableVertexAttribArray(0);
//    f2->glEnableVertexAttribArray(1);
//    f2->glEnableVertexAttribArray(2);
//    f2->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
//    f2->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
//    f2->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
//    m_vbo2.release();




    //    m_world.rotate(90,1,0,0);


    scale=1;

    m_program->release();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void GLWidget::getFeatureValues(){

    float lminX=1000;
    float lminY=1000;
    float lminZ=1000;
    float lmaxX=-1000;
    float lmaxY=-1000;
    float lmaxZ=-1000;

//    qDebug()<<nVertex;

    float *t_vertex=new float[nVertex*3];
    for(int i=0;i<nVertex;i++){
        QVector3D tV=QVector3D(vertexs[i*3+0],vertexs[i*3+1],vertexs[i*3+2]);
        tV=m_proj * m_camera * m_world * tV;
        t_vertex[i*3+0]=tV.x();
        t_vertex[i*3+1]=tV.y();
        t_vertex[i*3+2]=tV.z();
        if(tV.x()<c_window->minX)c_window->minX=tV.x();
        if(tV.y()<c_window->minY)c_window->minY=tV.y();
        if(tV.z()<c_window->minZ)c_window->minZ=tV.z();
        if(tV.x()>c_window->maxX)c_window->maxX=tV.x();
        if(tV.y()>c_window->maxY)c_window->maxY=tV.y();
        if(tV.z()>c_window->maxZ)c_window->maxZ=tV.z();
        if(tV.x()<lminX)lminX=tV.x();
        if(tV.y()<lminY)lminY=tV.y();
        if(tV.z()<lminZ)lminZ=tV.z();
        if(tV.x()>lmaxX)lmaxX=tV.x();
        if(tV.y()>lmaxY)lmaxY=tV.y();
        if(tV.z()>lmaxZ)lmaxZ=tV.z();
    }

    QVector3D t_basePoint=m_proj * m_camera * m_world * basePoint;
    if(lminY<t_basePoint.y()){
        lminY=t_basePoint.y();
    }

//    qDebug()<<lmaxY<<" "<<lminY;
    float stepSize=0.08;

    stepNum=(lmaxY-lminY)/stepSize+1;
    eachStep=new QVector3D[stepNum];
    eachStepMax=new float[stepNum];
    eachStepMin=new float[stepNum];
    eachCnt=new int[stepNum];

    headMax=-1000;
    headMin=1000;
    neckMax=1000;
    neckMin=1000;

    for(int i=0;i<stepNum;i++){
        eachStep[i]=QVector3D(0,0,0);
        eachStepMax[i]=-1000;
        eachStepMin[i]=1000;
        eachCnt[i]=0;
    }
    for(int i=0;i<nVertex;i++){
        int step=(t_vertex[i*3+1]-lminY)/stepSize;
//        qDebug()<<step;
        if(step<0)continue;
        eachStep[step]+=QVector3D(vertexs[i*3+0],vertexs[i*3+1],vertexs[i*3+2]);
//        eachStep[step]+=QVector3D(t_vertex[i*3+0],t_vertex[i*3+1],t_vertex[i*3+2]);
        eachCnt[step]++;
    }
    for(int i=0;i<stepNum;i++){
        if(eachCnt[i]!=0)eachStep[i]/=eachCnt[i];
    }
//    if(c_window->spineNames[spineNum]!="mush25.ply")eachStep[0]=basePoint;

    int centStep;
    float tL=1000;
    for(int i=0;i<nVertex;i++){
        int step=(t_vertex[i*3+1]-lminY)/stepSize;
        if(step<0)continue;
        if(abs(t_vertex[i*3+1])<tL){
            tL=abs(t_vertex[i*3+1]);
            centStep=step;
        }
        float v=(QVector3D(vertexs[i*3+0],vertexs[i*3+1],vertexs[i*3+2])-eachStep[step]).length();
        if(eachStepMax[step]<v){
            eachStepMax[step]=v;
        }
        if(eachStepMin[step]>v){
            eachStepMin[step]=v;
        }
    }
    for(int i=0;i<stepNum;i++){

        if(i<centStep){ // neck
            if(neckMax>eachStepMax[i]){
                neckMin=eachStepMin[i];
                neckMax=eachStepMax[i];
                indexNeck=i;
            }
        }
        else{  //head
            if(headMax<eachStepMax[i]){
                headMin=eachStepMin[i];
                headMax=eachStepMax[i];
                indexHead=i;
            }
        }
    }

    L=0;
    for(int i=1;i<stepNum;i++){
        L+=(eachStep[i]-eachStep[i-1]).length();
    }


    nFace2=0;

    for(int i=0;i<nFace;i++){
        int step1=(t_vertex[faces[i*3+0]*3+1]-lminY)/stepSize;
        int step2=(t_vertex[faces[i*3+1]*3+1]-lminY)/stepSize;
        int step3=(t_vertex[faces[i*3+2]*3+1]-lminY)/stepSize;
        if(step1==step2 && step2==step3){
            if( step1==indexHead-3 || step1==indexNeck){
                for(int j=0;j<3;j++){
                    vboData2[nFace2*3*8+j*8+0]=vboData[i*3*8+j*8+0];
                    vboData2[nFace2*3*8+j*8+1]=vboData[i*3*8+j*8+1];
                    vboData2[nFace2*3*8+j*8+2]=vboData[i*3*8+j*8+2];
                    vboData2[nFace2*3*8+j*8+3]=vboData[i*3*8+j*8+3];
                    vboData2[nFace2*3*8+j*8+4]=vboData[i*3*8+j*8+4];
                    vboData2[nFace2*3*8+j*8+5]=vboData[i*3*8+j*8+5];
                    vboData2[nFace2*3*8+j*8+6]=vboData[i*3*8+j*8+6];
                    vboData2[nFace2*3*8+j*8+7]=vboData[i*3*8+j*8+7];
                }
                nFace2++;
            }
        }
    }


    //qDebug()<<L<<" "<<headMin<<" "<<headMax<<" "<<neckMin<<" "<<neckMax;

    //fprintf(c_window->newFeature,"%s,%f,%f,%f,%f,%f\n",c_window->spineNames[spineNum].toStdString().c_str(),L,headMin*2,headMax*2,neckMin*2,neckMax*2);

    delete t_vertex;

//    qDebug()<<c_window->minX<<" "<<c_window->minY<<" "<<c_window->minZ<<endl;
//    qDebug()<<c_window->maxX<<" "<<c_window->maxY<<" "<<c_window->maxZ<<endl;


}

void GLWidget::getGeometriValues(){

    int *edgeMatrix=new int[nVertex*nVertex];

    for(int i=0;i<nVertex*nVertex;i++){
        edgeMatrix[i]=0;
    }

    for(int i=0;i<nFace;i++){
        edgeMatrix[faces[i*3+0]*nVertex + faces[i*3+1]]++;
        edgeMatrix[faces[i*3+1]*nVertex + faces[i*3+0]]++;
        edgeMatrix[faces[i*3+1]*nVertex + faces[i*3+2]]++;
        edgeMatrix[faces[i*3+2]*nVertex + faces[i*3+1]]++;
        edgeMatrix[faces[i*3+2]*nVertex + faces[i*3+0]]++;
        edgeMatrix[faces[i*3+0]*nVertex + faces[i*3+2]]++;
    }
    basePoint=QVector3D(0,0,0);


//    for(int i=0;i<nFace;i++){
//            if(!(edgeMatrix[faces[i*3+0]*nVertex+faces[i*3+1]]==1
//                    || edgeMatrix[faces[i*3+1]*nVertex+faces[i*3+2]]==1
//                 || edgeMatrix[faces[i*3+2]*nVertex+faces[i*3+0]]==1)){
//                vboData[i*3*8+0*8+0]=0;
//                vboData[i*3*8+0*8+1]=0;
//                vboData[i*3*8+0*8+2]=0;

//                vboData[i*3*8+1*8+0]=0;
//                vboData[i*3*8+1*8+1]=0;
//                vboData[i*3*8+1*8+2]=0;

//                vboData[i*3*8+2*8+0]=0;
//                vboData[i*3*8+2*8+1]=0;
//                vboData[i*3*8+2*8+2]=0;

//            }
//    }


    int cnt=0;
    for(int i=0;i<nVertex;i++){
        for(int j=i+1;j<nVertex;j++){
            if(edgeMatrix[i*nVertex+j]==1){
                basePoint+=QVector3D(vertexs[i*3+0],vertexs[i*3+1],vertexs[i*3+2]);
                basePoint+=QVector3D(vertexs[j*3+0],vertexs[j*3+1],vertexs[j*3+2]);
                cnt+=2;
            }
        }
    }
    basePoint/=cnt;
    delete edgeMatrix;
}



void GLWidget::paintGL()
{
    //qDebug()<<"start glwidget"+QString::number(spineNum);

    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glFrontFace(GL_CW);

    fps++;

    if( c_window->totalFrame->currentIndex()==0 && c_window->spineCorrect[spineNum]==false){
        glClearColor(0.1,0.1,0.1,1);
    }
    else if(spineNum==c_window->focusSpine && c_window->clicked==true && c_window->totalFrame->currentIndex()==0){
        glClearColor(c_window->spineColors[spineNum].redF(),c_window->spineColors[spineNum].greenF(),c_window->spineColors[spineNum].blueF(),1);
    }
    else if(spineNum==c_window->focusSpine && c_window->clicked==false){
        glClearColor(c_window->spineColors[spineNum].redF(),c_window->spineColors[spineNum].greenF(),c_window->spineColors[spineNum].blueF(),1);
    }
    else{
        glClearColor(1, 1, 1, 1);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glEnable(GL_CULL_FACE);


//    glShadeModel(GL_SMOOTH);


    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    m_program->setUniformValue(m_mvMatrixLoc2, m_camera * m_world_scale_bar);

    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    m_program->setUniformValue(m_curvMin,curvMin);

    m_program->setUniformValue(m_curvMax,curvMax);
    m_program->setUniformValue(m_gaussianType,gaussianType);


    if(c_window->drawLine->isChecked()){
        m_program->setUniformValue(m_pmodeLoc,0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, nFace*3);
    }

    m_program->setUniformValue(m_pmodeLoc,1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, nFace*3);

//    m_program->setUniformValue(m_pmodeLoc,5);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glDrawArrays(GL_TRIANGLES, 0, nFace*3);


//    QOpenGLVertexArrayObject::Binder vaoBinder2(&m_vao2);
//    m_program->setUniformValue(m_pmodeLoc,4);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glDrawArrays(GL_TRIANGLES, 0, nFace2*3);


    vaoBinder.release();

    m_program->setUniformValue(m_pmodeLoc,3); // Scale bar
    glBegin(GL_LINES);
    glVertex3f(-0.8,-0.5,0);
    glVertex3f(-0.8,0.5,0);

    glVertex3f(-0.75,-0.5,0);
    glVertex3f(-0.85,-0.5,0);

    glVertex3f(-0.75,0.5,0);
    glVertex3f(-0.85,0.5,0);

//    glVertex3f(-0.85,-0.5,0);
//    glVertex3f(-0.85,0.5,0);
//    glVertex3f(-0.8,0.5,0);
//    glVertex3f(-0.8,-0.5,0);
    glEnd();



//    m_program->setUniformValue(m_pmodeLoc,2);
//    glBegin(GL_QUADS);
//    for(int i=1;i<stepNum;i++){
//        glVertex3f(eachStep[i-1].x()+0.01,eachStep[i-1].y(),eachStep[i-1].z());
//        glVertex3f(eachStep[i-1].x()-0.01,eachStep[i-1].y(),eachStep[i-1].z());
//        glVertex3f(eachStep[i].x()-0.01,eachStep[i].y(),eachStep[i].z());
//        glVertex3f(eachStep[i].x()+0.01,eachStep[i].y(),eachStep[i].z());

//    }
//    glEnd();

//    glColor4f(1,0,0,1);
//    glBegin(GL_QUADS);
//    glVertex3f(-0.01,0,0);
//    glVertex3f(+0.01,0,0);
//    glVertex3f(basePoint.x()+0.01,basePoint.y(),basePoint.z());
//    glVertex3f(basePoint.x()-0.01,basePoint.y(),basePoint.z());
//    glEnd();




    m_program->release();



//    if(c_window->totalFrame->currentIndex()==0){
//        glColor4f(c_window->spineColors[spineNum].redF(),c_window->spineColors[spineNum].greenF(),c_window->spineColors[spineNum].blueF(),1);
//    }
    if(c_window->coloringType==0){
        glColor4f(c_window->spineColors[spineNum].redF(),c_window->spineColors[spineNum].greenF(),c_window->spineColors[spineNum].blueF(),1);
    }
    else if(c_window->coloringType==1){
        glColor4f(c_window->clusterColors[c_window->clusterLabel[spineNum]].redF(),c_window->clusterColors[c_window->clusterLabel[spineNum]].greenF(),c_window->clusterColors[c_window->clusterLabel[spineNum]].blueF(),1);
    }
    else if(c_window->coloringType==2){
        if(c_window->indexOfSelectedSpines[spineNum]==-1){
            glColor4f(50.0/255.0,50.0/255.0,50.0/255.0,30.0/255.0);

//            t_pen.setColor(QColor(50,50,50,30));
        }
        else glColor4f(c_window->clusterColors[c_window->indexOfSelectedSpines[spineNum]].redF(),c_window->clusterColors[c_window->indexOfSelectedSpines[spineNum]].greenF(),c_window->clusterColors[c_window->indexOfSelectedSpines[spineNum]].blueF(),1);

    }
    else if(c_window->coloringType==3 && c_window->typeEnable){
        glColor4f(c_window->typeColors[c_window->typesInt[spineNum]].redF(),c_window->typeColors[c_window->typesInt[spineNum]].greenF(),c_window->typeColors[c_window->typesInt[spineNum]].blueF(),1);
    }
    else if(c_window->coloringType==4){
        QColor t=c_window->groups[c_window->spineGroups[spineNum]]->color;
        glColor4f(t.redF(),t.greenF(),t.blueF(),1);
    }


//    glColor4f(c_window->spineColors[spineNum].redF(),c_window->spineColors[spineNum].greenF(),c_window->spineColors[spineNum].blueF(),1);
    glBegin(GL_QUADS);
    glVertex3f(-1,-1,0);
    glVertex3f(-1,1,0);
    glVertex3f(-0.9,1,0);
    glVertex3f(-0.9,-1,0);
    glEnd();



    glColor3f(0.7,0.7,0.7);
    glBegin(GL_LINES);
    glVertex3f(-1,-0.99,0);
    glVertex3f(1,-0.99,0);
    glVertex3f(1,-0.99,0);
    glVertex3f(1,1,0);
    glEnd();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glFrontFace(GL_CCW);
//    glDisable(GL_CULL_FACE);



    QPainter painter(this);
    painter.setPen(QColor(255,128,128,255));
//    painter.drawLine(10,10,100,100);
    QFont t;
    t.setBold(false);
    painter.setFont(t);
    painter.drawText(this->width()*0.7, this->height()*0.5+this->height()*0.3*scale,this->width()*0.4,20,Qt::AlignCenter, QString("1")+QChar(181)+QString("m"));

    if(negativeCurvatureArea!=-1){
        t.setPointSize(12);
        t.setBold(true);
        t.setWeight(QFont::Black);
        painter.setFont(t);

        painter.setPen(QColor(0,0,100,255));
        painter.drawText(fixedPos.x()*this->width()+2,fixedPos.y()*this->height()+2,QString("Area: ")+QString::number(negativeCurvatureArea));

        painter.setPen(QColor(220,255,255,255));
        painter.drawText(fixedPos.x()*this->width(),fixedPos.y()*this->height(),QString("Area: ")+QString::number(negativeCurvatureArea));
    }

    painter.end();

    //qDebug()<<"end glwidget";

}
void GLWidget::getPixelNum(){
    int tw=c_window->width();
    tw=tw-tw%4;
    GLubyte *pixels = new GLubyte[c_window->height()*tw*3];
    glReadPixels(0, 0, tw, this->height(), GL_RGB, GL_UNSIGNED_BYTE, pixels);
    for(int i=0;i<c_window->height()*tw;i++){
        if(pixels[i*3+0]!=255 || pixels[i*3+1]!=255 || pixels[i*3+2]!=255){
            c_window->spine_pixel_num++;
        }
    }
    c_window->total_pixel_num+=c_window->height()*tw;
    delete[] pixels;

}
void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
//    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);

    float t=w>h?w:h;

    m_proj.ortho(-w/t,w/t,-h/t,h/t,-20,20);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    mousePressedPos=event->pos();
    if(c_window->totalFrame->currentIndex()==1){
        if(event->x()<this->width()*0.05){

            if(c_window->spineEnableMode->isChecked()){
                c_window->displaySpines[spineNum]=true;
                return;
            }


            if(c_window->IsShift){
                c_window->addSpineToGroup(spineNum,c_window->currentSelectGroup);

            }
            else{
                if(c_window->clicked==false){
                    c_window->clicked=true;
                    c_window->focusSpine=spineNum;
                    emit viewChange(spineNum);
                    emit synchronization();

                }
                else if(c_window->clicked==true){
                    c_window->clicked=false;
                    c_window->focusSpine=-1;
                    emit viewChange(-1);
                    emit synchronization();

                }
            }

        }
    }

}
void GLWidget::mouseReleaseEvent(QMouseEvent *event){
    QPoint t=mousePressedPos-event->pos();
    if(abs(t.x())+abs(t.y())<1){

        if(c_window->totalFrame->currentIndex()==0){

            if(c_window->IsShift){
                c_window->spineCorrect[spineNum]=!c_window->spineCorrect[spineNum];
            }
            else{

                if(c_window->clicked==false && event->x()<this->width()*0.2){
                    c_window->clicked=true;
                    c_window->focusSpine=spineNum;
        //            emit viewChange(spineNum);
                    emit synchronization();

                }
                else if(c_window->clicked==true){
                    c_window->clicked=false;
                    c_window->focusSpine=-1;
        //            emit viewChange(-1);
                    emit synchronization();
                }
            }

            update();
        }
        else{
            if(negativeCurvatureArea!=-1){
                setNegativeCurvatureArea();
                c_window->synchronization();

            }

        }
    }
}
void GLWidget::setNegativeCurvatureArea(){
    int sa=0;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureNames[i]=="SA")sa=i;
    }

    if(fixedNegativeCurvature){
        fixedNegativeCurvature=false;
        negativeCurvatureArea=-1;
        c_window->data[spineNum][sa]=0;
    }
    else{
        fixedNegativeCurvature=true;
        c_window->data[spineNum][sa]=negativeCurvatureArea;
    }

    c_window->featureRanges[sa].setX(0);
    c_window->featureRanges[sa].setY(0);

    for(int i=0;i<c_window->data.length();i++){
        if(c_window->featureRanges[sa].x()>c_window->data[i][sa]){
            c_window->featureRanges[sa].setX(c_window->data[i][sa]);
        }
        if(c_window->featureRanges[sa].y()<c_window->data[i][sa]){
            c_window->featureRanges[sa].setY(c_window->data[i][sa]);
        }
    }
    if(c_window->featureRanges[sa].x()==c_window->featureRanges[sa].y()){
        c_window->featureRanges[sa].setX(c_window->featureRanges[sa].x()-0.0001);
        c_window->featureRanges[sa].setY(c_window->featureRanges[sa].y()+0.0001);

    }


    c_window->minValues[sa]=1000;
    c_window->maxValues[sa]=-1000;
    c_window->avgValues[sa]=0;
    for(int c=0;c<c_window->dataRendering->dataList.length();c++){
        for(int i=0;i<c_window->dataRendering->dataList[c]->spineIndex.length();i++){
            int v=c_window->dataRendering->dataList[c]->spineIndex[i];
            c_window->avgValues[sa]+=c_window->data[v][sa];
            if(c_window->data[v][sa]<c_window->minValues[sa]){
                c_window->minValues[sa]=c_window->data[v][sa];
            }
            if(c_window->data[v][sa]>c_window->maxValues[sa]){
                c_window->maxValues[sa]=c_window->data[v][sa];
            }
        }
        c_window->avgValues[sa]/=c_window->dataRendering->dataList[c]->spineIndex.length();
    }

    for(int c=0;c<c_window->groups.length();c++){
        c_window->groups[c]->totalFeature[sa]=0;
        for(int i=0;i<c_window->groups[c]->dataList->count();i++){
            int index=c_window->groups[c]->dataList->item(i)->whatsThis().toInt();
            int len=c_window->dataRendering->dataList[index]->spineIndex.length();
            for(int j=0;j<len;j++){
                int spineIndex=c_window->dataRendering->dataList[index]->spineIndex[j];
                c_window->groups[c]->totalFeature[sa]+=c_window->data[spineIndex][sa];
            }
        }
    }
}
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

    if(event->x()<this->width()*0.05){
        if(c_window->clicked==false){
            c_window->focusSpine=spineNum;
            emit synchronization();

        }
    }
    else{
        if(c_window->clicked==false){
            c_window->focusSpine=-1;
            emit synchronization();

        }
    }


    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    float t=this->width()>this->height()?this->width():this->height();


    QVector3D cur=QVector3D(event->x()/t,event->y()/t,-2)*m_world;
    QVector3D prev=QVector3D(m_lastPos.x()/t,m_lastPos.y()/t,-2)*m_world;

    if((cur-prev).length()>0.01){
        if(fixedNegativeCurvature==false)
            getNegativeCurvatureArea(event->pos());

        if (event->buttons() & Qt::LeftButton) {
                m_world.rotate(acos((cur.length()*cur.length()+prev.length()*prev.length()-(cur-prev).length()*(cur-prev).length())/(2*cur.length()*prev.length()))*360/scale
                           ,cross_product(cur,prev));
        }
        if (event->buttons() & Qt::RightButton) {
            m_world.scale(1-dy/t);
            m_world_scale_bar.scale(1-dy/t);
            scale*=1-dy/t;
        }
        if (event->buttons() & Qt::MiddleButton) {
            m_world.translate(QVector3D(-dx/t,-dy/t,0)*m_world/scale);
        }
        m_lastPos = event->pos();
    }
}

void GLWidget::getNegativeCurvatureArea(QPoint mousePos){

    fixedPos=QPointF(float(mousePos.x())/this->width(),float(mousePos.y())/this->height());

    for(int i=0;i<nFace;i++){
        for(int j=0;j<3;j++){
            vboData[i*3*8+j*8+6]=meanCurvature[faces[i*3+j]];
            vboData[i*3*8+j*8+7]=gaussianCurvature[faces[i*3+j]];
        }
    }
    negativeCurvatureArea=-1;

    QVector2D globalMousePos(float(mousePos.x())/this->width()*2-1,float(-mousePos.y())/this->height()*2+1);
    float minDis=10000;
    float maxDepth=-10000;
    int pointedV=-1;
    for(int i=0;i<nVertex;i++){
        QVector3D v(vertexs[i*3+0],vertexs[i*3+1],vertexs[i*3+2]);
        QVector3D cam_v=m_camera * m_world * v;
        QVector3D proj_v=m_proj * cam_v;

        float dis=(QVector2D(proj_v.x(),proj_v.y())-globalMousePos).length();
        if(dis<0.025*scale){
            if(cam_v.z()>maxDepth){
                maxDepth=cam_v.z();
                pointedV=i;
            }
        }

    }

    if(pointedV==-1){
        m_vbo.bind();
        m_vbo.allocate(vboData,nFace*8*3 * sizeof(float));
        m_vbo.release();
        return;
    }
    QVector3D v(vertexs[pointedV*3+0],vertexs[pointedV*3+1],vertexs[pointedV*3+2]);
    QVector3D cam_v=m_camera * m_world * v;
    QVector3D proj_v=m_proj * cam_v;


    float midCurvature=(c_window->colorbar->minV+c_window->colorbar->maxV)/2.0;
    if(c_window->curvatureType->currentIndex()==0){
        if(meanCurvature[pointedV]>midCurvature){
            for(int i=0;i<facesOfVertex[pointedV].length();i++){
                int curf=facesOfVertex[pointedV][i];
                for(int j=0;j<3;j++){
                    vboData[curf*3*8+j*8+6]=-2;
                    vboData[curf*3*8+j*8+7]=-2;
                }
            }
            m_vbo.bind();
            m_vbo.allocate(vboData,nFace*8*3 * sizeof(float));
            m_vbo.release();
            return;
        }
    }
    else{
        if(gaussianCurvature[pointedV]>midCurvature){
            for(int i=0;i<facesOfVertex[pointedV].length();i++){
                int curf=facesOfVertex[pointedV][i];
                for(int j=0;j<3;j++){
                    vboData[curf*3*8+j*8+6]=-2;
                    vboData[curf*3*8+j*8+7]=-2;
                }
            }
            m_vbo.bind();
            m_vbo.allocate(vboData,nFace*8*3 * sizeof(float));
            m_vbo.release();
            return;
        }
    }


    fixedVert=pointedV;

    negativeCurvatureArea=0;
    QVector <int> queue;
    queue.push_back(pointedV);
    bool *check=new bool[nVertex]();
    bool *faceCheck=new bool[nFace]();

    check[pointedV]=true;


    for(int i=0;i<facesOfVertex[pointedV].length();i++){
        int curf=facesOfVertex[pointedV][i];
        faceCheck[curf]=true;
        negativeCurvatureArea+=facesArea[curf];

        for(int j=0;j<3;j++){
            vboData[curf*3*8+j*8+6]=-1+vboData[curf*3*8+j*8+6];
            vboData[curf*3*8+j*8+7]=-1+vboData[curf*3*8+j*8+6];
        }
    }
    while(!queue.empty()){
        int cur_vertex=queue.back();
        queue.pop_back();
        for(int i=0;i<neighborVertices[cur_vertex].length();i++){
            int next_vertex=neighborVertices[cur_vertex][i];
            if(check[next_vertex])continue;
            if(c_window->curvatureType->currentIndex()==0){
                if(meanCurvature[next_vertex]<=midCurvature){
                    check[next_vertex]=true;
                    queue.push_back(next_vertex);
                    for(int j=0;j<facesOfVertex[next_vertex].length();j++){
                        int curf=facesOfVertex[next_vertex][j];
                        if(!faceCheck[curf]){
                            faceCheck[curf]=true;
                            negativeCurvatureArea+=facesArea[curf];
                            for(int k=0;k<3;k++){
                                vboData[curf*3*8+k*8+6]=-1+vboData[curf*3*8+k*8+6];
                                vboData[curf*3*8+k*8+7]=-1+vboData[curf*3*8+k*8+6];
                            }
                        }
                    }
                }
            }
            else{
                if(gaussianCurvature[next_vertex]<=midCurvature){
                    check[next_vertex]=true;
                    queue.push_back(next_vertex);
                    for(int j=0;j<facesOfVertex[next_vertex].length();j++){
                        int curf=facesOfVertex[next_vertex][j];
                        for(int k=0;k<3;k++){
                            vboData[curf*3*8+k*8+6]=-1+vboData[curf*3*8+k*8+6];
                            vboData[curf*3*8+k*8+7]=-1+vboData[curf*3*8+k*8+6];
                        }
                    }
                }
            }
        }
    }
    for(int i=0;i<facesOfVertex[pointedV].length();i++){
        int curf=facesOfVertex[pointedV][i];
        for(int j=0;j<3;j++){
            vboData[curf*3*8+j*8+6]=-2;
            vboData[curf*3*8+j*8+7]=-2;
        }
    }
    m_vbo.bind();
    m_vbo.allocate(vboData,nFace*8*3 * sizeof(float));
    m_vbo.release();
    delete []check;
    delete []faceCheck;
}
void GLWidget::getNegativeCurvatureAreaLoad(QPoint mousePos,int pointedV){

    fixedPos=QPointF(float(mousePos.x())/this->width(),float(mousePos.y())/this->height());

    for(int i=0;i<nFace;i++){
        for(int j=0;j<3;j++){
            vboData[i*3*8+j*8+6]=meanCurvature[faces[i*3+j]];
            vboData[i*3*8+j*8+7]=gaussianCurvature[faces[i*3+j]];
        }
    }
    negativeCurvatureArea=-1;

    QVector3D v(vertexs[pointedV*3+0],vertexs[pointedV*3+1],vertexs[pointedV*3+2]);
    QVector3D cam_v=m_camera * m_world * v;
    QVector3D proj_v=m_proj * cam_v;


    float midCurvature=(c_window->colorbar->minV+c_window->colorbar->maxV)/2.0;
    if(c_window->curvatureType->currentIndex()==0){
        if(meanCurvature[pointedV]>midCurvature){
            for(int i=0;i<facesOfVertex[pointedV].length();i++){
                int curf=facesOfVertex[pointedV][i];
                for(int j=0;j<3;j++){
                    vboData[curf*3*8+j*8+6]=-2;
                    vboData[curf*3*8+j*8+7]=-2;
                }
            }
            m_vbo.bind();
            m_vbo.allocate(vboData,nFace*8*3 * sizeof(float));
            m_vbo.release();
            return;
        }
    }
    else{
        if(gaussianCurvature[pointedV]>midCurvature){
            for(int i=0;i<facesOfVertex[pointedV].length();i++){
                int curf=facesOfVertex[pointedV][i];
                for(int j=0;j<3;j++){
                    vboData[curf*3*8+j*8+6]=-2;
                    vboData[curf*3*8+j*8+7]=-2;
                }
            }
            m_vbo.bind();
            m_vbo.allocate(vboData,nFace*8*3 * sizeof(float));
            m_vbo.release();
            return;
        }
    }


    fixedVert=pointedV;

    negativeCurvatureArea=0;
    QVector <int> queue;
    queue.push_back(pointedV);
    bool *check=new bool[nVertex]();
    bool *faceCheck=new bool[nFace]();

    check[pointedV]=true;


    for(int i=0;i<facesOfVertex[pointedV].length();i++){
        int curf=facesOfVertex[pointedV][i];
        faceCheck[curf]=true;
        negativeCurvatureArea+=facesArea[curf];

        for(int j=0;j<3;j++){
            vboData[curf*3*8+j*8+6]=-1+vboData[curf*3*8+j*8+6];
            vboData[curf*3*8+j*8+7]=-1+vboData[curf*3*8+j*8+6];
        }
    }
    while(!queue.empty()){
        int cur_vertex=queue.back();
        queue.pop_back();
        for(int i=0;i<neighborVertices[cur_vertex].length();i++){
            int next_vertex=neighborVertices[cur_vertex][i];
            if(check[next_vertex])continue;
            if(c_window->curvatureType->currentIndex()==0){
                if(meanCurvature[next_vertex]<=midCurvature){
                    check[next_vertex]=true;
                    queue.push_back(next_vertex);
                    for(int j=0;j<facesOfVertex[next_vertex].length();j++){
                        int curf=facesOfVertex[next_vertex][j];
                        if(!faceCheck[curf]){
                            faceCheck[curf]=true;
                            negativeCurvatureArea+=facesArea[curf];
                            for(int k=0;k<3;k++){
                                vboData[curf*3*8+k*8+6]=-1+vboData[curf*3*8+k*8+6];
                                vboData[curf*3*8+k*8+7]=-1+vboData[curf*3*8+k*8+6];
                            }
                        }
                    }
                }
            }
            else{
                if(gaussianCurvature[next_vertex]<=midCurvature){
                    check[next_vertex]=true;
                    queue.push_back(next_vertex);
                    for(int j=0;j<facesOfVertex[next_vertex].length();j++){
                        int curf=facesOfVertex[next_vertex][j];
                        for(int k=0;k<3;k++){
                            vboData[curf*3*8+k*8+6]=-1+vboData[curf*3*8+k*8+6];
                            vboData[curf*3*8+k*8+7]=-1+vboData[curf*3*8+k*8+6];
                        }
                    }
                }
            }
        }
    }
    for(int i=0;i<facesOfVertex[pointedV].length();i++){
        int curf=facesOfVertex[pointedV][i];
        for(int j=0;j<3;j++){
            vboData[curf*3*8+j*8+6]=-2;
            vboData[curf*3*8+j*8+7]=-2;
        }
    }
    m_vbo.bind();
    m_vbo.allocate(vboData,nFace*8*3 * sizeof(float));
    m_vbo.release();
    delete []check;
    delete []faceCheck;

    fixedNegativeCurvature=true;



    int sa=0;
    for(int i=0;i<c_window->featureNum;i++){
        if(c_window->featureNames[i]=="SA")sa=i;
    }

    c_window->data[spineNum][sa]=negativeCurvatureArea;

    c_window->featureRanges[sa].setX(0);
    c_window->featureRanges[sa].setY(0);

    for(int i=0;i<c_window->data.length();i++){
        if(c_window->featureRanges[sa].x()>c_window->data[i][sa]){
            c_window->featureRanges[sa].setX(c_window->data[i][sa]);
        }
        if(c_window->featureRanges[sa].y()<c_window->data[i][sa]){
            c_window->featureRanges[sa].setY(c_window->data[i][sa]);
        }
    }
    if(c_window->featureRanges[sa].x()==c_window->featureRanges[sa].y()){
        c_window->featureRanges[sa].setX(c_window->featureRanges[sa].x()-0.0001);
        c_window->featureRanges[sa].setY(c_window->featureRanges[sa].y()+0.0001);

    }


    c_window->minValues[sa]=1000;
    c_window->maxValues[sa]=-1000;
    c_window->avgValues[sa]=0;
    for(int c=0;c<c_window->dataRendering->dataList.length();c++){
        for(int i=0;i<c_window->dataRendering->dataList[c]->spineIndex.length();i++){
            int v=c_window->dataRendering->dataList[c]->spineIndex[i];
            c_window->avgValues[sa]+=c_window->data[v][sa];
            if(c_window->data[v][sa]<c_window->minValues[sa]){
                c_window->minValues[sa]=c_window->data[v][sa];
            }
            if(c_window->data[v][sa]>c_window->maxValues[sa]){
                c_window->maxValues[sa]=c_window->data[v][sa];
            }
        }
        c_window->avgValues[sa]/=c_window->dataRendering->dataList[c]->spineIndex.length();
    }

    for(int c=0;c<c_window->groups.length();c++){
        c_window->groups[c]->totalFeature[sa]=0;
        for(int i=0;i<c_window->groups[c]->dataList->count();i++){
            int index=c_window->groups[c]->dataList->item(i)->whatsThis().toInt();
            int len=c_window->dataRendering->dataList[index]->spineIndex.length();
            for(int j=0;j<len;j++){
                int spineIndex=c_window->dataRendering->dataList[index]->spineIndex[j];
                c_window->groups[c]->totalFeature[sa]+=c_window->data[spineIndex][sa];
            }
        }
    }

}
void GLWidget::timerEvent(QTimerEvent *event){

    if(c_window->enableRotation)m_world.rotate(1,basePoint);

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
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape){
        if(c_window->clicked==true){
            c_window->clicked=false;
            c_window->focusSpine=-1;
            emit viewChange(-1);
            emit synchronization();

        }
    }
    else if(event->key()==Qt::Key_R){
//        m_world.setToIdentity();
//        m_world_scale_bar.setToIdentity();
//        scale=1;

        c_window->enableRotation=!c_window->enableRotation;



    }
    if(event->key()==Qt::Key_Shift){
        c_window->IsShift=true;
    }
}
void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Shift){
        c_window->IsShift=false;
    }
}

void GLWidget::Load3DFile(QString name){


//        QFile t_f("test.txt");
//        t_f.open(QIODevice::WriteOnly);
//        QTextStream output(&t_f);

//    float minXCoord=100;
//    float maxXCoord=-100;
//    float minYCoord=100;
//    float maxYCoord=-100;
//    float minZCoord=100;
//    float maxZCoord=-100;

    bool IsNormal=true;

    float centX=0;
    float centY=0;
    float centZ=0;
//    char c_num[10];
//    itoa(name,c_num,10);
    QFile File(name);
    if (File.open(QIODevice::ReadOnly))
    {
        bool endHeader=false;
        int vertexCount=0;
        int faceCount=0;
        bool endVertex=false;
        QTextStream input(&File);
        while (!input.atEnd())
        {
            QString line = input.readLine();
            QStringList words=line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
            if(endHeader){
                if(!endVertex){
                    vertexs[vertexCount*3+0]=QString(words[0]).toFloat();
                    vertexs[vertexCount*3+1]=QString(words[1]).toFloat();
                    vertexs[vertexCount*3+2]=QString(words[2]).toFloat();
                    centX+=vertexs[vertexCount*3+0]/nVertex;
                    centY+=vertexs[vertexCount*3+1]/nVertex;
                    centZ+=vertexs[vertexCount*3+2]/nVertex;

//                    if(vertexs[vertexCount*3+0]<minXCoord)minXCoord=vertexs[vertexCount*3+0];
//                    if(vertexs[vertexCount*3+0]>maxXCoord)maxXCoord=vertexs[vertexCount*3+0];
//                    if(vertexs[vertexCount*3+1]<minYCoord)minYCoord=vertexs[vertexCount*3+1];
//                    if(vertexs[vertexCount*3+1]>maxYCoord)maxYCoord=vertexs[vertexCount*3+1];
//                    if(vertexs[vertexCount*3+2]<minZCoord)minZCoord=vertexs[vertexCount*3+2];
//                    if(vertexs[vertexCount*3+2]>maxZCoord)maxZCoord=vertexs[vertexCount*3+2];

                    if(words.length()>3){

                        normals[vertexCount*3+0]=QString(words[3]).toFloat();
                        normals[vertexCount*3+1]=QString(words[4]).toFloat();
                        normals[vertexCount*3+2]=QString(words[5]).toFloat();
                    }
                    else{
                        IsNormal=false;
                    }
                    vertexCount++;
                    if(vertexCount==nVertex)endVertex=true;
                }
                else{
                    faces[faceCount*3+0]=QString(words[1]).toInt();
                    faces[faceCount*3+1]=QString(words[2]).toInt();
                    faces[faceCount*3+2]=QString(words[3]).toInt();
                    faceCount++;
                }

            }
            else{
                for(int i=0;i<words.count();i++){
                    if(words[i]=="vertex"){
                        QString t=words[i+1];
                        nVertex=t.toInt();
                        vertexs=new float[nVertex*3];
                        normals=new float[nVertex*3];
                    }
                    if(words[i]=="face"){
                        QString t=words[i+1];
                        nFace=t.toInt();
                        faces=new int[nFace*3];
                    }
                    if(words[i]=="end_header"){
                        endHeader=true;

                    }
                }
            }
        }
        File.close();
    }



    if(IsNormal==false){
        getNormal();
    }


//    float xLength=maxXCoord-minXCoord;
//    float yLength=maxYCoord-minYCoord;
//    float zLength=maxZCoord-minZCoord;
//    float scale_factor;
//    if(xLength>yLength && xLength>zLength){
//        scale_factor=1.0/xLength;
//    }
//    else if(yLength>xLength && yLength>zLength){
//        scale_factor=1.0/yLength;
//    }
//    else{
//        scale_factor=1.0/zLength;
//    }

//    scale_factor=1;


    for(int i=0;i<nVertex;i++){
        vertexs[i*3+0]=(vertexs[i*3+0]-centX);
        vertexs[i*3+1]=(vertexs[i*3+1]-centY);
        vertexs[i*3+2]=(vertexs[i*3+2]-centZ);

    }

    //centroid=QVector3D(centX,centY,centZ);


    meanCurvature=new float[nVertex];
    gaussianCurvature=new float[nVertex];
    for(int i=0;i<nVertex;i++){
        meanCurvature[i]=0;
        gaussianCurvature[i]=0;
    }

    int curvatureCount=0;
    float minMC=1000;
    float maxMC=-1000;
    float minGC=1000;
    float maxGC=-1000;
    //itoa(name,c_num,10);
    QFile File2(name+"curve");
    if (File2.open(QIODevice::ReadOnly))
    {
        QTextStream input(&File2);
        while (!input.atEnd())
        {
            QString line = input.readLine();
            QStringList words=line.split(QRegExp("\\s+"),QString::SkipEmptyParts);
            meanCurvature[curvatureCount]=QString(words[0]).toFloat();
            gaussianCurvature[curvatureCount]=QString(words[1]).toFloat();

            if(meanCurvature[curvatureCount]<minMC)minMC=meanCurvature[curvatureCount];
            if(meanCurvature[curvatureCount]>maxMC)maxMC=meanCurvature[curvatureCount];
            if(gaussianCurvature[curvatureCount]<minGC)minGC=gaussianCurvature[curvatureCount];
            if(gaussianCurvature[curvatureCount]>maxGC)maxGC=gaussianCurvature[curvatureCount];
            curvatureCount++;
        }
        File2.close();
//        minMC=-10,maxMC=10;
//        minGC=-100,maxGC=100;
        for(int i=0;i<nVertex;i++){
            if(meanCurvature[i]<0){
                if(minMC!=maxMC)meanCurvature[i]=(minMC-meanCurvature[i])/minMC/2.0;
            }
            else{
                if(minMC!=maxMC)meanCurvature[i]=(meanCurvature[i])/maxMC/2.0+0.5;
            }
            if(gaussianCurvature[i]<0){
                if(minGC!=maxGC)gaussianCurvature[i]=(minGC-gaussianCurvature[i])/minGC/2.0;
            }
            else{
                if(minGC!=maxGC)gaussianCurvature[i]=(gaussianCurvature[i])/maxGC/2.0+0.5;
            }
        }
    }

    vboData2=new float[nFace*3*8];
    vboData=new float[nFace*3*8];
    for(int i=0;i<nFace;i++){
        for(int j=0;j<3;j++){
            vboData[i*3*8+j*8+0]=vertexs[faces[i*3+j]*3+0];
            vboData[i*3*8+j*8+1]=vertexs[faces[i*3+j]*3+1];
            vboData[i*3*8+j*8+2]=vertexs[faces[i*3+j]*3+2];
            vboData[i*3*8+j*8+3]=normals[faces[i*3+j]*3+0];
            vboData[i*3*8+j*8+4]=normals[faces[i*3+j]*3+1];
            vboData[i*3*8+j*8+5]=normals[faces[i*3+j]*3+2];
            vboData[i*3*8+j*8+6]=meanCurvature[faces[i*3+j]];
            vboData[i*3*8+j*8+7]=gaussianCurvature[faces[i*3+j]];
        }
    }

    facesArea=new float[nFace];
    for(int i=0;i<nFace;i++){
        QVector3D a=QVector3D(vertexs[faces[i*3+0]*3+0],vertexs[faces[i*3+0]*3+1],vertexs[faces[i*3+0]*3+2]);
        QVector3D b=QVector3D(vertexs[faces[i*3+1]*3+0],vertexs[faces[i*3+1]*3+1],vertexs[faces[i*3+1]*3+2]);
        QVector3D c=QVector3D(vertexs[faces[i*3+2]*3+0],vertexs[faces[i*3+2]*3+1],vertexs[faces[i*3+2]*3+2]);
        QVector3D ts=cross_product(b-a,c-a);
        facesArea[i]=0.5*sqrt(QVector3D::dotProduct(ts,ts));
    }


    generateNeighborVertexList();
//    for(int i=0;i<nVertex;i++){
//        output<<vertexs[i*3+0]<<" "<<vertexs[i*3+1]<<" "<<vertexs[i*3+2]<<" "<<normals[i*3+0]<<" "<<normals[i*3+1]<<" "<<normals[i*3+2]<<endl;
//    }
//    for(int i=0;i<nFace;i++){
//        output<<faces[i*3+0]<<" "<<faces[i*3+1]<<" "<<faces[i*3+2]<<endl;
//    }
//    t_f.close();

}
void GLWidget::generateNeighborVertexList(){
    facesOfVertex=new QVector<int>[nVertex];
    neighborVertices=new QVector<int>[nVertex];
    for(int i=0;i<nVertex;i++){
        for(int j=0;j<nFace;j++){
            int v[3]={faces[j*3+0],faces[j*3+1],faces[j*3+2]};
            if(v[0]==i){
                if(!neighborVertices[i].contains(v[1]))neighborVertices[i].push_back(v[1]);
                if(!neighborVertices[i].contains(v[2]))neighborVertices[i].push_back(v[2]);

                if(!facesOfVertex[i].contains(j))facesOfVertex[i].push_back(j);
            }
            else if(v[1]==i){
                if(!neighborVertices[i].contains(v[0]))neighborVertices[i].push_back(v[0]);
                if(!neighborVertices[i].contains(v[2]))neighborVertices[i].push_back(v[2]);

                if(!facesOfVertex[i].contains(j))facesOfVertex[i].push_back(j);
            }
            else if(v[2]==i){
                if(!neighborVertices[i].contains(v[0]))neighborVertices[i].push_back(v[0]);
                if(!neighborVertices[i].contains(v[1]))neighborVertices[i].push_back(v[1]);

                if(!facesOfVertex[i].contains(j))facesOfVertex[i].push_back(j);
            }
        }
    }
}
QVector3D GLWidget::cross_product(QVector3D v1,QVector3D v2){
    QVector3D v;
    v.setX(v1.y()*v2.z()-v2.y()*v1.z());
    v.setY(v1.z()*v2.x()-v2.z()*v1.x());
    v.setZ(v1.x()*v2.y()-v2.x()*v1.y());
    return v;
}

void GLWidget::setCurvatureMin(float n){
    curvMin=n;
}
void GLWidget::setCurvatureMax(float n){
    curvMax=n;

}
void GLWidget::setCurvatureType(int t){
    gaussianType=t;
}
void GLWidget::getNormal(){
    int *normalCount=new int[nVertex];
    for(int i=0;i<nVertex;i++){
        normalCount[i]=0;
    }
    for(int i=0;i<nFace;i++){
        QVector3D a=QVector3D(vertexs[faces[i*3+0]*3+0],vertexs[faces[i*3+0]*3+1],vertexs[faces[i*3+0]*3+2]);
        QVector3D b=QVector3D(vertexs[faces[i*3+1]*3+0],vertexs[faces[i*3+1]*3+1],vertexs[faces[i*3+1]*3+2]);
        QVector3D c=QVector3D(vertexs[faces[i*3+2]*3+0],vertexs[faces[i*3+2]*3+1],vertexs[faces[i*3+2]*3+2]);

        QVector3D tn=QVector3D::crossProduct(a-b,b-c);
        normals[faces[i*3+0]*3+0]+=tn.x();
        normals[faces[i*3+0]*3+1]+=tn.y();
        normals[faces[i*3+0]*3+2]+=tn.z();
        normals[faces[i*3+1]*3+0]+=tn.x();
        normals[faces[i*3+1]*3+1]+=tn.y();
        normals[faces[i*3+1]*3+2]+=tn.z();
        normals[faces[i*3+2]*3+0]+=tn.x();
        normals[faces[i*3+2]*3+1]+=tn.y();
        normals[faces[i*3+2]*3+2]+=tn.z();
        normalCount[faces[i*3+0]]++;
        normalCount[faces[i*3+1]]++;
        normalCount[faces[i*3+2]]++;
    }

    for(int i=0;i<nVertex;i++){
        if(normalCount[i]!=0){
            normals[i*3+0]/=normalCount[i];
            normals[i*3+1]/=normalCount[i];
            normals[i*3+2]/=normalCount[i];
            QVector3D t=QVector3D(normals[i*3+0],normals[i*3+1],normals[i*3+2]).normalized();
            normals[i*3+0]=t.x();
            normals[i*3+1]=t.y();
            normals[i*3+2]=t.z();

        }
    }


}
