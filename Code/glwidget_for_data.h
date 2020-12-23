///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#ifndef GlWidgetForData_H
#define GlWidgetForData_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QVector2D>
#include <QOpenGLShaderProgram>
#include "tinytiffreader.h"
#include <QVector>
class dataset{
public:

    QString basePath;


    GLuint tex_3D;
    GLuint tex_label;

    int w,h,d;
    int type=16;

    unsigned char *data8;
    unsigned short *data16;

    unsigned char *label_data;


    QVector<int> spineIndex;
    QVector<int> labelIndex;
    QVector<QColor> spineColors;

    int dataIndex;

};




class uchar4{
public:
    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned char w;
};

class int3{
public:
    int x;
    int y;
    int z;
};

class Window;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GlWidgetForData : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Compatibility
{
    Q_OBJECT

public:
    GlWidgetForData(Window *p, QWidget *parent = 0);
    ~GlWidgetForData();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QVector4D background_color;
    std::string filename;
    QVector4D color_table[512];

    void loadTifFile(QString basePath, int groupIndex);
    void changeDataset(QString basePath,int groupIndex,int dataIndex);
    int getObjectCoordfromMousePoint(QVector2D mousePoint); //mouse point: window coordinate

public slots:

    void interpolation_type_change(int a);
    void cleanup();

    void cut_plane(int r);

    void set_z_scale(int n);


    void ambi_change(int a);
    void diff_change(int a);
    void spec_change(int a);
    void shin_change(int a);
    void l_t_change(int a);


    void change_R1(int a);
    void change_R2(int a);
    void change_cut_d(int a);
    void change_cut_enable(bool a);
    void change_cut_alpha(int a);

    void change_phong_enable(bool a);
    void change_sampling_rate(int a);
    void cropCurrentData(QPoint pos);

signals:

    void backgroundColorChanged();
    void label_changed(int minx,int miny,int minz,int tw,int th,int td,unsigned char *tdata);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    QVector3D cross_product(QVector3D v1,QVector3D v2);
    void ReadColorTable();
    void ReadLabelColor();

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;


public:
    QPoint m_lastPos;

    QOpenGLShaderProgram *m_program;

    QVector3D box_min;
    QVector3D box_max;


    int con_window_size;


    int con_eye_pos;
    int con_up;
    int con_center;

    QString p_fps;
    int fps;
    int fps_start;




    int con_color_table;

    GLuint color_tex;

    int con_label_color;
    GLuint label_color_tex;




    int con_sample;
    float sample;


    int con_l_t;

    float z_scale;

    int con_background_color;

    Window *c_window;

    int con_specu,con_diffu,con_ambi,con_shin;


    int tog;
    float l_t;

//    uchar4 *G;
//    int3 *t_G;


    float ambi,diffu,specu,shin;

    float EYE;

    int phong_enable;
    int con_phong_enable;

    float R1;
    float R2;
    float cut_d;

    int con_v_size;

    float sampling_rate;
    int con_sampling_rate;

    int con_roi_center;
    int con_roi_size;
    float prev_depth=-1;
    QVector3D prev_roi_center;

    QVector3D center1,eye_position,up,shift;
    float scale;
    QVector3D roi_center=QVector3D(0,0,0);
    float roi_size=4;

    int con_tex_label;
    int con_tex;
    int con_box_min;
    int con_box_max;

    QVector<dataset*> dataList;

    QVector3D const_box_max;
    QVector3D const_box_min;

    int prevDataIndex=-1;
    QPoint mousePressedPos=QPoint(0,0);

    float sliceSliderPos=0.5;
    bool sliderPressed=false;
    QPointF cropStart;
    QPointF cropEnd;
    bool croping=false;
    bool cropWait=false;

};

#endif
