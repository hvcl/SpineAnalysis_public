#ifndef PARALLELCOORDINATE_H
#define PARALLELCOORDINATE_H

#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>



class Window;


class ParallelCoordinate :public QWidget
{
    Q_OBJECT

public:
    ParallelCoordinate(Window *p, QWidget *parent = 0);
    ~ParallelCoordinate();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    QVector<float> points;
    QVector<QVector3D> colors;

    float max_value;
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
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;


    void readAnalysisFile();

private:

    Window *c_window;
    int prePost; //0: false /1: pre /2: post


    QPoint m_lastPos;

    QPixmap background;
    QPoint lastPos;
    Qt::MouseButton button;


    QVector2D *axisStarts; //each axis point
    QVector2D *axisEnds; //each axis point


    int selectionFrame;
    bool doSelect;
    float startSelectValue;

};


#endif // PARALLELCOORDINATE_H
