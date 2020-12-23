#ifndef colorBar_H
#define colorBar_H

#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>
#include <QThread>



class Window;


class colorBar :public QWidget
{
    Q_OBJECT

public:
    colorBar(Window *p, QWidget *parent = 0);
    ~colorBar();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    float minV;
    float maxV;
public slots:

signals:
    void minValueChanged(float a);
    void maxValueChanged(float a);
    void changeDone();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
private:
    Window *c_window;


    bool ControlMin;
    bool ControlMax;
    QPoint m_lastPos;
};


#endif // colorBar_H
