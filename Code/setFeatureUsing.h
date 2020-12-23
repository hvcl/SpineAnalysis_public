#ifndef SETFEATURE_H
#define SETFEATURE_H

#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>


class Window;


class setFeatureUsing :public QWidget
{
    Q_OBJECT

public:
    setFeatureUsing(Window *p, QWidget *parent = 0);
    ~setFeatureUsing();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;


public slots:
    void setAutoManual();
    void setFeatureOnOff(bool a);
    void setFeatureWeight(int a);
    void releaseSlider();

    void runAddFeature();



signals:

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:

    Window *c_window;

    QVector<QCheckBox*> featureOnOff;
    QVector<QSlider*> featureWeight;
    QVector<QLabel*> curValue;

    QPushButton *autoManual;

    QPlainTextEdit *formula;
    QPushButton *addFeature;

    QHBoxLayout *features;

};


#endif // SETFEATURE_H
