///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////


#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMatrix4x4>
#include <QVector2D>
#include <QSplitter>
#include "glwidget_for_data.h"

#include "glwidget.h"
#include "circularCoordinate.h"
#include "parallelCoordinate.h"
#include "tsneSelection.h"
#include "setFeatureUsing.h"
#include "seedSelectionWindow.h"
#include "colorBar.h"
#include "variationGraph.h"
#include "typeChangeGraph.h"
#include "imageControlGraph.h"
#include "tinytiffwriter.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QTabWidget>
#include <QSpinBox>
#include <QDockWidget>
#include <QTextEdit>
#include <QTime>
#include <QCheckBox>
#include <QLabel>
#include <QSpacerItem>
#include <QComboBox>
#include <QDebug>
#include <QTimer>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>
#include "DarkStyle.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QFileDialog>
#include <QSplitterHandle>
#include <QColorDialog>
#include <QProgressBar>
#include <QThread>

class MainWindow;


//#define WINDOW_SCALE qApp->desktop()->logicalDpiX()/96.0
class spineDetectionWorker : public QObject
{
    Q_OBJECT

public:
    spineDetectionWorker(Window *p){
        c_window=p;
    }
private:
    Window *c_window;

public slots:
    void run(QString query){
        system(query.toStdString().c_str());
    }
};


class DataGroup{
public:
    QLineEdit *name=NULL;
    QPushButton *add=NULL;
    QPushButton *remove=NULL;
    QPushButton *colorChange=NULL;
    QColor color;
    QListWidget *dataList=NULL;
    QWidget *widget=NULL;
    QHBoxLayout *layout1=NULL;
    QVBoxLayout *layout2=NULL;

    int groupIndex=0;
    float totalFeature[30];
    int numSpine=0;


    DataGroup(int a){
        QFont title_font("Arial", 9, QFont::Bold);
        name = new QLineEdit();
        name->setObjectName(QString::number(a));
        name->setText(QString("Group_")+QString::number(a));
        name->setFont(title_font);

        color = QColor((rand()*255)/RAND_MAX,(rand()*255)/RAND_MAX,(rand()*255)/RAND_MAX);

        colorChange = new QPushButton;
        colorChange->setObjectName(QString::number(a));
        colorChange->setFixedSize(QSize(15, 15));
        colorChange->setFlat(true);
        colorChange->setAutoFillBackground(true);
        colorChange->setStyleSheet(QString("background-color: %1;foreground-color: %1; border-style: none;").arg(color.name()));


        add=new QPushButton;
        add->setObjectName(QString::number(a));
        add->setIcon(QIcon("Resource/icon_add.png"));
        add->setIconSize(QSize(20, 20));
        add->setFixedSize(QSize(25, 25));
        QPalette pal = add->palette();
        pal.setColor(QPalette::Button, QColor(0,0,0,50));
        add->setPalette(pal);
        add->setFlat(true);
        //connect(&remove, SIGNAL(clicked()), this, SLOT(handleDeleteBtn()));

        remove=new QPushButton;
        remove->setObjectName(QString::number(a));
        remove->setIcon(QIcon("Resource/icon_trash.png"));
        remove->setIconSize(QSize(20, 20));
        remove->setFixedSize(QSize(25, 25));
        pal = remove->palette();
        pal.setColor(QPalette::Button, QColor(0,0,0,50));
        remove->setPalette(pal);
        remove->setFlat(true);

        layout1 = new QHBoxLayout;
        layout1->addWidget(name);
        layout1->addWidget(add);
        layout1->addWidget(colorChange);
        layout1->addWidget(remove);

        dataList=new QListWidget;
        dataList->setMouseTracking(true);
        dataList->setFocusPolicy(Qt::ClickFocus);
//        dataList->setAutoFillBackground(true);
        pal=dataList->palette();
        pal.setColor(QPalette::Base,QColor(50,50,50));
        dataList->setPalette(pal);
        //dataList->setStyle(new DarkStyle);



        layout2 = new QVBoxLayout;
        layout2->addLayout(layout1);
        layout2->addWidget(dataList);


        widget = new QWidget;
        widget->setLayout(layout2);
        //widget->setStyle(new DarkStyle);
        widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    }

    void setIndex(int index) {
        name->setObjectName(QString::number(index));
        colorChange->setObjectName(QString::number(index));
        add->setObjectName(QString::number(index));
        remove->setObjectName(QString::number(index));
    }
};




struct forSort1 {
  int index;
  float value;
  bool operator<(const forSort1 &other) const {
    return (value < other.value);
  }
};
struct forSort2 {
  int index;
  float value;
  bool operator<(const forSort2 &other) const {
    return (value > other.value);
  }
};


class Window : public QWidget
{
    Q_OBJECT

public:

    int total_pixel_num=0;
    int spine_pixel_num=0;

    Window(MainWindow *parent);
    ~Window();
    void loadFile(const QUrl &fileName);

    QThread spineDetectionThread;

    QComboBox *projection;

    float WINDOW_SCALE;

    QGroupBox *clusterGroup;
    QGroupBox *typeGroup;
    QGroupBox *coloringGroup;


    float maxX,maxY,maxZ,minX,minY,minZ;

    bool setType;
    bool setPrePost;
    bool setKD;
    bool setCTL;
    QString dataLocation;

    QVector<QString> spineNames;
    QVector<QColor> spineColors;
    int focusSpine;
    bool clicked;
    QVector<bool> spineEnable;
    int enabledSpineNum;
    int featureNum;

    bool typeEnable;
    bool testTrainEnable;
    QVector<QString> types;
    QVector<int> typesInt;
    QColor typeColors[10];

    QVector<int> TestTrain;
    QColor testTrainColors[3];


    QVector<QVector2D> selectionRange; //selection for each feature
    QVector<bool> selection; //if there is selection on the exis, selection[featureN]=true;
    QVector<QString> featureNames;
    QVector<QVector2D> featureRanges; // feature0 <min,max>: <featureRanges[0].x,featureRanges[0].y>

    QVector<int> spineGroups;
    QVector<bool> spineCorrect;

    QVector<QVector<float>> data; // spine1: data[1][0] ~ data[1][feature_number-1]

    QVector2D selectionAreaStart;
    QVector2D selectionAreaEnd;
    bool AreaSelection;

    QVector<QVector2D> tsneResult; //normalized result

    float centers[30][10];
    QColor clusterColors[30];

    QVector<int> clusterLabel;

    bool isFocus;

    int coloringType=0; //0: Data  1: Cluster  2: Type

    int numCluster;

    QCheckBox *clusterOnOff[30];

    QCheckBox *typeOnOff[10];

    QCheckBox *testTrainOnOff[3];

    void checkSpineEnable();

    QVector<bool> featureEnables;
    QVector<float> featureWeights;
    bool IsAutoControl;


    QString constrains[20];

    float minValues[30];
    float maxValues[30];
    float avgValues[30];


    QSpinBox *GroupNumSet;
    int currentSelectGroup;
    QList<int> selectedSpines[30];
    QVector<int> indexOfSelectedSpines;

    void addSpineToGroup(int a,int group);
    void deleteSpineFromGroup(int a,int group);


    bool seedChanged;


    bool IsShift;

    void changeTitle();

    FILE *newFeature;
    QHBoxLayout *clusterOnOffLayout;

    QCheckBox *selectionMode;
    QCheckBox *groupChangeMode;

    QCheckBox *spineEnableMode;
    QCheckBox *lookEnabled;

    QCheckBox *drawLine;

    QCheckBox *lookSelectedSpine;




    bool *displaySpines;



    QSlider *SetMin;
    QSlider *SetMax;


    colorBar *colorbar;

    QPushButton *SelectiveTsne;
    QPushButton *AlltSNE;
    QPushButton *AlltSNEForTrend;
    QPushButton *AllPCAForTrend;



    seedSelectionWindow *selectionWindow=NULL;


    bool enableRotation=true;


    void generateFlow();

    float minFlow[4][30];
    float maxFlow[4][30];
    float avgFlow[4][30];
    int nFlow[4];


    void arrangeRanderPart();

    QListWidget *groupList;
    QPushButton *groupAdd;
    QPushButton *groupDelete;
    QLabel *groupLabel;

    QHBoxLayout *groupLayout1;
    QVBoxLayout *groupLayout2;

    QSplitter *dataLayout1;

    GlWidgetForData* dataRendering;

    QGridLayout *dataMeshRenderPart;
    QVBoxLayout *dataMeshLayout;

    QSplitter *groupInfoLayout;
    QList<int> groupInfoSizes;

    QVBoxLayout *groupInfoLayout2;




    int curDataIndex=-1;
    QString curDataName="";
    int globalDataIndex=0;

    QVector<DataGroup*>groups;

    QComboBox *preGroup;
    QComboBox *postGroup;
    variationGraph *groupCompare;
    QVBoxLayout *groupCompareLayout;

    QGroupBox *groupCompareBox;

    QGridLayout *typeCompareLayout;
    QVector<typeChangeGraph *>typeCompare;

    QSlider *imageBrightControl;
    QSlider *imageContrastControl;
    QSlider *imageContrastPosControl;
    QSlider *imageLabelOpacityControl;
    QLabel *imageBrightLabel;
    QLabel *imageContrastLabel;
    QLabel *imageContrastPosLabel;
    QLabel *imageLabelOpacityLabel;

    imageControlGraph *tfRendering;

    QHBoxLayout *imageBrightLayout;
    QHBoxLayout *imageContrastLayout;
    QHBoxLayout *imageContrastPosLayout;
    QHBoxLayout *imageLabelOpacityLayout;
    QVBoxLayout *imageControlLayout1;
    QHBoxLayout *imageControlLayout1_1;

    QHBoxLayout *imageControlLayout2;
    QVBoxLayout *imageControlLayout2_1;

    QGroupBox *imageControlLayoutGroup;
    QGroupBox *imageLabelOpacityLayoutGroup;

    QVBoxLayout *imageControlWholeLayout;

    QHBoxLayout *Rt2;

    QVBoxLayout *onoffLayout;

    QGroupBox *datagroupGroup;
    QCheckBox *datagroupOnOff[100];
    QGridLayout *datagroupLayout;

    QPushButton *GenerateMorpData;
    QLineEdit *OutputPath;
    QLabel *OutputPathLabel;
    QGroupBox *DatasetWidget;
    QProgressBar *DatasetGenerateProgress;
    QLabel *ProgressLabel;


protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

signals:
    void runTSNE();
    void runSelectiveTSNE();
    void runSpineDetection(QString query);


public slots:
    void synchronization();
    void viewChange(int num);

    void selectDisplayMode(bool a);
    void displayShow(bool a);
    void selectReset();
    bool checkConstraint(int numConstrain,int spineNum);
    void featureUsageSetting();

    void handleGroupAdd();
    void handleGroupDelete();
    void addData();
    void addDataSub(QString dir,int index);


    void focusGroup();
    void focusGroup(QString a);
    void currentDataChange(QListWidgetItem* a);
    void setUIbasedOnCurDataIndex();
    void UIChanged(int a);
    void renderVarGraph(int a);
    void colorChangeGroup();
    void typeCompareLayoutUpdate();
    void typeCompareLayoutArrange();
    void groupNameChanged(QString a);

    void imageControlSliderChanged(int a);

    void generateDataset();
    void checkDatasetGeneration();
    void orderingFormatrixView1();
    void orderingFormatrixView2();
    void orderingFormatrixView3();
    void orderingFormatrixViewBySOM();

    void preOrdering();

private slots:
    void runCheckSpineEnable(bool a);
    void runClassification();


    void changeFocus(bool a);
    void kmeansClustering();
    void optimizedKmeansClustering();

    void changeColoringType(int a);
    void selectionForClustering();

    void saveResult();

public:

    bool arranging=false;
    float matrixD[1000][1000];
    float matrixN[1000][1000];
    int indexPos[1000][1000];
    int posX[1000];
    int posY[1000];
    void recursiveDivideX(int startX,int endX,int startY,int endY,QVector<int> indexList);
    void recursiveDivideY(int startX,int endX,int startY,int endY,QVector<int> indexList);

    int processedIndex=-1;
    QString processedPath="";
    QTimer *processedTimer;
    QListWidgetItem *processedItem=NULL;


//    QTabWidget *RenderPart;
    QGridLayout *RenderPart;
    QSplitter *Frame1;
    QHBoxLayout *Frame2;
    QVBoxLayout *ControlPart;
    QVBoxLayout *ControlPart2;
    QTabWidget *totalFrame;

    QSlider *SetMean;



    QSlider *createSlider(int a);

    QVector<GLWidget*> spines;

    CircularCoordinate *featurePlot;
    ParallelCoordinate *parallelPlot;
    tsneSelection *tsneGraph;

    int dataNum;

    int curSpine;

    QComboBox *curvatureType;
    bool dummy;
    QSpacerItem *dummyItem;

    QCheckBox *enableFocus;

    QLabel *GroupNumLabel;
    QPushButton *RunClustering1;
    QPushButton *RunClustering2;

    QHBoxLayout *clusterLayout;

    QComboBox *coloringTypeSet;

    QHBoxLayout *typeOnOffLayout;


    QHBoxLayout *TestTrainOnOffLayout;



    QPushButton *popupSetFeature;
    setFeatureUsing *setting=NULL;


    QPushButton *popupSelectionWindow;

    QPushButton *saveClusteringResult;

    QPushButton *displaySelectResult;


    MainWindow *mainWindow;

    int groupIndexOfDataList[1000];

    QVector2D nodePosition[30][30];


};

#endif
