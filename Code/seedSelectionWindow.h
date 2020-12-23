#ifndef seedSelectionWindow_H
#define seedSelectionWindow_H

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
#include <QSpinBox>
#include <QListWidget>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QComboBox>

class Window;


class seedSelectionWindow :public QWidget
{
    Q_OBJECT

public:
    seedSelectionWindow(Window *p, QWidget *parent = 0);
    ~seedSelectionWindow();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    int groupN;
    QComboBox *curSelectGroup;

    QHBoxLayout *listLayout;

    QPushButton *down[30];
    QPushButton *up[30];


    QPlainTextEdit *tEdit[20];
    void doMakeStandard(int g);

public slots:

    void chagnedGroupNum(int a);

    void pushAddAllButton();
    void pushDeleteButton();

    void focusItem(QListWidgetItem* a);
    void clickItem(QListWidgetItem* a);

    void changeSelectionGroup(int a);
    void constrainChanged();

    void makeSelection();
    void makeStandard();

signals:

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:

    Window *c_window;
    QSpinBox *groupNum;
    QPushButton *addAllSelectedSpines;
    QListWidget *spineList[30];
    QGroupBox *groupBox[30];
    QPushButton *deleteSpine[30];
    QPoint mousePos;



};


#endif // seedSelectionWindow_H
