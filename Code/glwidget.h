///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions>

#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QVector2D>
#include <QOpenGLVertexArrayObject>
#include <QVector>


class Window;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget,protected QOpenGLFunctions_4_3_Compatibility
{
    Q_OBJECT

public:
    GLWidget(int num, QString name, Window *p, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    bool fixedNegativeCurvature=false;
    float negativeCurvatureArea=-1;

    QPointF fixedPos=QPointF(0,0);
    int fixedVert=0;
    void getNegativeCurvatureArea(QPoint mousePos);
    void getNegativeCurvatureAreaLoad(QPoint mousePos,int pointedV);

    void getPixelNum();

public slots:

    void cleanup();

    void setCurvatureMin(float n);
    void setCurvatureMax(float n);
    void setCurvatureType(int t);
    void setNegativeCurvatureArea();
signals:
    void viewChange(int num);
    void synchronization();


protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    void Load3DFile(QString name);
    QVector3D cross_product(QVector3D v1,QVector3D v2);

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void getGeometriValues();

    void getFeatureValues();

    void getNormal();

    void saveRotatedMesh(QString basePath);
    void generateNeighborVertexList();

private:
    bool second_render=false;
    QPoint m_lastPos;

    QOpenGLShaderProgram *m_program;

    int m_xRot;
    int m_yRot;
    int m_zRot;

    QString p_fps;
    int fps;
    int fps_start;


    Window *c_window;


    int nVertex;
    int nFace;
    int nFace2;




    float *vertexs;
    float *normals;
    float *meanCurvature;
    float *gaussianCurvature;

    int *faces;

    QVector<int> *neighborVertices;
    QVector<int> *facesOfVertex;

    float *facesArea;


    float *vboData;
    float *vboData2;



    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;

    QOpenGLVertexArrayObject m_vao2;
    QOpenGLBuffer m_vbo2;

    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_mvMatrixLoc2;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_camera_o;
    QMatrix4x4 m_world;
    QMatrix4x4 m_world_scale_bar;


    int m_pmodeLoc;
    float scale;

    float curvMin;
    float curvMax;
    int m_curvMin;
    int m_curvMax;

    bool gaussianType;
    int m_gaussianType;

    int spineNum;


    QVector3D openPoint[2];
    QVector3D basePoint;
    QVector3D centroid;
    int stepNum;
    QVector3D *eachStep;

    float *eachStepMax;
    float *eachStepMin;
    int *eachCnt;

    float headMax;
    float headMin;
    float neckMax;
    float neckMin;
    int indexHead;
    int indexNeck;

    float L;

    QString fileName;

    QPoint mousePressedPos=QPoint(0,0);






};

#endif
