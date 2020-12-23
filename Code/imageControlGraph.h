#ifndef imageControlGraph_H
#define imageControlGraph_H

#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>



class Window;


class imageControlGraph :public QWidget
{
    Q_OBJECT

public:
    imageControlGraph(Window *p, QWidget *parent = 0);
    ~imageControlGraph();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    int group=0;

public slots:


signals:


protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;


private:

    Window *c_window;


    QPoint m_lastPos;
    QPoint lastPos;
    Qt::MouseButton button;


};


#endif
