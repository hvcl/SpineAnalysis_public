#ifndef TSNE_H
#define TSNE_H

#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>
#include <QThread>



class Window;


class TSNE_worker : public QObject
{
    Q_OBJECT

public:
    TSNE_worker(Window *p);
private:
    Window *c_window;

public slots:
    void run();
    void selective_run();
    void TrendPCArun();
    void TrendtSNErun();

signals:
    void resultReady();
    void selectiveResultReady();
    void resultTrendPCAReady();
    void resultTrendtSNEReady();

};





class tsneSelection :public QWidget
{
    Q_OBJECT

public:
    tsneSelection(Window *p, QWidget *parent = 0);
    ~tsneSelection();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    QVector<float> points;
    QVector<QVector3D> colors;

public slots:
    void changedResult();
    void changedSelectiveResult();
    void changedTrendPCAResult();
    void changedTrendtSNEResult();


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

    QThread workerThread;
    Window *c_window;


    QPoint m_lastPos;

    QPixmap background;
    QPoint lastPos;
    Qt::MouseButton button;


    QVector2D graphStart; //graph start point
    QVector2D graphEnd; //graph end point


    bool doMoving=false;
    QVector2D prevP;

    bool doSelect=false;
    float startSelectValue;

    QVector2D maxValue;
    QVector2D minValue;


    bool IsSelectGroupMode;
    int selectNum;
    uint clickTime;



};


#endif // TSNE_H
