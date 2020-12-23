#ifndef VARIATIONGRAPH_H
#define VARIATIONGRAPH_H

#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>



class Window;


class variationGraph :public QWidget
{
    Q_OBJECT

public:
    variationGraph(Window *p, QWidget *parent = 0);
    ~variationGraph();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    QVector<float> points;
    QVector<QVector3D> colors;

public slots:


signals:
    void viewChange(int num);
    void synchronization();


protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;


    void readAnalysisFile();

private:

    Window *c_window;


    QPoint m_lastPos;

    QPixmap background;
    QPoint lastPos;
    Qt::MouseButton button;

    float variation[30];
    float preTotal[30];
    float postTotal[30];

    float maxV;
    float minV;




};


#endif // VARIATIONGRAPH_H
