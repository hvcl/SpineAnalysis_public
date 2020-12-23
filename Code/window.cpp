///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "window.h"
#include "mainwindow.h"
#include "Eigen/Eigen"

Window::Window(MainWindow *parent)
{

//    for(int i=0;i<10;i++){
//        for(int j=0;j<10;j++){
//            nodePosition[i][j].setX(0.1*j);
//            nodePosition[i][j].setY(0.1*i);
//        }
//    }

    qsrand(1);
    mainWindow=parent;

    WINDOW_SCALE=qApp->desktop()->logicalDpiX()/96.0;

    maxX=-1000,maxY=-1000,maxZ=-1000,minX=1000,minY=1000,minZ=1000;



    setType=false;
    setPrePost=false;
    setKD=false;
    setCTL=false;
    //dataLocation="dataset_final/";
    //dataLocation="2_1024_287_256_8/spine/";
    SelectiveTsne=new QPushButton("t-SNE of Selected Spines");
    AlltSNE=new QPushButton("Projection");
    AlltSNE->setStyle(new DarkStyle);

    AlltSNEForTrend=new QPushButton("t-SNE for Trend");
    AllPCAForTrend=new QPushButton("PCA for Trend");

    seedChanged=false;
    IsShift=false;
    numCluster=4;
    for(int i=0;i<30;i++){
        clusterColors[i]=QColor(qrand()%206+50,qrand()%206+50,qrand()%206+50);
    }
    QColor tcolor=clusterColors[3];
    clusterColors[2]=QColor(50,141,100);
    clusterColors[3]=clusterColors[5];
    clusterColors[5]=tcolor;

    for(int i=0;i<10;i++){
        typeColors[i]=QColor(qrand()%206+50,qrand()%206+50,qrand()%206+50);
    }
    for(int i=0;i<3;i++){
        testTrainColors[i]=QColor(qrand()%206+50,qrand()%206+50,qrand()%206+50);
    }

    for(int i=0;i<20;i++){
        constrains[i]=QString("");
    }

    isFocus=true;
    AreaSelection=false;

    dummy=false;
    dummyItem=new QSpacerItem(100,100);

    curSpine=-1;

    featurePlot=new CircularCoordinate(this);

    connect(featurePlot,SIGNAL(viewChange(int)),this,SLOT(viewChange(int)));

    parallelPlot=new ParallelCoordinate(this);
    connect(parallelPlot,SIGNAL(viewChange(int)),this,SLOT(viewChange(int)));

    tsneGraph=new tsneSelection(this);
    connect(tsneGraph,SIGNAL(viewChange(int)),this,SLOT(viewChange(int)));


    connect(parallelPlot,SIGNAL(synchronization()),featurePlot,SLOT(update()));
    connect(parallelPlot,SIGNAL(synchronization()),tsneGraph,SLOT(update()));

    connect(featurePlot,SIGNAL(synchronization()),parallelPlot,SLOT(update()));
    connect(featurePlot,SIGNAL(synchronization()),tsneGraph,SLOT(update()));

    connect(tsneGraph,SIGNAL(synchronization()),parallelPlot,SLOT(update()));
    connect(tsneGraph,SIGNAL(synchronization()),featurePlot,SLOT(update()));


    groupChangeMode=new QCheckBox("Manual Group Assign Mode");
    groupChangeMode->setChecked(false);


    spineEnableMode=new QCheckBox("Select Spines for display");
    spineEnableMode->setChecked(false);


    lookEnabled=new QCheckBox("Show");
    lookEnabled->setChecked(false);



    IsAutoControl=false;
    for(int i=0;i<featureNum;i++){
        featureEnables.push_back(true);
        featureWeights.push_back(1.0);
    }


    currentSelectGroup=0;


    RenderPart=new QGridLayout;



    curvatureType=new QComboBox();
    curvatureType->addItem("Mean Curvature");
    curvatureType->addItem("Gaussian Curvature");
    curvatureType->setStyle(new DarkStyle);

    enableFocus=new QCheckBox("On/off Focus");
    enableFocus->setChecked(true);

    connect(enableFocus,SIGNAL(clicked(bool)),this,SLOT(changeFocus(bool)));

    SetMean=createSlider(100);



    colorbar=new colorBar(this);
    colorbar->setFixedHeight(20*WINDOW_SCALE);


//    for(int i=0;i<spines.length();i++){
//        connect(colorbar,SIGNAL(minValueChanged(float)),spines[i],SLOT(setCurvatureMin(float)));
//        connect(colorbar,SIGNAL(maxValueChanged(float)),spines[i],SLOT(setCurvatureMax(float)));
//        connect(curvatureType,SIGNAL(currentIndexChanged(int)),spines[i],SLOT(setCurvatureType(int)));
//        connect(spines[i],SIGNAL(viewChange(int)),this,SLOT(viewChange(int)));
//        connect(spines[i],SIGNAL(synchronization()),this,SLOT(synchronization()));

//    }
    popupSetFeature=new QPushButton("Feature Manager");
    connect(popupSetFeature,SIGNAL(pressed()),this,SLOT(featureUsageSetting()));
    popupSetFeature->setStyle(new DarkStyle);
    popupSetFeature->setStyleSheet(QString("QPushButton {font: bold;}"));





    ControlPart=new QVBoxLayout;
    ControlPart->addWidget(featurePlot);
    ControlPart->addWidget(parallelPlot);
//    ControlPart->addWidget(popupSetFeature);
//    ControlPart->addWidget(curvatureType);
//    ControlPart->addWidget(colorbar);

    //ControlPart->addWidget(enableFocus);



    GroupNumLabel=new QLabel("Group Number: ");
    GroupNumSet=new QSpinBox;
    GroupNumSet->setMinimum(2);
    GroupNumSet->setMaximum(30);
    GroupNumSet->setValue(4);
    RunClustering1=new QPushButton("Clustering");
    connect(RunClustering1,SIGNAL(pressed()),this,SLOT(kmeansClustering()));

    RunClustering2=new QPushButton("Classification");
    connect(RunClustering2,SIGNAL(pressed()),this,SLOT(runClassification()));
    RunClustering2->setStyle(new DarkStyle);
    RunClustering2->setStyleSheet(QString("QPushButton {font: bold;}"));


    clusterLayout=new QHBoxLayout;
//    clusterLayout->addWidget(GroupNumLabel,2);
//    clusterLayout->addWidget(GroupNumSet,1);
//    clusterLayout->addWidget(RunClustering1,2);
    clusterLayout->addWidget(RunClustering2,2);

    char ct[50];
    for(int i=0;i<30;i++){
        sprintf(ct,"Group %d",i);
        clusterOnOff[i]=new QCheckBox(tr(ct));
        sprintf(ct,"QCheckBox {font: bold; color : rgb(%d,%d,%d);}",clusterColors[i].red(),clusterColors[i].green(),clusterColors[i].blue());
        clusterOnOff[i]->setStyleSheet(QString(ct));
        clusterOnOff[i]->setChecked(true);
        connect(clusterOnOff[i],SIGNAL(clicked(bool)),this,SLOT(runCheckSpineEnable(bool)));

    }

    clusterOnOff[0]->setText("Irregular");
    clusterOnOff[1]->setText("Mushroom");
    clusterOnOff[2]->setText("Stubby");
    clusterOnOff[3]->setText("Thin");

    clusterOnOffLayout=new QHBoxLayout;
    for(int i=0;i<4;i++){
        clusterOnOffLayout->addWidget(clusterOnOff[i]);
    }


    TestTrainOnOffLayout=new QHBoxLayout;

    testTrainOnOff[0]=new QCheckBox("Test");
    testTrainOnOff[1]=new QCheckBox("Train");
    for(int i=0;i<2;i++){
        sprintf(ct,"QCheckBox {font: bold; color : rgb(%d,%d,%d);}",testTrainColors[i].red(),typeColors[i].green(),typeColors[i].blue());
        testTrainOnOff[i]->setStyleSheet(QString(ct));
        testTrainOnOff[i]->setChecked(true);
        connect(testTrainOnOff[i],SIGNAL(clicked(bool)),this,SLOT(runCheckSpineEnable(bool)));
        TestTrainOnOffLayout->addWidget(testTrainOnOff[i]);
    }


    typeOnOffLayout=new QHBoxLayout;


    typeOnOff[0]=new QCheckBox("None");
    typeOnOff[1]=new QCheckBox("Mushroom");
    typeOnOff[2]=new QCheckBox("Thin");
    typeOnOff[3]=new QCheckBox("Stubby");
    typeOnOff[4]=new QCheckBox("Filopodia");

    for(int i=0;i<5;i++){
        sprintf(ct,"QCheckBox {font: bold; color : rgb(%d,%d,%d);}",typeColors[i].red(),typeColors[i].green(),typeColors[i].blue());
        typeOnOff[i]->setStyleSheet(QString(ct));
        typeOnOff[i]->setChecked(true);
        connect(typeOnOff[i],SIGNAL(clicked(bool)),this,SLOT(runCheckSpineEnable(bool)));
        typeOnOffLayout->addWidget(typeOnOff[i]);
    }



    coloringTypeSet=new QComboBox;
    coloringTypeSet->addItem("Assign based on Data");
    coloringTypeSet->addItem("Assign based on Phenotype");
    coloringTypeSet->addItem("Assign based on Selection");
    coloringTypeSet->addItem("Assign based on Pre-defined Type");
    coloringTypeSet->addItem("Assign based on Data Group");

    coloringTypeSet->setStyle(new DarkStyle);
    coloringTypeSet->setStyleSheet(QString("QComboBox {font: bold;}"));


    //coloringTypeSet->addItem("Assign based on Type");
    connect(coloringTypeSet,SIGNAL(currentIndexChanged(int)),this,SLOT(changeColoringType(int)));




    popupSelectionWindow=new QPushButton("Selection Manager");
    connect(popupSelectionWindow,SIGNAL(pressed()),this,SLOT(selectionForClustering()));


    saveClusteringResult=new QPushButton("Save Clustering Result");
    connect(saveClusteringResult,SIGNAL(pressed()),this,SLOT(saveResult()));


    displaySelectResult=new QPushButton("Reset Selection");
    connect(displaySelectResult,SIGNAL(pressed()),this,SLOT(selectReset()));

    QHBoxLayout *tL=new QHBoxLayout;
    tL->addWidget(groupChangeMode);
    tL->addWidget(saveClusteringResult);
    QHBoxLayout *tL2=new QHBoxLayout;
    tL2->addWidget(spineEnableMode);
    tL2->addWidget(lookEnabled);
    tL2->addWidget(displaySelectResult);

    connect(spineEnableMode,SIGNAL(clicked(bool)),this,SLOT(selectDisplayMode(bool)));
    connect(lookEnabled,SIGNAL(clicked(bool)),this,SLOT(displayShow(bool)));

    displaySpines=new bool[data.length()];
    for(int i=0;i<data.length();i++)displaySpines[i]=false;


    drawLine=new QCheckBox("Line Rendering");

    drawLine->setChecked(true);


    clusterGroup=new QGroupBox("Classification Panel");
    clusterGroup->setStyleSheet(QString("QGroupBox {font: bold; color : rgb(150,150,150);}"));

    selectionWindow=new seedSelectionWindow(this);
    seedChanged=true;
    QTimer *ttimer = new QTimer(this);
    connect(ttimer, SIGNAL(timeout()), selectionWindow, SLOT(update()));
    ttimer->start(100);

    connect(GroupNumSet,SIGNAL(valueChanged(int)),selectionWindow,SLOT(chagnedGroupNum(int)));




    QVBoxLayout *clusterGroupLayout=new QVBoxLayout;
    //clusterGroupLayout->addLayout(tL);
    clusterGroupLayout->addLayout(clusterOnOffLayout);
    clusterGroupLayout->addLayout(clusterLayout);
    clusterGroupLayout->addWidget(selectionWindow);
    clusterGroup->setLayout(clusterGroupLayout);


    typeGroup=new QGroupBox("Pre-defined Types");

    typeGroup->setLayout(typeOnOffLayout);
    typeGroup->setStyleSheet(QString("QGroupBox {font: bold; color : rgb(150,150,150);} "));


    coloringGroup=new QGroupBox("Representative Color Setting");
    coloringGroup->setStyleSheet(QString("QGroupBox {font: bold; color : rgb(150,150,150);}"));
    coloringGroup->setFixedHeight(60*WINDOW_SCALE);

    QHBoxLayout *tColoringLayout=new QHBoxLayout;
    tColoringLayout->addWidget(coloringTypeSet);
    coloringGroup->setLayout(tColoringLayout);

//    QHBoxLayout *Options=new QHBoxLayout;
//    Options->addWidget(lookSelectedSpine);
//    Options->addWidget(drawLine);
//    Options->addWidget(enableFocus);


    projection=new QComboBox;
    projection->addItem("PCA");
    projection->addItem("tSNE");
    projection->addItem("UMAP");
    projection->setCurrentIndex(1);
    projection->setStyle(new DarkStyle);
    projection->setStyleSheet(QString("QComboBox {font: bold;}"));

    AlltSNE->setStyleSheet(QString("QPushButton {font: bold;}"));



    QHBoxLayout *ttSNE=new QHBoxLayout;
    ttSNE->addWidget(projection);
    ttSNE->addWidget(AlltSNE);

    //ttSNE->addWidget(SelectiveTsne);

    QHBoxLayout *trend=new QHBoxLayout;

    trend->addWidget(AlltSNEForTrend);
    trend->addWidget(AllPCAForTrend);


    QVBoxLayout *sub1ControlPart2=new QVBoxLayout;
    QVBoxLayout *sub2ControlPart2=new QVBoxLayout;

    sub1ControlPart2->addLayout(ttSNE);
    sub1ControlPart2->addWidget(tsneGraph);

    //sub2ControlPart2->addLayout(Options);
//    ControlPart2->addWidget(lookSelectedSpine,1);

//    ControlPart2->addWidget(drawLine,1);
//    ControlPart2->addWidget(enableFocus,1);

    //sub2ControlPart2->addSpacing(10);
    sub2ControlPart2->addWidget(clusterGroup);
    sub2ControlPart2->addSpacing(10);

//    ControlPart2->addLayout(tL);
////    ControlPart2->addLayout(tL2);
//    ControlPart2->addLayout(clusterLayout,1);
//    ControlPart2->addLayout(clusterOnOffLayout,1);
////    ControlPart2->addLayout(TestTrainOnOffLayout,1);

//////    ControlPart2->addWidget(typeGroup);
    //////ControlPart2->addSpacing(10);

//    ControlPart2->addLayout(typeOnOffLayout,1);

//////    sub2ControlPart2->addWidget(coloringGroup,1);
//    ControlPart2->addWidget(coloringTypeSet,1);
//////    sub2ControlPart2->addSpacing(20);
//////    sub2ControlPart2->addWidget(popupSetFeature,1);
//    ControlPart2->addWidget(popupSelectionWindow,1);


    datagroupLayout=new QGridLayout;
    datagroupGroup=new QGroupBox("Data Group");
    datagroupGroup->setStyleSheet(QString("QGroupBox {font: bold; color : rgb(150,150,150);} "));
    datagroupGroup->setLayout(datagroupLayout);

    onoffLayout=new QVBoxLayout;
    onoffLayout->addWidget(datagroupGroup);
    onoffLayout->addWidget(typeGroup);

    ControlPart2=new QVBoxLayout;
    ControlPart2->addLayout(sub1ControlPart2,1);
    ControlPart2->addLayout(onoffLayout);
    ControlPart2->addLayout(sub2ControlPart2,1);







    QVBoxLayout *Rt=new QVBoxLayout;
    Rt->addWidget(colorbar);
    Rt->addWidget(curvatureType);

    Rt2=new QHBoxLayout;
    Rt2->addLayout(Rt);
    Rt2->addWidget(coloringGroup);

    QVBoxLayout *RenderFrame=new QVBoxLayout;
    RenderFrame->addLayout(RenderPart);
    RenderFrame->addLayout(Rt2);


    Frame2=new QHBoxLayout;
    Frame2->addLayout(RenderFrame,2);
    Frame2->addLayout(ControlPart,1);
    Frame2->addLayout(ControlPart2,1);

    QWidget *tFrame2Widget=new QWidget;
    tFrame2Widget->setLayout(Frame2);



    groupLabel=new QLabel("Group List");
    QFont tff;
    tff.setBold(true);
    groupLabel->setFont(tff);

    groupAdd=new QPushButton;
    groupAdd->setIcon(QIcon("Resource/icon_add_black.png"));
    groupAdd->setIconSize(QSize(20, 20));
    groupAdd->setFixedSize(QSize(25, 25));
    QPalette pal = groupAdd->palette();
    pal.setColor(QPalette::Button, QColor(0,0,0,50));
    groupAdd->setPalette(pal);
    groupAdd->setFlat(true);
    connect(groupAdd, SIGNAL(clicked()), this, SLOT(handleGroupAdd()));


    groupList=new QListWidget;
    QPalette pal4 = groupList->palette();
    pal4.setColor(QPalette::Base, QColor(0,0,0,150));
    groupList->setPalette(pal4);
    groupList->setAutoFillBackground(true);

    groupLayout1=new QHBoxLayout;
    groupLayout1->addWidget(groupLabel);
    groupLayout1->addWidget(groupAdd);

    groupLayout2=new QVBoxLayout;
    groupLayout2->addLayout(groupLayout1);
    groupLayout2->addWidget(groupList);


    QWidget *groupLayout2Widget=new QWidget;
    groupLayout2Widget->setLayout(groupLayout2);

    dataRendering=new GlWidgetForData(this);


    imageBrightLabel=new QLabel("Bright");
    imageBrightLabel->setStyleSheet(QString("QLabel {font: Bold;}"));
    imageBrightLabel->setFixedWidth(60);
    imageContrastLabel=new QLabel("Contrast");
    imageContrastLabel->setStyleSheet(QString("QLabel {font: Bold;}"));
    imageContrastLabel->setFixedWidth(60);
    imageContrastPosLabel=new QLabel("MidPos");
    imageContrastPosLabel->setStyleSheet(QString("QLabel {font: Bold;}"));
    imageContrastPosLabel->setFixedWidth(60);
    imageLabelOpacityLabel=new QLabel("Spine\nOpacity");
    imageLabelOpacityLabel->setStyleSheet(QString("QLabel {font: Bold;}"));
    imageLabelOpacityLabel->setFixedWidth(60);


    imageBrightControl=new QSlider(Qt::Horizontal);
    imageBrightControl->setRange(0, 255);
    imageBrightControl->setSingleStep(1);
    imageBrightControl->setTickInterval(25);
    imageBrightControl->setTickPosition(QSlider::TicksAbove);
    imageBrightControl->setValue(128);
    imageBrightControl->setStyle(new DarkStyle);

    imageContrastControl=new QSlider(Qt::Horizontal);
    imageContrastControl->setRange(1, 255);
    imageContrastControl->setSingleStep(1);
    imageContrastControl->setTickInterval(25);
    imageContrastControl->setTickPosition(QSlider::TicksAbove);
    imageContrastControl->setValue(128);
    imageContrastControl->setStyle(new DarkStyle);

    imageContrastPosControl=new QSlider(Qt::Horizontal);
    imageContrastPosControl->setRange(0, 255);
    imageContrastPosControl->setSingleStep(1);
    imageContrastPosControl->setTickInterval(25);
    imageContrastPosControl->setTickPosition(QSlider::TicksAbove);
    imageContrastPosControl->setValue(128);
    imageContrastPosControl->setStyle(new DarkStyle);


    imageLabelOpacityControl=new QSlider(Qt::Horizontal);
    imageLabelOpacityControl->setRange(0, 255);
    imageLabelOpacityControl->setSingleStep(1);
    imageLabelOpacityControl->setTickInterval(25);
    imageLabelOpacityControl->setTickPosition(QSlider::TicksAbove);
    imageLabelOpacityControl->setValue(128);
    imageLabelOpacityControl->setStyle(new DarkStyle);


    connect(imageBrightControl,SIGNAL(valueChanged(int)),this,SLOT(imageControlSliderChanged(int)));
    connect(imageContrastControl,SIGNAL(valueChanged(int)),this,SLOT(imageControlSliderChanged(int)));
    connect(imageContrastPosControl,SIGNAL(valueChanged(int)),this,SLOT(imageControlSliderChanged(int)));
    connect(imageLabelOpacityControl,SIGNAL(valueChanged(int)),this,SLOT(imageControlSliderChanged(int)));


    imageBrightLayout=new QHBoxLayout;
    imageBrightLayout->addWidget(imageBrightLabel);
    imageBrightLayout->addWidget(imageBrightControl);

    imageContrastLayout=new QHBoxLayout;
    imageContrastLayout->addWidget(imageContrastLabel);
    imageContrastLayout->addWidget(imageContrastControl);

    imageContrastPosLayout=new QHBoxLayout;
    imageContrastPosLayout->addWidget(imageContrastPosLabel);
    imageContrastPosLayout->addWidget(imageContrastPosControl);

    imageLabelOpacityLayout=new QHBoxLayout;
//    imageLabelOpacityLayout->addWidget(imageLabelOpacityLabel);
    imageLabelOpacityLayout->addWidget(imageLabelOpacityControl);

    imageLabelOpacityLayoutGroup=new QGroupBox("Label Opacity");
    imageLabelOpacityLayoutGroup->setStyleSheet(QString("QGroupBox {font: bold; color : rgb(150,150,150);} "));
    imageLabelOpacityLayoutGroup->setLayout(imageLabelOpacityLayout);

    imageControlLayout1=new QVBoxLayout;
    imageControlLayout1->addLayout(imageBrightLayout);
    imageControlLayout1->addLayout(imageContrastLayout);
    imageControlLayout1->addLayout(imageContrastPosLayout);
//    imageControlLayout1->addLayout(imageLabelOpacityLayout);

    tfRendering=new imageControlGraph(this);

    imageControlLayout1_1=new QHBoxLayout;
    imageControlLayout1_1->addLayout(imageControlLayout1,2);
    imageControlLayout1_1->addWidget(tfRendering,1);

    imageControlLayoutGroup=new QGroupBox("Image Adjust");
    imageControlLayoutGroup->setStyleSheet(QString("QGroupBox {font: bold; color : rgb(150,150,150);} "));
    imageControlLayoutGroup->setLayout(imageControlLayout1_1);


    imageControlLayout2_1=new QVBoxLayout;
    imageControlLayout2_1->addWidget(imageLabelOpacityLayoutGroup);
    imageControlLayout2_1->addWidget(coloringGroup);


    imageControlLayout2=new QHBoxLayout;
    imageControlLayout2->addWidget(imageControlLayoutGroup);
    imageControlLayout2->addLayout(imageControlLayout2_1);




    imageControlWholeLayout=new QVBoxLayout;
    imageControlWholeLayout->addWidget(dataRendering);
    imageControlWholeLayout->addLayout(imageControlLayout2);


    dataMeshRenderPart=new QGridLayout;


    //n=spineNames.length();
    //gridSize=sqrt(n*2)+0.9999;

    //for(int i=0;i<n;i++){
    //    dataMeshRenderPart->addWidget(spines[i],i/gridSize,i%gridSize);
    //}


    spineDetectionWorker *sdworker=new spineDetectionWorker(this);
    sdworker->moveToThread(&spineDetectionThread);
    connect(&spineDetectionThread, &QThread::finished, sdworker, &QObject::deleteLater);
    connect(this,SIGNAL(runSpineDetection(QString)),sdworker,SLOT(run(QString)));
    spineDetectionThread.start();


    GenerateMorpData=new QPushButton("Generate");
    GenerateMorpData->setStyle(new DarkStyle);
    GenerateMorpData->setStyleSheet(QString("QPushButton {font: bold; color: rgb(255,100,100);}"));
    connect(GenerateMorpData,SIGNAL(pressed()),this,SLOT(generateDataset()));
    OutputPath=new QLineEdit();
    OutputPath->setStyleSheet(QString("QLineEdit {font: bold;}"));

    OutputPathLabel=new QLabel("Dataset Name: ");
    OutputPathLabel->setStyleSheet(QString("OutputPathLabel {font: bold;}"));

    DatasetGenerateProgress=new QProgressBar;
    DatasetGenerateProgress->setValue(0);
    DatasetGenerateProgress->setStyle(new DarkStyle);
    ProgressLabel=new QLabel("");
    ProgressLabel->setStyleSheet(QString("QLabel {font: bold; color: rgb(100,100,255);}"));


    QHBoxLayout *DatasetLayout=new QHBoxLayout;
    DatasetLayout->addWidget(OutputPathLabel);
    DatasetLayout->addWidget(OutputPath);
    DatasetLayout->addWidget(GenerateMorpData);
    QVBoxLayout *DatasetLayout2=new QVBoxLayout;
    DatasetLayout2->addWidget(ProgressLabel);
    DatasetLayout2->addWidget(DatasetGenerateProgress);
    DatasetLayout2->addLayout(DatasetLayout);

    DatasetWidget=new QGroupBox("Generate 3D Morphological Data");
    DatasetWidget->setStyleSheet(QString("QGroupBox {font: bold; color : rgb(150,150,150);} "));

    DatasetWidget->setLayout(DatasetLayout2);
    DatasetWidget->hide();


    dataMeshLayout=new QVBoxLayout;
    dataMeshLayout->addWidget(DatasetWidget);
    dataMeshLayout->addLayout(dataMeshRenderPart,1);
    dataMeshLayout->addWidget(parallelPlot,1);



    QWidget *dataMeshLayoutWidget=new QWidget;
    dataMeshLayoutWidget->setLayout(dataMeshLayout);


    QWidget *imageControlWholeLayoutWidget=new QWidget;
    imageControlWholeLayoutWidget->setLayout(imageControlWholeLayout);

    dataLayout1=new QSplitter(Qt::Horizontal);
    dataLayout1->addWidget(groupLayout2Widget);
    dataLayout1->addWidget(imageControlWholeLayoutWidget);
    dataLayout1->addWidget(dataMeshLayoutWidget);

    dataLayout1->setChildrenCollapsible(false);
    QList<int> sizes;
    sizes<<100<<this->width()-350<<250;
    dataLayout1->setSizes(sizes);
    dataLayout1->setHandleWidth(15);
    dataLayout1->setStyleSheet("QSplitter::handle {image: url(Resource/icon_handle.png);}");


    typeCompareLayout=new QGridLayout;

    QWidget *typeCompareLayoutWidget=new QWidget;
    typeCompareLayoutWidget->setLayout(typeCompareLayout);

    preGroup=new QComboBox;
    preGroup->addItem("Pre: ");
    connect(preGroup,SIGNAL(currentIndexChanged(int)),this,SLOT(renderVarGraph(int)));

    postGroup=new QComboBox;
    postGroup->addItem("Post: ");
    connect(postGroup,SIGNAL(currentIndexChanged(int)),this,SLOT(renderVarGraph(int)));

    QHBoxLayout *prepostLayout=new QHBoxLayout;
    prepostLayout->addWidget(preGroup);
    prepostLayout->addWidget(postGroup);

    groupCompare=new variationGraph(this);

    groupCompareLayout=new QVBoxLayout;
    groupCompareLayout->addLayout(prepostLayout);
    groupCompareLayout->addWidget(groupCompare);

    groupCompareBox=new QGroupBox("Morphology difference between two groups"); //plasticity analysis
    groupCompareBox->setStyleSheet(QString("QGroupBox {font: bold; color : rgb(150,150,150);}"));
    groupCompareBox->setLayout(groupCompareLayout);

    QFont tttt;
    tttt.setBold(true);
    preGroup->setStyle(new DarkStyle);
    postGroup->setStyle(new DarkStyle);
    preGroup->setFont(tttt);
    postGroup->setFont(tttt);


    groupInfoLayout=new QSplitter(Qt::Horizontal);
    groupInfoLayout->addWidget(typeCompareLayoutWidget);
    groupInfoLayout->addWidget(featurePlot);
    groupInfoLayout->addWidget(groupCompareBox);
    groupInfoLayout->setChildrenCollapsible(false);
    sizes.clear();
    sizes<<this->width()/3<<this->width()/3<<this->width()/3;
    groupInfoLayout->setSizes(sizes);
    groupInfoLayout->setHandleWidth(15);
    groupInfoLayout->setStyleSheet("QSplitter::handle {image: url(Resource/icon_handle.png);}");


    QWidget *lineA = new QWidget;
    lineA->setFixedHeight(3);
    lineA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineA->setStyleSheet(QString("background-color: #000000;"));

    QWidget *lineB = new QWidget;
    lineB->setFixedHeight(1);
    lineB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineB->setStyleSheet(QString("background-color: #c0c0c0;"));

    QLabel *groupInfoLabel=new QLabel("Group Comparison");
    groupInfoLabel->setStyleSheet(QString("QLabel {font: bold; font-size: 12px; color : rgb(0,0,0);}"));


    groupInfoLayout2=new QVBoxLayout;
    groupInfoLayout2->addWidget(lineA);
    groupInfoLayout2->addWidget(groupInfoLabel);
    groupInfoLayout2->addWidget(lineB);
    groupInfoLayout2->addWidget(groupInfoLayout);

    QWidget *groupInfoLayoutWidget=new QWidget;
    groupInfoLayoutWidget->setLayout(groupInfoLayout2);
    Frame1=new QSplitter(Qt::Vertical);
    Frame1->addWidget(dataLayout1);
    Frame1->addWidget(groupInfoLayoutWidget);
    Frame1->setChildrenCollapsible(false);
    sizes.clear();
    sizes<<this->height()/2<<this->height()/2;
    Frame1->setSizes(sizes);



    totalFrame=new QTabWidget;
    totalFrame->addTab(Frame1,"Data Management and Group Comparison");
    totalFrame->addTab(tFrame2Widget,"Exploration and Phenotype Classification");
    QFont t;
    t.setBold(true);
    totalFrame->setFont(t);
//    totalFrame->setTabPosition(QTabWidget::West);

    connect(totalFrame,SIGNAL(currentChanged(int)),this,SLOT(UIChanged(int)));

    QHBoxLayout *totalFrameLayout=new QHBoxLayout;
    totalFrameLayout->addWidget(totalFrame);

    setLayout(totalFrameLayout);

    setWindowTitle(tr("Spine Analysis"));

    //this->show();
    QWidget::setFocusPolicy(Qt::ClickFocus);

    checkSpineEnable();


    //startTimer(1000);


    //arrangeRanderPart();



    QFile::copy("setting/defaultModel.pkl","model.pkl");


    for(int g=0;g<4;g++){
        nFlow[g]=0;
        for(int i=0;i<30;i++){
            minFlow[g][i]=0;
            maxFlow[g][i]=0;
            avgFlow[g][i]=0;

        }
    }
    for(int i=0;i<30;i++){
        minValues[i]=0;
        maxValues[i]=0;
        avgValues[i]=0;
    }

    processedTimer = new QTimer(this);
    connect(processedTimer, SIGNAL(timeout()), this, SLOT(checkDatasetGeneration()));


    qDebug()<<"end window init";


}
void Window::imageControlSliderChanged(int a){
    tfRendering->update();
    dataRendering->update();
}
void Window::typeCompareLayoutUpdate(){
    for(int i=0;i<typeCompare.length();i++){
        typeCompare[i]->update();
    }
}
void Window::typeCompareLayoutArrange(){
    int n=typeCompare.length();
    for(int i=0;i<n;i++){
        typeCompareLayout->removeWidget(typeCompare[i]);
    }
    int gridSize=sqrt(n)+0.9999;
    for(int i=0;i<n;i++){
        typeCompareLayout->addWidget(typeCompare[i],i/gridSize,i%gridSize);
    }


}
void Window::renderVarGraph(int a){
    groupCompare->update();
}
void Window::UIChanged(int a){
    clicked=false;
    focusSpine=-1;



    if(a==0){
//        Frame1->addWidget(dataLayout1,1);
        groupInfoLayout->addWidget(featurePlot);
        groupInfoLayout->addWidget(groupCompareBox);
        groupInfoLayout->setSizes(groupInfoSizes);

//        dataMeshLayout->addLayout(dataMeshRenderPart,1);
        dataMeshLayout->addWidget(parallelPlot,1);

        imageControlLayout2_1->addWidget(coloringGroup);

        groupLayout2->addWidget(datagroupGroup);

        setUIbasedOnCurDataIndex();
    }
    else if(a==1){
        groupInfoSizes.clear();
        groupInfoSizes=groupInfoLayout->sizes();
        ControlPart->addWidget(featurePlot);
        ControlPart->addWidget(parallelPlot);
        Rt2->addWidget(coloringGroup);
        onoffLayout->addWidget(datagroupGroup);
        onoffLayout->addWidget(typeGroup);

        checkSpineEnable();
        arrangeRanderPart();
        //preOrdering();
        //arranging=true;

    }
}
void Window::generateDataset(){
    if(processedIndex!=-1){
        QMessageBox msgBox;
        msgBox.setText("Other processing is running...");
        msgBox.setStyle(new DarkStyle);
        msgBox.exec();
        return;
    }
    QString opath=OutputPath->text();
    if(opath==""){
        opath="untitled/";
    }
    if(opath[opath.length()-1]!='/'){
        opath+='/';
    }
    opath="dataset/"+opath;

    if(QDir(opath).exists()){
        QMessageBox msgBox;
        msgBox.setText("Dataset already exist...");
        msgBox.setStyle(new DarkStyle);
        msgBox.exec();
        return;
    }

   int tsize=dataRendering->dataList[curDataIndex]->w*dataRendering->dataList[curDataIndex]->h;

   TinyTIFFFile* tif=TinyTIFFWriter_open("dataset\\temp.tif", 16, dataRendering->dataList[curDataIndex]->w, dataRendering->dataList[curDataIndex]->h);
   if (tif) {
       unsigned short *tdata=new unsigned short[tsize];
       for (uint16_t frame=0; frame<dataRendering->dataList[curDataIndex]->d; frame++) {
           for(int i=0;i<tsize;i++){



               float t=float(dataRendering->dataList[curDataIndex]->data16[frame*tsize+i])/65535;
               float c=float(imageContrastControl->value())/255.0; //0~1
               c=pow(c,3)*10; //0~10
               float minv=1.0/(1+exp(-c*(-6)));
               float maxv=1.0/(1+exp(-c*6));
               t=t*12-6;// -6 ~ 6
               t=t - (imageContrastPosControl->value()-128.0)/128.0 *6;
               t=1.0/(1+exp(-c*t)); //0~1
               t=(t-minv)/(maxv-minv);

               t=t*float(imageBrightControl->value())/255.0*2;
               if(t<0)t=0;
               if(t>1)t=1;

               tdata[i]=t*65535;
           }
           TinyTIFFWriter_writeImage(tif, tdata);
       }
       TinyTIFFWriter_close(tif);
       delete []tdata;
   }
   else {
       QMessageBox msgBox;
       msgBox.setText("Unknown error...");
       msgBox.setStyle(new DarkStyle);
       msgBox.exec();

       return;
   }

    processedIndex=curDataIndex;
    processedPath=opath;
    ProgressLabel->setText(processedItem->text()+": Image pre-processing...");
    DatasetGenerateProgress->setValue(20);
    processedTimer->start(1000);


    std::string query="";

    query+="SpineDetection\\DendriticSpineImageProcessing ";
    //query+=curDataName.toStdString();
    query+="dataset/temp.tif";
    query+=" ";
    query+=opath.toStdString();
    qDebug()<<query.c_str();

    emit runSpineDetection(QString(query.c_str()));
//    system(query.c_str());

//    dataRendering->dataList[curDataIndex]->;
//    curDataName;
}
void Window::checkDatasetGeneration(){
    if(QFileInfo::exists(processedPath+"dataset.DXplorerInfo")){
        qDebug()<<"end dataset generation";
        DatasetGenerateProgress->setValue(0);
        ProgressLabel->setText("");

        processedTimer->stop();

        QString dirO=processedPath;

        QString dir="";
        for(int i=0;i<dirO.length()-1;i++){
            dir+=dirO[i];
            if(dirO[i]=='/'){
                dir="";
            }
        }
        processedItem->setBackgroundColor(QColor(50,50,50));
        processedItem->setTextColor(QColor(200,200,200));
        processedItem->setText(dir);
        //processedItem->setSelected(true);
        //curDataIndex=processedIndex;
        qDebug()<<"start change dataset";


    //    groups[index]->dataList->addItem(dir);
        dataRendering->changeDataset(processedPath,groupIndexOfDataList[processedIndex],processedIndex);
        qDebug()<<"end change dataset";

        setUIbasedOnCurDataIndex();
        typeCompareLayoutUpdate();

        processedIndex=-1;

    }
    else if(QFileInfo::exists(processedPath+"label.tif")){
        DatasetGenerateProgress->setValue(80);
        ProgressLabel->setText(processedItem->text()+": 3D mesh and feature generation...");

    }
    else if(QDir(processedPath+"spine").exists()){
        DatasetGenerateProgress->setValue(60);
        ProgressLabel->setText(processedItem->text()+": Spine detection...");
    }
    else if(QFileInfo::exists(processedPath+"data.tif")){
        DatasetGenerateProgress->setValue(40);
        ProgressLabel->setText(processedItem->text()+": Binarization...");



    }
    update();

}
void Window::handleGroupAdd(){
    int index=groups.size();
    DataGroup *newGroup=new DataGroup(index);

    for(int i=0;i<30;i++)newGroup->totalFeature[i]=0;

    preGroup->addItem(newGroup->name->text());
    postGroup->addItem(newGroup->name->text());

    newGroup->name->installEventFilter(this);
    newGroup->name->setMouseTracking(true);
    connect(newGroup->name,SIGNAL(textChanged(QString)),this,SLOT(groupNameChanged(QString)));
    connect(newGroup->add, SIGNAL(pressed()), this,SLOT( addData()));
    connect(newGroup->remove, SIGNAL(pressed()), this,SLOT( removeGroup()));
    connect(newGroup->colorChange, SIGNAL(pressed()), this, SLOT(colorChangeGroup()));

    connect(newGroup->name, SIGNAL(textEdited(QString)), this, SLOT(focusGroup(QString)));
    connect(newGroup->add, SIGNAL(pressed()), this,SLOT( focusGroup()));
    connect(newGroup->colorChange, SIGNAL(pressed()), this, SLOT(focusGroup()));


    connect(newGroup->dataList,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(currentDataChange(QListWidgetItem*)));

    QListWidgetItem *item = new QListWidgetItem();
    groups.push_back(newGroup);
    item->setSizeHint(QSize(50,200));//groups[groups.size()-1]->widget->sizeHint());
    groupList->addItem(item);
    groupList->setItemWidget(item, groups[groups.size() - 1]->widget);

    typeChangeGraph *newTypeCompare=new typeChangeGraph(groups.size() - 1,this);
    typeCompare.push_back(newTypeCompare);
    typeCompareLayoutArrange();

    datagroupOnOff[index]=new QCheckBox(newGroup->name->text());
    char ct[100];
    sprintf(ct,"QCheckBox {font: bold; color : rgb(%d,%d,%d);}",newGroup->color.red(),newGroup->color.green(),newGroup->color.blue());
    datagroupOnOff[index]->setStyleSheet(QString(ct));
    datagroupOnOff[index]->setChecked(true);
    connect(datagroupOnOff[index],SIGNAL(clicked(bool)),this,SLOT(runCheckSpineEnable(bool)));

    datagroupLayout->addWidget(datagroupOnOff[index],index/4,index%4);

    //focusItem(item);

}
void Window::groupNameChanged(QString a){
    QObject *senderObj = sender(); // This will give Sender object
    int index = senderObj->objectName().toInt();
    typeCompare[index]->update();
    preGroup->setItemText(index+1,a);
    postGroup->setItemText(index+1,a);
    datagroupOnOff[index]->setText(a);
}
void Window::colorChangeGroup(){
    QObject *senderObj = sender(); // This will give Sender object
    int index = senderObj->objectName().toInt();
    QColorDialog color_test;
    QColor t=color_test.getColor(groups[index]->color);
    if(t.isValid()){
        groups[index]->color = t;
        groups[index]->colorChange->setStyleSheet(QString("background-color: %1;foreground-color: %1; border-style: none;").arg(t.name()));
        datagroupOnOff[index]->setStyleSheet(QString("QCheckBox {font: bold; color: %1;}").arg(t.name()));

        synchronization();
    }


}
void Window::currentDataChange(QListWidgetItem* a){
    if(processedIndex==-1)processedItem=a;
    curDataIndex=a->whatsThis().toInt();
    curDataName=a->text();
    setUIbasedOnCurDataIndex();
}

void Window::addData(){
    QObject *senderObj = sender(); // This will give Sender object
    int index = senderObj->objectName().toInt();
    QString dir = QFileDialog::getOpenFileName(this, "Select Dataset or Image...", QDir::currentPath()+"/dataset","*.tif *.DXplorerInfo");
    if (dir == "")return;
    addDataSub(dir,index);

}
void Window::addDataSub(QString dir,int index){
    if(dir[dir.length()-4]=='.' && dir[dir.length()-3]=='t'&&
            dir[dir.length()-2]=='i'&&dir[dir.length()-1]=='f'){

        QString name="";
        for(int i=0;i<dir.length();i++){
            if(dir[i]=='/')name="";
            else name+=dir[i];
        }
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(50,30));//groups[groups.size()-1]->widget->sizeHint());
        item->setBackgroundColor(QColor(50,50,50));
        item->setTextColor(QColor(255,100,100));
        item->setText(dir);
        groups[index]->dataList->addItem(item);
        item->setWhatsThis(QString::number(globalDataIndex));
        item->setSelected(true);
        if(processedIndex==-1)processedItem=item;

    //    groups[index]->dataList->addItem(dir);
        dataRendering->loadTifFile(dir,-1);
        curDataIndex=globalDataIndex-1;
        groupIndexOfDataList[curDataIndex]=index;
        curDataName=dir;
        setUIbasedOnCurDataIndex();
        return;
    }

    QString dirO=dir;
    QString tword="";
    QString tdir="";

    dir="";
    for(int i=0;i<dirO.length();i++){
        tword+=dirO[i];
        if(dirO[i]=='/'){
            dir+=tword;
            tdir=tword;
            tword="";
        }
    }
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(50,30));//groups[groups.size()-1]->widget->sizeHint());
    item->setBackgroundColor(QColor(50,50,50));
    item->setTextColor(QColor(200,200,200));
    item->setText(tdir);
    groups[index]->dataList->addItem(item);
    item->setWhatsThis(QString::number(globalDataIndex));
    item->setSelected(true);

    if(processedIndex==-1)processedItem=item;
//    groups[index]->dataList->addItem(dir);
    dataRendering->loadTifFile(dir,index);
    curDataIndex=globalDataIndex-1;
    groupIndexOfDataList[curDataIndex]=index;

    curDataName=tdir;
    setUIbasedOnCurDataIndex();

    typeCompareLayoutUpdate();

}
void Window::setUIbasedOnCurDataIndex(){
    for(int i=0;i<data.length();i++){
        spineEnable[i]=false;
    }
    if(curDataIndex==-1)return;
    if(dataRendering->dataList[curDataIndex]->basePath==""){

        for(int i=0;i<data.length();i++){
            dataMeshRenderPart->removeWidget(spines[i]);
            spines[i]->hide();
        }
        DatasetWidget->show();
        return;
    }

    DatasetWidget->hide();

    for(int j=0;j<featureNum;j++){
        minValues[j]=1000;
        maxValues[j]=-1000;
        avgValues[j]=0;
    }
    for(int i=0;i<dataRendering->dataList[curDataIndex]->spineIndex.length();i++){
        int v=dataRendering->dataList[curDataIndex]->spineIndex[i];
        for(int j=0;j<featureNum;j++){
            avgValues[j]+=data[v][j];
            if(data[v][j]<minValues[j]){
                minValues[j]=data[v][j];
            }
            if(data[v][j]>maxValues[j]){
                maxValues[j]=data[v][j];
            }
        }
    }
    for(int j=0;j<featureNum;j++){
        avgValues[j]/=dataRendering->dataList[curDataIndex]->spineIndex.length();
    }



    int n=dataRendering->dataList[curDataIndex]->spineIndex.length();
    int gridSize=sqrt(n*2)+0.9999;

    for(int i=0;i<data.length();i++){
        dataMeshRenderPart->removeWidget(spines[i]);
        spines[i]->hide();
    }
    for(int i=0;i<n;i++){
        spineEnable[dataRendering->dataList[curDataIndex]->spineIndex[i]]=true;

        dataMeshRenderPart->addWidget(spines[dataRendering->dataList[curDataIndex]->spineIndex[i]],i/gridSize,i%gridSize);
        spines[dataRendering->dataList[curDataIndex]->spineIndex[i]]->show();
    }
}
void Window::focusGroup() {
    QObject *senderObj = sender(); // This will give Sender object
    int index = senderObj->objectName().toInt();
    QListWidgetItem* item = groupList->item(index);
    item->setSelected(true);
    groupList->setCurrentItem(item, QItemSelectionModel::Select);
    groupList->repaint();
}
void Window::focusGroup(QString a) {
    QObject *senderObj = sender(); // This will give Sender object
    int index = senderObj->objectName().toInt();
    QListWidgetItem* item = groupList->item(index);
    item->setSelected(true);
    groupList->setCurrentItem(item, QItemSelectionModel::Select);
    groupList->repaint();
}
void Window::handleGroupDelete(){

}
int fps_start=-1;
int frames=0;
void Window::timerEvent(QTimerEvent *event){
//    frames++;
//    if(fps_start==-1){
//        fps_start=GetTickCount();

//    }
//    else{
//        if(GetTickCount()-fps_start>1000){
//            qDebug()<<"FPS: "<<frames;
//            frames=0;
//            fps_start=GetTickCount();
//        }
//    }

    total_pixel_num=0;
    spine_pixel_num=0;

    if(spines.length()>0)spines[0]->getPixelNum();
//    int tw=this->width();
//    tw=tw-tw%4;
//    GLubyte *pixels = new GLubyte[this->height()*tw*3];
//    glReadPixels(0, 0, tw, this->height(), GL_RGB, GL_UNSIGNED_BYTE, pixels);
//    for(int i=0;i<this->height()*tw;i++){
//        if(pixels[i*3+0]!=255 || pixels[i*3+1]!=255 || pixels[i*3+2]!=255){
//            spine_pixel_num++;
//        }
//    }
//    total_pixel_num+=this->height()*tw;
//    delete[] pixels;

    qDebug()<<"rendered pixels: "<<spine_pixel_num;
    qDebug()<<"total pixels: "<<total_pixel_num;
    qDebug()<<"rendered ratio: "<<float(spine_pixel_num)/total_pixel_num*100;
    qDebug()<<"---------------------";


//    if(arranging && totalFrame->currentIndex()==1){
//        arrangeRanderPart();
//    }


//    int caseN=0;
//    FILE *caseNum=fopen("AutoSaved\\caseNum.txt","r");
//    fscanf(caseNum,"%d",&caseN);
//    fclose(caseNum);

//    caseNum=fopen("AutoSaved\\caseNum.txt","w");
//    fprintf(caseNum,"%d",caseN+1);
//    fclose(caseNum);
//    QPixmap t=this->grab(QRect(QPoint(0, 0), QSize(-1, -1)));
//    t.save("AutoSaved/"+QString::number(caseN)+".jpg");





//    int caseN=0;
//    FILE *caseNum=fopen("AutoSaved\\caseNum.txt","r");
//    fscanf(caseNum,"%d",&caseN);
//    fclose(caseNum);

//    caseNum=fopen("AutoSaved\\caseNum.txt","w");
//    fprintf(caseNum,"%d",caseN+1);
//    fclose(caseNum);

//    char fileName[100];
//    sprintf(fileName,"AutoSaved\\Backup_%d.csv",caseN);

//    FILE *clusteringResult=fopen(fileName,"w");
//    fprintf(clusteringResult,"Name,Result Group,Training Group\n");
//    for(int i=0;i<data.length();i++){
//        fprintf(clusteringResult,"%s,%d,%d\n",spineNames[i].toStdString().c_str(),clusterLabel[i],indexOfSelectedSpines[i]);
//    }
//    fprintf(clusteringResult,"-1,-1,-1\n");


//    fprintf(clusteringResult,"%d\n",numCluster);

//    for(int i=0;i<numCluster;i++){
//        QString tstr=QString("");
//        for(int j=0;j<constrains[i].length();j++){
//            if(constrains[i][j]=='\n' || constrains[i][j]=='\r')tstr=tstr+" ";
//            else tstr=tstr+constrains[i][j];
//        }
//        fprintf(clusteringResult,"%s\n",tstr.toStdString().c_str());
//    }

//    int enabledNum=0;
//    for(int i=0;i<featureNum;i++){
//        if(featureEnables[i])enabledNum++;
//    }

//    fprintf(clusteringResult,"%d\n",enabledNum);

//    for(int i=0;i<featureNum;i++){
//        if(featureEnables[i]){
//            fprintf(clusteringResult,"%s\n",featureNames[i].toStdString().c_str());
//        }
//    }
//    fclose(clusteringResult);
}


void Window::selectReset(){
    for(int i=0;i<data.length();i++){
        displaySpines[i]=false;
    }
}
void Window::selectDisplayMode(bool a){

}

void Window::displayShow(bool a){
    checkSpineEnable();
    synchronization();
}

Window::~Window(){
    if(setting!=NULL){
        delete setting;
    }
//    if(selectionWindow!=NULL){
//        delete selectionWindow;
//    }
    spineDetectionThread.quit();
    spineDetectionThread.wait();
}


void Window::addSpineToGroup(int a, int group){
    if(indexOfSelectedSpines[a]!=-1){
        if(indexOfSelectedSpines[a]==group){
            return;
        }
        deleteSpineFromGroup(a,indexOfSelectedSpines[a]);
    }
    indexOfSelectedSpines[a]=group;
    selectedSpines[group].push_back(a);
    seedChanged=true;
}
void Window::deleteSpineFromGroup(int a, int group){
    selectedSpines[group].removeAt(selectedSpines[group].indexOf(a));
    indexOfSelectedSpines[a]=-1;
    seedChanged=true;
}

void Window::selectionForClustering(){
    if(selectionWindow!=NULL){
        delete selectionWindow;
    }
    selectionWindow=new seedSelectionWindow(this);
    selectionWindow->show();
    seedChanged=true;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), selectionWindow, SLOT(update()));
    timer->start(100);
}

void Window::featureUsageSetting(){
    if(setting!=NULL){
        delete setting;
    }
    setting=new setFeatureUsing(this);
    setting->show();
}

void Window::arrangeRanderPart(){
    qDebug()<<"start arrange renderpart";
    if(enabledSpineNum==0){
        if(dummy==false){
            RenderPart->addItem(dummyItem,0,0);
            dummy=true;
        }
        arranging=false;
        return;
    }

    if(dummy){
        RenderPart->removeItem(dummyItem);
        dummy=false;
    }
    orderingFormatrixView3();
//    orderingFormatrixViewBySOM();

}
void Window::orderingFormatrixView1(){
    int n=spines.length();
    int gridSize=sqrt(enabledSpineNum)+0.9999;
    qDebug()<<enabledSpineNum << gridSize;

    QVector<forSort2> spine_index;
    for(int i=0;i<n;i++){
        if(spineEnable[i]){
            forSort2 t;
            t.index=i;
            t.value=tsneResult[i].y();
            spine_index.push_back(t);
        }
    }
    qSort(spine_index);
    int es=spine_index.size();
    qDebug()<<"sort done"<<es<<spine_index[0].value<<spine_index[1].value;

    int cnt=0;
    for(int i=0;i<gridSize;i++){
        QVector<forSort1> spine_index2;
        for(int j=0;j<gridSize;j++){
            int tind=i*gridSize+j;
            forSort1 t;
            t.index=spine_index[tind].index;
            t.value=tsneResult[t.index].x();
            spine_index2.push_back(t);
            cnt++;
            if(cnt>=es)break;
        }
        int s=spine_index2.size();
        if(s==0)break;
        qSort(spine_index2);
        for(int j=0;j<s;j++){
            RenderPart->addWidget(spines[spine_index2[j].index],i,j);
            if(spines[spine_index2[j].index]->isHidden())spines[spine_index2[j].index]->show();
        }
        if(cnt>=es)break;
    }
    arranging=false;
    qDebug()<<"all done";
}
void Window::preOrdering(){
    int n=spines.length();
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            matrixD[i][j]=0;
            matrixN[i][j]=0;
            indexPos[i][j]=-1;
        }
        posX[i]=0;
        posY[i]=0;
    }

    int gridSize=sqrt(enabledSpineNum)+0.9999;

    QVector<forSort2> spine_index;
    for(int i=0;i<n;i++){
        if(spineEnable[i]){
            forSort2 t;
            t.index=i;
            t.value=tsneResult[i].y();
            spine_index.push_back(t);
        }
    }
    qSort(spine_index);
    int es=spine_index.size();

    int cnt=0;
    for(int i=0;i<gridSize;i++){
        QVector<forSort1> spine_index2;
        for(int j=0;j<gridSize;j++){
            int tind=i*gridSize+j;
            forSort1 t;
            t.index=spine_index[tind].index;
            t.value=tsneResult[t.index].x();
            spine_index2.push_back(t);
            cnt++;
            if(cnt>=es)break;
        }
        int s=spine_index2.size();
        if(s==0)break;
        qSort(spine_index2);
        for(int j=0;j<s;j++){
            posX[spine_index2[j].index]=j;
            posY[spine_index2[j].index]=i;
            indexPos[j][i]=spine_index2[j].index;
        }
        if(cnt>=es)break;
    }
    for(int i=0;i<n;i++){
        if(spineEnable[i]){
            for(int j=0;j<n;j++){
                if(spineEnable[j]){
                    matrixD[i][j]=abs(posX[i]-posX[j])+abs(posY[i]-posY[j]);
                }
            }
        }
    }
    QVector<forSort1> forNx;
    QVector<forSort1> forNy;
    for(int i=0;i<n;i++){
        if(spineEnable[i]){

            forNx.clear();
            forNy.clear();
            forSort1 t;
            for(int j=0;j<n;j++){
                if(spineEnable[j]){

                    t.index=j;
                    t.value=abs(tsneResult[i].x()-tsneResult[j].x());
                    forNx.push_back(t);
                    t.index=j;
                    t.value=abs(tsneResult[i].y()-tsneResult[j].y());
                    forNy.push_back(t);
                }
            }
            qSort(forNx);
            qSort(forNy);
            for(int j=0;j<forNx.size();j++){
                matrixN[i][forNx[j].index]+=j;
                matrixN[i][forNy[j].index]+=j;
            }
        }
    }
}
void Window::orderingFormatrixView2(){
    int n=spines.length();
    int gridSize=sqrt(enabledSpineNum)+0.9999;

    arranging=false;
//    float J=0;
//    for(int i=0;i<n;i++){
//        if(spineEnable[i]){
//            for(int j=0;j<n;j++){
//                if(spineEnable[j]){
//                    J+=abs(matrixD[i][j]-matrixN[i][j]);
//                }
//            }
//        }
//    }
    for(int i=0;i<n;i++){
        if(spineEnable[i]){
            float minv=-10000;
            int mindx=0;
            int mindy=0;
            for(int dx=-1;dx<=1;dx++){
                for(int dy=-1;dy<=1;dy++){
                    if(posX[i]+dx<0 || posX[i]+dx>=gridSize || posY[i]+dy<0 || posY[i]+dy>=gridSize||(posY[i]+dy)*gridSize+posX[i]+dx>=enabledSpineNum )
                        continue;
//                    if(dy!=0 && (posY[i]+dy)*gridSize+posX[i]+dx>=enabledSpineNum)
//                        continue;
                    int a=indexPos[posX[i]+dx][posY[i]+dy];

                    float newv=0;
                    float prev=0;
                    if(a==-1){
                        for(int j=0;j<n;j++){
                            if(spineEnable[j]){
                                prev+=abs(matrixD[i][j]-matrixN[i][j]);
                                newv+=abs(abs(posX[i]+dx-posX[j])+abs(posY[i]+dy-posY[j])-matrixN[i][j]);
                            }
                        }
                    }
                    else{
                        for(int j=0;j<n;j++){
                            if(spineEnable[j]){
                                prev+=abs(matrixD[i][j]-matrixN[i][j])+abs(matrixD[a][j]-matrixN[a][j]);
                                newv+=abs(abs(posX[i]-posX[j])+abs(posY[i]-posY[j])-matrixN[a][j])
                                        +abs(abs(posX[a]-posX[j])+abs(posY[a]-posY[j])-matrixN[i][j]);
                            }
                        }
                    }

                    if(prev-newv>minv && newv<prev){
                        minv=prev-minv;
                        mindx=dx;
                        mindy=dy;
                    }
                }
            }
            if(minv!=-10000){
                arranging=true;
                int a=indexPos[posX[i]+mindx][posY[i]+mindy];
                if(a==-1){
                    indexPos[posX[i]][posY[i]]=-1;
                    posX[i]=posX[i]+mindx;
                    posY[i]=posY[i]+mindy;
                    indexPos[posX[i]][posY[i]]=i;
                }
                else{
                    int tx=posX[a];
                    int ty=posY[a];
                    int tindex=indexPos[tx][ty];

                    indexPos[tx][ty]=indexPos[posX[i]][posY[i]];
                    indexPos[posX[i]][posY[i]]=tindex;

                    posX[a]=posX[i];
                    posY[a]=posY[i];
                    posX[i]=tx;
                    posY[i]=ty;
                }

                for(int j=0;j<n;j++){
                    if(spineEnable[j]){
                        matrixD[i][j]=abs(posX[i]-posX[j])+abs(posY[i]-posY[j]);
                        matrixD[j][i]=abs(posX[i]-posX[j])+abs(posY[i]-posY[j]);
                    }
                }
            }
        }
    }
    for(int i=0;i<spines.length();i++){
        RenderPart->removeWidget(spines[i]);
        spines[i]->hide();

    }
    for(int j=0;j<n;j++){
        if(spineEnable[j]){
            RenderPart->addWidget(spines[j],posY[j],posX[j]);
            if(spines[j]->isHidden())spines[j]->show();
        }
    }
}
void Window::orderingFormatrixView3(){
    int n=spines.length();
    int gridSize=sqrt(enabledSpineNum)+0.9999;
    qDebug()<<enabledSpineNum << gridSize;
    QVector<int> spine_index;
    for(int i=0;i<n;i++){
        if(spineEnable[i]){
            spine_index.push_back(i);
        }
    }
    recursiveDivideY(0,gridSize,0,(enabledSpineNum-1)/gridSize +1,spine_index);

    for(int i=0;i<spines.length();i++){
        RenderPart->removeWidget(spines[i]);
        spines[i]->hide();

    }
    for(int j=0;j<n;j++){
        if(spineEnable[j]){
            RenderPart->addWidget(spines[j],posY[j],posX[j]);
            if(spines[j]->isHidden())spines[j]->show();
        }
    }

    arranging=false;
    qDebug()<<"all done";
}

void Window::recursiveDivideX(int startX,int endX,int startY,int endY,QVector<int> indexList){
    int len=indexList.length();
    QVector<forSort1> spine_index;
    forSort1 t;
    for(int i=0;i<len;i++){
        t.index=indexList[i];
        t.value=tsneResult[t.index].x();
        spine_index.push_back(t);
    }
    qSort(spine_index);

    if(endY<=startY+1){
        for(int i=0;i<len;i++){
            posX[spine_index[i].index]=startX+i;
            posY[spine_index[i].index]=startY;
        }
        return;
    }
    QVector<int> spine_index2;
    QVector<int> spine_index3;

    int len2=(endX-startX+1)/2 * (endY-startY);
    int gridSize=sqrt(enabledSpineNum)+0.9999;
    if((endY-1)*gridSize+startX+(endX-startX+1)/2>enabledSpineNum){
        if(enabledSpineNum%gridSize>=startX){
            len2=(endX-startX+1)/2 * (endY-startY-1) + (enabledSpineNum%gridSize-startX);
        }
        else{
            len2=(endX-startX+1)/2 * (endY-startY-1);
        }
    }
    qDebug()<<"x:"<<len2<<len;

    for(int i=0;i<len2;i++){
        spine_index2.push_back(spine_index[i].index);
    }
    recursiveDivideY(startX,startX+(endX-startX+1)/2,startY,endY,spine_index2);
    for(int i=len2;i<len;i++){
        spine_index3.push_back(spine_index[i].index);
    }
    recursiveDivideY(startX+(endX-startX+1)/2,endX,startY,endY,spine_index3);
}
void Window::recursiveDivideY(int startX,int endX,int startY,int endY,QVector<int> indexList){
    int len=indexList.length();
    QVector<forSort2> spine_index;
    forSort2 t;
    for(int i=0;i<len;i++){
        t.index=indexList[i];
        t.value=tsneResult[t.index].y();
        spine_index.push_back(t);
    }
    qSort(spine_index);

    if(endX<=startX+1){
        for(int i=0;i<len;i++){
            posX[spine_index[i].index]=startX;
            posY[spine_index[i].index]=startY+i;
        }
        return;
    }
    QVector<int> spine_index2;
    QVector<int> spine_index3;

    int len2=(endY-startY+1)/2 * (endX-startX);
    if(len2>len)len2=len;
    qDebug()<<"y:"<<len2<<len;
    for(int i=0;i<len2;i++){
        spine_index2.push_back(spine_index[i].index);
    }
    recursiveDivideX(startX,endX,startY,startY+(endY-startY+1)/2,spine_index2);
    for(int i=len2;i<len;i++){
        spine_index3.push_back(spine_index[i].index);
    }
    recursiveDivideX(startX,endX,startY+(endY-startY+1)/2,endY,spine_index3);
}


void Window::orderingFormatrixViewBySOM(){
    qDebug()<<"start som";

//    int n=spines.length();
//    int gridSize=sqrt(enabledSpineNum);
//    qDebug()<<enabledSpineNum << gridSize;
//    if(enabledSpineNum<4)return;
//    QVector<int> spine_index;
//    for(int i=0;i<n;i++){
//        if(spineEnable[i]){
//            spine_index.push_back(i);
//        }
//    }

//    float *inputData=new float[enabledSpineNum*2];
//    int cnt=0;
//    for(int j=0;j<n;j++){
//        if(spineEnable[j]){
//            inputData[cnt*2+0]=tsneResult[j].x();
//            inputData[cnt*2+1]=tsneResult[j].y();
//            cnt++;
//        }
//    }

//    SOM graphs;
//    graphs.SetData(inputData,enabledSpineNum,2);
//    graphs.InitializeMap(gridSize, gridSize);

//    graphs.SigmaNeighbouringInitial = 1.5;
//    graphs.SigmaNeighbourhoodFinal = 1;
//    graphs.LearningRateInitial = 0.5;
//    graphs.LearningRateFinal = 0.1;
//    graphs.Train(1000);

//    float sommin=100000;
//    float sommax=-100000;
//    for(int i=0;i<gridSize;i++){
//        for(int j=0;j<gridSize;j++){
//            if(graphs.SOMMap(j,i)(0)<sommin)sommin=graphs.SOMMap(j,i)(0);
//            if(graphs.SOMMap(j,i)(0)>sommax)sommax=graphs.SOMMap(j,i)(0);
//        }
//    }

//    for(int i=0;i<gridSize;i++){
//        for(int j=0;j<gridSize;j++){
//            graphs.SOMMap(j,i)(0)=(graphs.SOMMap(j,i)(0)-sommin)/(sommax-sommin);
//        }
//    }

//    sommin=100000;
//    sommax=-100000;
//    for(int i=0;i<gridSize;i++){
//        for(int j=0;j<gridSize;j++){
//            if(graphs.SOMMap(j,i)(1)<sommin)sommin=graphs.SOMMap(j,i)(1);
//            if(graphs.SOMMap(j,i)(1)>sommax)sommax=graphs.SOMMap(j,i)(1);
//        }
//    }

//    for(int i=0;i<gridSize;i++){
//        for(int j=0;j<gridSize;j++){
//            graphs.SOMMap(j,i)(1)=(graphs.SOMMap(j,i)(1)-sommin)/(sommax-sommin);
//        }
//    }


//    for(int i=0;i<gridSize;i++){
//        for(int j=0;j<gridSize;j++){
//            nodePosition[j][i].setX(graphs.SOMMap(j,i)(0));
//            nodePosition[j][i].setY(graphs.SOMMap(j,i)(1));
//        }
//    }


//    for(int i=0;i<gridSize;i++){
//        for(int j=0;j<gridSize;j++){
//            qDebug()<<graphs.SOMMap(j,i)(0)<<graphs.SOMMap(j,i)(1);
//        }
//        qDebug()<<"\n";
//    }


//    for(int i=0;i<spines.length();i++){
//        RenderPart->removeWidget(spines[i]);
//        spines[i]->hide();

//    }
//    int chkInd[1000];
//    for(int i=0;i<n;i++)chkInd[i]=0;
//    for(int k=0;k<gridSize*gridSize;k++){
//        float minDis=10000;
//        int ind=0;
//        int curx=k%gridSize;
//        int cury=k/gridSize;
//        for(int j=0;j<n;j++){
//            if(spineEnable[j]){

//                float dis=(graphs.SOMMap(curx,cury)(0)-tsneResult[j].x())*(graphs.SOMMap(curx,cury)(0)-tsneResult[j].x())
//                        +(graphs.SOMMap(curx,cury)(1)-tsneResult[j].y())*(graphs.SOMMap(curx,cury)(1)-tsneResult[j].y());
//                if(minDis>dis){
//                    minDis=dis;
//                    ind=j;
//                }

//            }
//        }
//        if(chkInd[ind]==0){
//            RenderPart->addWidget(spines[ind],cury,curx);
//            if(spines[ind]->isHidden())spines[ind]->show();
//            chkInd[ind]=1;
//        }
//    }

////    for(int j=0;j<n;j++){
////        if(spineEnable[j]){
////            float minDis=10000;
////            int indx=0;
////            int indy=0;
////            for(int k=0;k<gridSize*gridSize;k++){
////                int curx=k%gridSize;
////                int cury=k/gridSize;
////                float dis=(graphs.SOMMap(curx,cury)(0)-tsneResult[j].x())*(graphs.SOMMap(curx,cury)(0)-tsneResult[j].x())
////                        +(graphs.SOMMap(curx,cury)(1)-tsneResult[j].y())*(graphs.SOMMap(curx,cury)(1)-tsneResult[j].y());
////                if(minDis>dis){
////                    minDis=dis;
////                    indx=curx;
////                    indy=cury;
////                }
////            }
////            RenderPart->addWidget(spines[j],indy,indx);
////            if(spines[j]->isHidden())spines[j]->show();
////        }
////    }

//    arranging=false;
//    qDebug()<<"all done";
}

QSlider *Window::createSlider(int a)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, a);
    slider->setSingleStep(1);
    slider->setPageStep(10);
    slider->setTickInterval(10);
    slider->setTickPosition(QSlider::TicksAbove);
    return slider;
}

void Window::loadFile(const QUrl &fileName)
{
    setWindowFilePath(fileName.toString());
}
void Window::viewChange(int num){
//    return;

    if(num==-2){
        for(int i=0;i<spines.length();i++){
            RenderPart->removeWidget(spines[i]);
            spines[i]->hide();

        }
        //preOrdering();
        //arranging=true;
        arrangeRanderPart();
    }
    else if(num==-1){
        RenderPart->removeWidget(spines[curSpine]);
        //preOrdering();
        //arranging=true;
        arrangeRanderPart();
        curSpine=-1;
    }
    else{
        for(int i=0;i<spines.length();i++){
            if(spineEnable[i]){
                RenderPart->removeWidget(spines[i]);
                spines[i]->hide();
            }

        }

        RenderPart->addWidget(spines[num],0,0);
        spines[num]->show();
        curSpine=num;

    }
    update();
}
void Window::synchronization(){
    changeTitle();

    featurePlot->update();
    parallelPlot->update();
    tsneGraph->update();
    if(totalFrame->currentIndex()==0){
        dataRendering->update();
        typeCompareLayoutUpdate();
    }
}
void Window::changeFocus(bool a){
    isFocus=a;
}

void Window::changeTitle(){
    if(focusSpine==-1){
        mainWindow->setWindowTitle(tr("Spine Analysis"));
        setWindowTitle(tr("Spine Analysis"));
        mainWindow->update();
        this->update();
    }
    else{
        mainWindow->setWindowTitle(spineNames[focusSpine]);
        setWindowTitle(spineNames[focusSpine]);
        mainWindow->update();
        this->update();
    }
}

void Window::saveResult(){
    int caseN=0;
    FILE *caseNum=fopen("ClusteringResult\\caseNum.txt","r");
    fscanf(caseNum,"%d",&caseN);
    fclose(caseNum);

    caseNum=fopen("ClusteringResult\\caseNum.txt","w");
    fprintf(caseNum,"%d",caseN+1);
    fclose(caseNum);

    char fileName[100];
    sprintf(fileName,"ClusteringResult\\clusteringResult_%d.csv",caseN);

    FILE *clusteringResult=fopen(fileName,"w");
    fprintf(clusteringResult,"Name,Result Group,Training Group\n");
    for(int i=0;i<data.length();i++){
        fprintf(clusteringResult,"%s,%d,%d\n",spineNames[i].toStdString().c_str(),clusterLabel[i],indexOfSelectedSpines[i]);
    }
    fprintf(clusteringResult,"-1,-1,-1\n");


    fprintf(clusteringResult,"%d\n",numCluster);

    for(int i=0;i<numCluster;i++){
        QString tstr=QString("");
        for(int j=0;j<constrains[i].length();j++){
            if(constrains[i][j]=='\n' || constrains[i][j]=='\r')tstr=tstr+" ";
            else tstr=tstr+constrains[i][j];
        }
        fprintf(clusteringResult,"%s\n",tstr.toStdString().c_str());
    }

    int enabledNum=0;
    for(int i=0;i<featureNum;i++){
        if(featureEnables[i])enabledNum++;
    }

    fprintf(clusteringResult,"%d\n",enabledNum);

    for(int i=0;i<featureNum;i++){
        if(featureEnables[i]){
            fprintf(clusteringResult,"%s\n",featureNames[i].toStdString().c_str());
        }
    }
    fclose(clusteringResult);

    sprintf(fileName,"ClusteringResult\\clusteringModel_%d.pkl",caseN);

    QFile::copy("model.pkl", fileName);


    FILE *selectionFeatures=fopen("selectionFeatures.csv","w");
    for(int i=0;i<data.length();i++){
        if(indexOfSelectedSpines[i]!=-1){
            fprintf(selectionFeatures,"%d",indexOfSelectedSpines[i]);
            for(int j=0;j<featureNum;j++){
                fprintf(selectionFeatures,",%f",data[i][j]);
            }
            fprintf(selectionFeatures,"\n");
        }
    }
    fclose(selectionFeatures);


}
void Window::runClassification(){
    int k=GroupNumSet->value();
    for(int i=0;i<numCluster;i++){
        clusterOnOffLayout->removeWidget(clusterOnOff[i]);
        clusterOnOff[i]->hide();
    }
    numCluster=k;
    for(int i=0;i<numCluster;i++){
        clusterOnOffLayout->addWidget(clusterOnOff[i]);
        clusterOnOff[i]->show();
    }

    bool doClassification=true;
//    for(int i=0;i<k;i++){
//        if(selectedSpines[i].length()==0){
//            doClassification=false;
//        }
//    }
    if(doClassification==false){
        return;
    }

    int enabledNum=0;
    for(int i=0;i<featureNum;i++){
        if(featureEnables[i])enabledNum++;
    }

    int inputSize=0;
    FILE *input=fopen("classificationInput.raw","wb");
    FILE *label=fopen("classificationLabel.raw","wb");

    FILE *bootstrappedFeatures=fopen("bootstrappedFeatures","r");
    while(1){
        int _label;
        fscanf(bootstrappedFeatures,"%d",&_label);
        if(_label==-1)break;
        inputSize++;
        fwrite(&_label,4,1,label);
        float _feature;
        for(int j=0;j<featureNum;j++){
            fscanf(bootstrappedFeatures,"%f",&_feature);
            if(featureEnables[j]){
                float v=(_feature-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());

                fwrite(&v,4,1,input);
            }
        }
    }
    fclose(bootstrappedFeatures);

    for(int i=0;i<k;i++){
        qDebug()<<selectedSpines[i].length();
        for(int j=0;j<selectedSpines[i].length();j++){
            qDebug()<<selectedSpines[i][j];

            inputSize++;
            fwrite(&i,4,1,label);


            for(int f=0;f<featureNum;f++){
                if(featureEnables[f]){
                    float v=(data[selectedSpines[i][j]][f]-featureRanges[f].x())/(featureRanges[f].y()-featureRanges[f].x());
                    fwrite(&v,4,1,input);
                }
            }

//            for(int f=0;f<featureNum;f++){
//                if(featureEnables[f]){
//                    float v=(data[selectedSpines[i][j]][f]-featureRanges[f].x())/(featureRanges[f].y()-featureRanges[f].x());
//                    fwrite(&v,4,1,input);
//                }
//            }
        }
    }
    fclose(input);
    fclose(label);

    FILE *totalData=fopen("classificationData.raw","wb");
    for(int i=0;i<data.length();i++){
        for(int j=0;j<featureNum;j++){
            if(featureEnables[j]){
                float v=(data[i][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
                fwrite(&v,4,1,totalData);
            }
        }


//        for(int j=0;j<featureNum;j++){
//            if(featureEnables[j]){
//                float v=(data[i][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
//                fwrite(&v,4,1,totalData);
//            }
//        }
    }
    fclose(totalData);

    char tt[30];
    std::string query;
    query="python classification.py ";
//    query="python classification_randomForest.py ";
    query+=itoa(inputSize,tt,10);
    query+=" ";
    query+=itoa(enabledNum,tt,10);
    query+=" ";
    query+=itoa(k,tt,10);
    query+=" > classificationLog";

    qDebug()<<query.c_str()<<endl;

    system(query.c_str());

    int groupNum=GroupNumSet->value();
    float *resultProba=new float[groupNum];
    bool *chk=new bool[groupNum];
    FILE *result=fopen("classificationResult.raw","rb");
    for(int i=0;i<data.length();i++){
        fread(resultProba,4,groupNum,result);
        float maxProba;
        int index;
        maxProba=0;
        index=-1;
        for(int j=0;j<groupNum;j++){
            if(resultProba[j]>maxProba){
                maxProba=resultProba[j];
                index=j;
            }
        }

        clusterLabel[i]=index;

    }
    fclose(result);
    delete resultProba;
    delete chk;

    selectionWindow->doMakeStandard(1);
    selectionWindow->doMakeStandard(2);
    selectionWindow->doMakeStandard(3);

    synchronization();
    checkSpineEnable();

    saveResult();

}

bool Window::checkConstraint(int numConstrain,int spineNum){
    bool IsGood=true;
    bool orConstraint[10];
    bool orCheck[10];
    for(int o=0;o<10;o++){
        orConstraint[o]=false;
        orCheck[o]=false;
    }

    QStringList eachConstrain=constrains[numConstrain].split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    for(int j=0;j<eachConstrain.count();j++){

        int IsOr=-1;
        int compareType=0;
        int formulaType=0;
        int len=QString(eachConstrain[j]).length();
        for(int k=0;k<len;k++){
            if(QString(eachConstrain[j]).at(k).toLatin1()=='<')formulaType++;
        }
        if(formulaType==0){
            for(int k=0;k<len;k++){
                if(QString(eachConstrain[j]).at(k).toLatin1()=='>')formulaType++;
            }
            if(formulaType==1){
                compareType=2;
            }
        }
        if(formulaType==1){
            float firstV=0;
            bool endFirst=false;
            int variType=0;
            float secondV=0;
            int cur=0;

            if(QString(eachConstrain[j]).at(0).toLatin1()=='('){
                IsOr=QString(eachConstrain[j]).at(1).toLatin1()-'0';
                orCheck[IsOr]=true;
                cur=3;
            }

            if(QString(eachConstrain[j]).at(cur).toLatin1()<='9' && QString(eachConstrain[j]).at(cur).toLatin1()>='0'){
                variType=0; //number
            }
            else{
                variType=1; //feature
            }
            QString tRes=QString("");
            int calType=0;
            while(1){
                if(variType==0){
                    char ch=QString(eachConstrain[j]).at(cur).toLatin1();
                    if(ch=='<' || ch=='>'){
                        endFirst=true;
                        if(QString(eachConstrain[j]).at(cur+1).toLatin1()=='='){
                            compareType++;
                            cur++;
                        }

                        qDebug()<<tRes;
                        qDebug()<<tRes.toFloat();
                        if(calType==0){ //+
                            firstV+=tRes.toFloat();
                        }
                        else if(calType==1){ //-
                            firstV-=tRes.toFloat();
                        }
                        else if(calType==2){ // div
                            float tt=tRes.toFloat();
                            if(tt!=0)firstV/=tt;
                        }
                        else if(calType==3){ //mul
                            firstV*=tRes.toFloat();
                        }

                        tRes=QString("");
                        break;
                    }
                    if((ch<='9' && ch>='0') || ch=='.'){
                        tRes.append(ch);
                    }
                    else{
                        qDebug()<<tRes;
                        qDebug()<<tRes.toFloat();
                        if(calType==0){ //+
                            firstV+=tRes.toFloat();
                        }
                        else if(calType==1){ //-
                            firstV-=tRes.toFloat();
                        }
                        else if(calType==2){ // div
                            float tt=tRes.toFloat();
                            if(tt!=0)firstV/=tt;
                        }
                        else if(calType==3){ //mul
                            firstV*=tRes.toFloat();
                        }

                        if(ch=='+')calType=0;
                        if(ch=='-')calType=1;
                        if(ch=='/')calType=2;
                        if(ch=='*')calType=3;
                        tRes=QString("");
                        ch=QString(eachConstrain[j]).at(cur+1).toLatin1();
                        if(ch<='9' && ch>='0'){
                            variType=0;
                        }
                        else variType=1;
                    }
                    cur++;
                }
                else{
                    char ch=QString(eachConstrain[j]).at(cur).toLatin1();
                    if(ch=='<' || ch=='>'){
                        endFirst=true;
                        if(QString(eachConstrain[j]).at(cur+1).toLatin1()=='='){
                            compareType++;
                            cur++;
                        }


                        float fValue=0;
                        for(int f=0;f<featureNum;f++){
                            if(featureNames[f]==tRes){
                                fValue=data[spineNum][f];
                            }
                        }
                        if(calType==0){ //+
                            firstV+=fValue;
                        }
                        else if(calType==1){ //-
                            firstV-=fValue;
                        }
                        else if(calType==2){ // div
                            float tt=fValue;
                            if(tt!=0)firstV/=tt;
                        }
                        else if(calType==3){ //mul
                            firstV*=fValue;
                        }

                        tRes=QString("");

                        break;
                    }
                    if(ch!='+' && ch!='-' && ch!='/' && ch!='*'){
                        tRes.append(ch);
                    }
                    else{
                        float fValue=0;
                        for(int f=0;f<featureNum;f++){
                            if(featureNames[f]==tRes){
                                fValue=data[spineNum][f];
                            }
                        }
                        if(calType==0){ //+
                            firstV+=fValue;
                        }
                        else if(calType==1){ //-
                            firstV-=fValue;
                        }
                        else if(calType==2){ // div
                            float tt=fValue;
                            if(tt!=0)firstV/=tt;
                        }
                        else if(calType==3){ //mul
                            firstV*=fValue;
                        }

                        if(ch=='+')calType=0;
                        if(ch=='-')calType=1;
                        if(ch=='/')calType=2;
                        if(ch=='*')calType=3;
                        tRes=QString("");
                        ch=QString(eachConstrain[j]).at(cur+1).toLatin1();
                        if(ch<='9' && ch>='0'){
                            variType=0;
                        }
                        else variType=1;
                    }
                    cur++;
                }
            }


            cur++;

            if(QString(eachConstrain[j]).at(cur).toLatin1()<='9' && QString(eachConstrain[j]).at(cur).toLatin1()>='0'){
                variType=0; //number
            }
            else{
                variType=1; //feature
            }
            tRes=QString("");
            calType=0;
            while(1){
                if(variType==0){
                    if(cur==len){
                        if(calType==0){ //+
                            secondV+=tRes.toFloat();
                        }
                        else if(calType==1){ //-
                            secondV-=tRes.toFloat();
                        }
                        else if(calType==2){ // div
                            float tt=tRes.toFloat();
                            if(tt!=0)secondV/=tt;
                        }
                        else if(calType==3){ //mul
                            secondV*=tRes.toFloat();
                        }

                        break;
                    }
                    char ch=QString(eachConstrain[j]).at(cur).toLatin1();

                    if((ch<='9' && ch>='0') || ch=='.'){
                        tRes.append(ch);
                    }
                    else{
                        if(calType==0){ //+
                            secondV+=tRes.toFloat();
                        }
                        else if(calType==1){ //-
                            secondV-=tRes.toFloat();
                        }
                        else if(calType==2){ // div
                            float tt=tRes.toFloat();
                            if(tt!=0)secondV/=tt;
                        }
                        else if(calType==3){ //mul
                            secondV*=tRes.toFloat();
                        }

                        if(ch=='+')calType=0;
                        if(ch=='-')calType=1;
                        if(ch=='/')calType=2;
                        if(ch=='*')calType=3;
                        tRes=QString("");
                        ch=QString(eachConstrain[j]).at(cur+1).toLatin1();
                        if(ch<='9' && ch>='0'){
                            variType=0;
                        }
                        else variType=1;
                    }
                    cur++;
                }
                else{
                    if(cur==len){

                        float fValue=0;
                        for(int f=0;f<featureNum;f++){
                            if(featureNames[f]==tRes){
                                fValue=data[spineNum][f];
                            }
                        }
                        if(calType==0){ //+
                            secondV+=fValue;
                        }
                        else if(calType==1){ //-
                            secondV-=fValue;
                        }
                        else if(calType==2){ // div
                            float tt=fValue;
                            if(tt!=0)secondV/=tt;
                        }
                        else if(calType==3){ //mul
                            secondV*=fValue;
                        }


                        break;
                    }
                    char ch=QString(eachConstrain[j]).at(cur).toLatin1();

                    if(ch!='+' && ch!='-' && ch!='/' && ch!='*'){
                        tRes.append(ch);
                    }
                    else{
                        float fValue=0;
                        for(int f=0;f<featureNum;f++){
                            if(featureNames[f]==tRes){
                                fValue=data[spineNum][f];
                            }
                        }
                        if(calType==0){ //+
                            secondV+=fValue;
                        }
                        else if(calType==1){ //-
                            secondV-=fValue;
                        }
                        else if(calType==2){ // div
                            float tt=fValue;
                            if(tt!=0)secondV/=tt;
                        }
                        else if(calType==3){ //mul
                            secondV*=fValue;
                        }

                        if(ch=='+')calType=0;
                        if(ch=='-')calType=1;
                        if(ch=='/')calType=2;
                        if(ch=='*')calType=3;
                        tRes=QString("");
                        ch=QString(eachConstrain[j]).at(cur+1).toLatin1();
                        if(ch<='9' && ch>='0'){
                            variType=0;
                        }
                        else variType=1;
                    }
                    cur++;
                }
            }
            qDebug()<<firstV<<" "<<secondV;

            if(IsOr==-1){
                if(compareType==0){
                    if(firstV>=secondV){
                        IsGood=false;
                        break;
                    }
                }
                else if(compareType==1){
                    if(firstV>secondV){
                        IsGood=false;
                        break;
                    }
                }
                else if(compareType==2){
                    if(firstV<=secondV){
                        IsGood=false;
                        break;
                    }
                }
                else if(compareType==3){
                    if(firstV<secondV){
                        IsGood=false;
                        break;
                    }
                }
            }
            else{
                if(compareType==0){
                    if(firstV>=secondV){
                        continue;
                    }
                }
                else if(compareType==1){
                    if(firstV>secondV){
                        continue;

                    }
                }
                else if(compareType==2){
                    if(firstV<=secondV){
                        continue;

                    }
                }
                else if(compareType==3){
                    if(firstV<secondV){
                        continue;

                    }
                }
                orConstraint[IsOr]=true;
            }



        }
        if(formulaType==2){

            int curCompareType[2];
            QString curConstrain[2];
            curConstrain[0]="";
            int operCnt=0;
            for(int q=0;q<QString(eachConstrain[j]).length();q++){
                char ch=QString(eachConstrain[j]).at(q).toLatin1();
                if(ch=='<' || ch=='>'){
                    operCnt++;
                    if(operCnt==1){
                        if(ch=='<'){
                            if(QString(eachConstrain[j]).at(q+1).toLatin1()=='=')curCompareType[0]=1;
                            else curCompareType[0]=0;
                        }
                        if(ch=='>'){
                            if(QString(eachConstrain[j]).at(q+1).toLatin1()=='=')curCompareType[0]=3;
                            else curCompareType[0]=2;
                        }
                    }
                    else if(operCnt==2){
                        if(ch=='<'){
                            if(QString(eachConstrain[j]).at(q+1).toLatin1()=='=')curCompareType[1]=1;
                            else curCompareType[1]=0;
                        }
                        if(ch=='>'){
                            if(QString(eachConstrain[j]).at(q+1).toLatin1()=='=')curCompareType[1]=3;
                            else curCompareType[1]=2;
                        }
                    }
                }
                if(operCnt==2)break;
                curConstrain[0]+=ch;
            }

            curConstrain[1]="";
            operCnt=0;
            for(int q=0;q<QString(eachConstrain[j]).length();q++){
                char ch=QString(eachConstrain[j]).at(q).toLatin1();
                if(operCnt>0)
                    curConstrain[1]+=ch;
                if(ch=='<' || ch=='>'){
                    operCnt++;
                }
            }

            float firstV[2];
            float secondV[2];

            for(int q=0;q<2;q++){
                int len=curConstrain[q].length();
                qDebug()<<curConstrain[q];
                firstV[q]=0;
                secondV[q]=0;
                bool endFirst=false;
                int variType=0;
                int cur=0;

                if(curConstrain[q].at(0).toLatin1()=='('){
                    IsOr=curConstrain[q].at(1).toLatin1()-'0';
                    orCheck[IsOr]=true;
                    cur=3;
                }

                if(curConstrain[q].at(cur).toLatin1()<='9' && curConstrain[q].at(cur).toLatin1()>='0'){
                    variType=0; //number
                }
                else{
                    variType=1; //feature
                }
                QString tRes=QString("");
                int calType=0;
                while(1){
                    if(variType==0){
                        char ch=curConstrain[q].at(cur).toLatin1();
                        if(ch=='<' || ch=='>'){
                            endFirst=true;
                            if(curConstrain[q].at(cur+1).toLatin1()=='='){
                                compareType++;
                                cur++;
                            }

                            qDebug()<<tRes;
                            qDebug()<<tRes.toFloat();
                            if(calType==0){ //+
                                firstV[q]+=tRes.toFloat();
                            }
                            else if(calType==1){ //-
                                firstV[q]-=tRes.toFloat();
                            }
                            else if(calType==2){ // div
                                float tt=tRes.toFloat();
                                if(tt!=0)firstV[q]/=tt;
                            }
                            else if(calType==3){ //mul
                                firstV[q]*=tRes.toFloat();
                            }

                            tRes=QString("");
                            break;
                        }
                        if((ch<='9' && ch>='0') || ch=='.'){
                            tRes.append(ch);
                        }
                        else{
                            qDebug()<<tRes;
                            qDebug()<<tRes.toFloat();
                            if(calType==0){ //+
                                firstV[q]+=tRes.toFloat();
                            }
                            else if(calType==1){ //-
                                firstV[q]-=tRes.toFloat();
                            }
                            else if(calType==2){ // div
                                float tt=tRes.toFloat();
                                if(tt!=0)firstV[q]/=tt;
                            }
                            else if(calType==3){ //mul
                                firstV[q]*=tRes.toFloat();
                            }

                            if(ch=='+')calType=0;
                            if(ch=='-')calType=1;
                            if(ch=='/')calType=2;
                            if(ch=='*')calType=3;
                            tRes=QString("");
                            ch=curConstrain[q].at(cur+1).toLatin1();
                            if(ch<='9' && ch>='0'){
                                variType=0;
                            }
                            else variType=1;
                        }
                        cur++;
                    }
                    else{
                        char ch=curConstrain[q].at(cur).toLatin1();
                        if(ch=='<' || ch=='>'){
                            endFirst=true;
                            if(curConstrain[q].at(cur+1).toLatin1()=='='){
                                compareType++;
                                cur++;
                            }


                            float fValue=0;
                            for(int f=0;f<featureNum;f++){
                                if(featureNames[f]==tRes){
                                    fValue=data[spineNum][f];
                                }
                            }
                            if(calType==0){ //+
                                firstV[q]+=fValue;
                            }
                            else if(calType==1){ //-
                                firstV[q]-=fValue;
                            }
                            else if(calType==2){ // div
                                float tt=fValue;
                                if(tt!=0)firstV[q]/=tt;
                            }
                            else if(calType==3){ //mul
                                firstV[q]*=fValue;
                            }

                            tRes=QString("");

                            break;
                        }
                        if(ch!='+' && ch!='-' && ch!='/' && ch!='*'){
                            tRes.append(ch);
                        }
                        else{
                            float fValue=0;
                            for(int f=0;f<featureNum;f++){
                                if(featureNames[f]==tRes){
                                    fValue=data[spineNum][f];
                                }
                            }
                            if(calType==0){ //+
                                firstV[q]+=fValue;
                            }
                            else if(calType==1){ //-
                                firstV[q]-=fValue;
                            }
                            else if(calType==2){ // div
                                float tt=fValue;
                                if(tt!=0)firstV[q]/=tt;
                            }
                            else if(calType==3){ //mul
                                firstV[q]*=fValue;
                            }

                            if(ch=='+')calType=0;
                            if(ch=='-')calType=1;
                            if(ch=='/')calType=2;
                            if(ch=='*')calType=3;
                            tRes=QString("");
                            ch=curConstrain[q].at(cur+1).toLatin1();
                            if(ch<='9' && ch>='0'){
                                variType=0;
                            }
                            else variType=1;
                        }
                        cur++;
                    }
                }


                cur++;

                if(curConstrain[q].at(cur).toLatin1()<='9' && curConstrain[q].at(cur).toLatin1()>='0'){
                    variType=0; //number
                }
                else{
                    variType=1; //feature
                }
                tRes=QString("");
                calType=0;
                while(1){
                    if(variType==0){
                        if(cur==len){
                            if(calType==0){ //+
                                secondV[q]+=tRes.toFloat();
                            }
                            else if(calType==1){ //-
                                secondV[q]-=tRes.toFloat();
                            }
                            else if(calType==2){ // div
                                float tt=tRes.toFloat();
                                if(tt!=0)secondV[q]/=tt;
                            }
                            else if(calType==3){ //mul
                                secondV[q]*=tRes.toFloat();
                            }

                            break;
                        }
                        char ch=curConstrain[q].at(cur).toLatin1();

                        if((ch<='9' && ch>='0') || ch=='.'){
                            tRes.append(ch);
                        }
                        else{
                            if(calType==0){ //+
                                secondV[q]+=tRes.toFloat();
                            }
                            else if(calType==1){ //-
                                secondV[q]-=tRes.toFloat();
                            }
                            else if(calType==2){ // div
                                float tt=tRes.toFloat();
                                if(tt!=0)secondV[q]/=tt;
                            }
                            else if(calType==3){ //mul
                                secondV[q]*=tRes.toFloat();
                            }

                            if(ch=='+')calType=0;
                            if(ch=='-')calType=1;
                            if(ch=='/')calType=2;
                            if(ch=='*')calType=3;
                            tRes=QString("");
                            ch=curConstrain[q].at(cur+1).toLatin1();
                            if(ch<='9' && ch>='0'){
                                variType=0;
                            }
                            else variType=1;
                        }
                        cur++;
                    }
                    else{
                        if(cur==len){

                            float fValue=0;
                            for(int f=0;f<featureNum;f++){
                                if(featureNames[f]==tRes){
                                    fValue=data[spineNum][f];
                                }
                            }
                            if(calType==0){ //+
                                secondV[q]+=fValue;
                            }
                            else if(calType==1){ //-
                                secondV[q]-=fValue;
                            }
                            else if(calType==2){ // div
                                float tt=fValue;
                                if(tt!=0)secondV[q]/=tt;
                            }
                            else if(calType==3){ //mul
                                secondV[q]*=fValue;
                            }


                            break;
                        }
                        char ch=curConstrain[q].at(cur).toLatin1();

                        if(ch!='+' && ch!='-' && ch!='/' && ch!='*'){
                            tRes.append(ch);
                        }
                        else{
                            float fValue=0;
                            for(int f=0;f<featureNum;f++){
                                if(featureNames[f]==tRes){
                                    fValue=data[spineNum][f];
                                }
                            }
                            if(calType==0){ //+
                                secondV[q]+=fValue;
                            }
                            else if(calType==1){ //-
                                secondV[q]-=fValue;
                            }
                            else if(calType==2){ // div
                                float tt=fValue;
                                if(tt!=0)secondV[q]/=tt;
                            }
                            else if(calType==3){ //mul
                                secondV[q]*=fValue;
                            }

                            if(ch=='+')calType=0;
                            if(ch=='-')calType=1;
                            if(ch=='/')calType=2;
                            if(ch=='*')calType=3;
                            tRes=QString("");
                            ch=curConstrain[q].at(cur+1).toLatin1();
                            if(ch<='9' && ch>='0'){
                                variType=0;
                            }
                            else variType=1;
                        }
                        cur++;
                    }
                }
                qDebug()<<firstV[q]<<" "<<secondV[q];

            }



            if(IsOr==-1){
                if(curCompareType[0]==0){
                    if(firstV[0]>=secondV[0]){
                        IsGood=false;
                        break;
                    }
                }
                else if(curCompareType[0]==1){
                    if(firstV[0]>secondV[0]){
                        IsGood=false;
                        break;
                    }
                }
                else if(curCompareType[0]==2){
                    if(firstV[0]<=secondV[0]){
                        IsGood=false;
                        break;
                    }
                }
                else if(curCompareType[0]==3){
                    if(firstV[0]<secondV[0]){
                        IsGood=false;
                        break;
                    }
                }

                if(curCompareType[1]==0){
                    if(firstV[1]>=secondV[1]){
                        IsGood=false;
                        break;
                    }
                }
                else if(curCompareType[1]==1){
                    if(firstV[1]>secondV[1]){
                        IsGood=false;
                        break;
                    }
                }
                else if(curCompareType[1]==2){
                    if(firstV[1]<=secondV[1]){
                        IsGood=false;
                        break;
                    }
                }
                else if(curCompareType[1]==3){
                    if(firstV[1]<secondV[1]){
                        IsGood=false;
                        break;
                    }
                }

            }
            else{
                if(curCompareType[0]==0){
                    if(firstV[0]>=secondV[0]){
                        continue;
                    }
                }
                else if(curCompareType[0]==1){
                    if(firstV[0]>secondV[0]){
                        continue;

                    }
                }
                else if(curCompareType[0]==2){
                    if(firstV[0]<=secondV[0]){
                        continue;

                    }
                }
                else if(curCompareType[0]==3){
                    if(firstV[0]<secondV[0]){
                        continue;
                    }
                }
                if(curCompareType[1]==0){
                    if(firstV[1]>=secondV[1]){
                        continue;
                    }
                }
                else if(curCompareType[1]==1){
                    if(firstV[1]>secondV[1]){
                        continue;

                    }
                }
                else if(curCompareType[1]==2){
                    if(firstV[1]<=secondV[1]){
                        continue;

                    }
                }
                else if(curCompareType[1]==3){
                    if(firstV[1]<secondV[1]){
                        continue;
                    }
                }


                orConstraint[IsOr]=true;
            }


        }
    }

    for(int o=0;o<10;o++){
        if(orCheck[o]){
            if(orConstraint[o]==false)IsGood=false;
        }
    }
    if(IsGood){
        return true;
    }
    return false;
}
void Window::optimizedKmeansClustering(){

    int k=GroupNumSet->value();


    for(int i=0;i<numCluster;i++){
        clusterOnOffLayout->removeWidget(clusterOnOff[i]);
        clusterOnOff[i]->hide();
    }
    numCluster=k;
    for(int i=0;i<numCluster;i++){
        clusterOnOffLayout->addWidget(clusterOnOff[i]);
        clusterOnOff[i]->show();
    }
    qsrand(1111);

    int enabledFeatureNum=0;
    for(int i=0;i<featureNum;i++){
        if(featureEnables[i])enabledFeatureNum++;
    }

    bool featureSelect[30][30];
    for(int i=0;i<30;i++){
        for(int j=0;j<30;j++){
            featureSelect[i][j]=true;
        }
    }



    int nElement[30];
    for(int i=0;i<k;i++){
        if(selectedSpines[i].length()>0){
            for(int j=0;j<featureNum;j++){
                if(featureEnables[j]){
                    centers[i][j]=0;
                    for(int k=0;k<selectedSpines[i].length();k++){
                        centers[i][j]+=(data[selectedSpines[i][k]][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
                    }
                    centers[i][j]/=selectedSpines[i].length();
                }
            }
        }
        else{
            for(int j=0;j<featureNum;j++){
                if(featureEnables[j])centers[i][j]=float(qrand())/RAND_MAX;
            }
        }
    }
    while(1){
        int diff=0;
        for(int i=0;i<data.length();i++){
            float minDis=100000;
            int index=0;
            for(int n=0;n<k;n++){
                float dis=0;
                for(int j=0;j<featureNum;j++){
                    if(featureEnables[j]){
                        float normV=(data[i][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
                        dis+=featureWeights[j]*(normV-centers[n][j])*(normV-centers[n][j]);

                    }
                }
                if(dis<minDis){
                    minDis=dis;
                    index=n;
                }
            }
            if(clusterLabel[i]!=index)diff++;
            clusterLabel[i]=index;
        }

        for(int i=0;i<k;i++){
            for(int j=0;j<featureNum;j++){
                centers[i][j]=0;
            }
            nElement[i]=0;
        }
        for(int i=0;i<data.length();i++){
            for(int j=0;j<featureNum;j++){
                if(featureEnables[j]){
                    float normV=(data[i][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
                    centers[clusterLabel[i]][j]+=normV;
                }
            }
            nElement[clusterLabel[i]]++;
        }
        for(int i=0;i<k;i++){
            for(int j=0;j<featureNum;j++){
                if(featureEnables[j]){
                    centers[i][j]/=nElement[i];
                }
            }
        }
        if(diff<2)break;
    }
    synchronization();
    checkSpineEnable();
    viewChange(-2);

    saveResult();
}


void Window::kmeansClustering(){





    int k=GroupNumSet->value();

    bool doClassification=true;
    for(int i=0;i<k;i++){
        if(selectedSpines[i].length()==0){
            doClassification=false;
        }
    }
    if(doClassification && IsAutoControl){
        optimizedKmeansClustering();
        return;
    }

    for(int i=0;i<numCluster;i++){
        clusterOnOffLayout->removeWidget(clusterOnOff[i]);
        clusterOnOff[i]->hide();
    }
    numCluster=k;
    for(int i=0;i<numCluster;i++){
        clusterOnOffLayout->addWidget(clusterOnOff[i]);
        clusterOnOff[i]->show();
    }
    qsrand(123);

    int enabledFeatureNum=0;
    for(int i=0;i<featureNum;i++){
        if(featureEnables[i])enabledFeatureNum++;
    }

    int nElement[30];
    for(int i=0;i<k;i++){
        if(selectedSpines[i].length()>0){
            for(int j=0;j<featureNum;j++){
                if(featureEnables[j]){
                    centers[i][j]=0;
                    for(int k=0;k<selectedSpines[i].length();k++){
                        centers[i][j]+=(data[selectedSpines[i][k]][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
                    }
                    centers[i][j]/=selectedSpines[i].length();
                }
            }
        }
        else{
            for(int j=0;j<featureNum;j++){
                if(featureEnables[j])centers[i][j]=float(qrand())/RAND_MAX;
            }
        }
    }
    while(1){
        int diff=0;
        for(int i=0;i<data.length();i++){
            float minDis=100000;
            int index=0;
            for(int n=0;n<k;n++){
                float dis=0;
                for(int j=0;j<featureNum;j++){
                    if(featureEnables[j]){
                        float normV=(data[i][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
                        dis+=featureWeights[j]*(normV-centers[n][j])*(normV-centers[n][j]);

                    }
                }
                if(dis<minDis){
                    minDis=dis;
                    index=n;
                }
            }
            if(clusterLabel[i]!=index)diff++;
            clusterLabel[i]=index;
        }

        for(int i=0;i<k;i++){
            for(int j=0;j<featureNum;j++){
                centers[i][j]=0;
            }
            nElement[i]=0;
        }
        for(int i=0;i<data.length();i++){
            for(int j=0;j<featureNum;j++){
                if(featureEnables[j]){
                    float normV=(data[i][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
                    centers[clusterLabel[i]][j]+=normV;
                }
            }
            nElement[clusterLabel[i]]++;
        }
        for(int i=0;i<k;i++){
            for(int j=0;j<featureNum;j++){
                if(featureEnables[j]){
                    centers[i][j]/=nElement[i];
                }
            }
        }
        if(diff<2)break;
    }
    synchronization();
    checkSpineEnable();
    viewChange(-2);

    saveResult();

}
void Window::changeColoringType(int a){
    coloringType=a;
    synchronization();

}
void Window::checkSpineEnable(){
    for(int i=0;i<typeCompare.length();i++){
        if(datagroupOnOff[i]->isChecked()){
            typeCompare[i]->show();
        }
        else{
            typeCompare[i]->hide();
        }
    }
    enabledSpineNum=0;
    if(totalFrame->currentIndex()==0)return;

    for(int i=0;i<data.length();i++){
        spineEnable[i]=true;
        enabledSpineNum++;
        if(spineCorrect[i]==false){
            spineEnable[i]=false;
            enabledSpineNum--;
            continue;
        }

        if(lookEnabled->isChecked()){
            if(!displaySpines[i]){
                spineEnable[i]=false;
                enabledSpineNum--;
            }
            continue;
        }


        int j=0;
        for(;j<featureNum;j++){
            if(selection[j]==false)continue;
            float value1=(data[i][j]-featureRanges[j].x())/(featureRanges[j].y()-featureRanges[j].x());
            if(value1<selectionRange[j].x()-0.03 || value1>selectionRange[j].y()+0.03){
                spineEnable[i]=false;
                enabledSpineNum--;
                break;
            }
        }
        if(j==featureNum && AreaSelection){
            if(selectionAreaStart.x()<selectionAreaEnd.x()){
                if(tsneResult[i].x()<selectionAreaStart.x() ||
                        tsneResult[i].x()>selectionAreaEnd.x()){
                    spineEnable[i]=false;
                    enabledSpineNum--;
                    continue;
                }
            }
            else{
                if(tsneResult[i].x()>selectionAreaStart.x() ||
                        tsneResult[i].x()<selectionAreaEnd.x()){
                    spineEnable[i]=false;
                    enabledSpineNum--;
                    continue;

                }
            }

            if(selectionAreaStart.y()<selectionAreaEnd.y()){
                if(tsneResult[i].y()<selectionAreaStart.y() ||
                        tsneResult[i].y()>selectionAreaEnd.y()){
                    spineEnable[i]=false;
                    enabledSpineNum--;
                    continue;

                }
            }
            else{
                if(tsneResult[i].y()>selectionAreaStart.y() ||
                        tsneResult[i].y()<selectionAreaEnd.y()){
                    spineEnable[i]=false;
                    enabledSpineNum--;
                    continue;

                }
            }
        }

        if(j==featureNum && lookSelectedSpine->isChecked()){
            if(indexOfSelectedSpines[i]==-1){
                spineEnable[i]=false;
                enabledSpineNum--;
            }
            else{
                if(clusterOnOff[indexOfSelectedSpines[i]]->isChecked()==false){
                    spineEnable[i]=false;
                    enabledSpineNum--;
                }
                else if(typeEnable && typeOnOff[typesInt[i]]->isChecked()==false){
                    spineEnable[i]=false;
                    enabledSpineNum--;
                }
            }
        }
        else if(j==featureNum && clusterOnOff[clusterLabel[i]]->isChecked()==false){
            spineEnable[i]=false;
            enabledSpineNum--;
        }
        else if(j==featureNum && typeEnable && typeOnOff[typesInt[i]]->isChecked()==false){
            spineEnable[i]=false;
            enabledSpineNum--;
        }
        else if(j==featureNum && testTrainEnable && testTrainOnOff[TestTrain[i]]->isChecked()==false){
            spineEnable[i]=false;
            enabledSpineNum--;
        }
        else if(j==featureNum && datagroupOnOff[spineGroups[i]]->isChecked()==false){
            spineEnable[i]=false;
            enabledSpineNum--;
        }
    }

    for(int i=0;i<featureNum;i++){
        minValues[i]=1000;
        maxValues[i]=-1000;
        avgValues[i]=0;
    }
    for(int i=0;i<data.length();i++){
        if(spineEnable[i]){
            for(int j=0;j<featureNum;j++){
                avgValues[j]+=data[i][j];
                if(data[i][j]<minValues[j]){
                    minValues[j]=data[i][j];
                }
                if(data[i][j]>maxValues[j]){
                    maxValues[j]=data[i][j];
                }
            }
        }
    }
    if(enabledSpineNum==0){
        for(int i=0;i<featureNum;i++){
            minValues[i]=0;
            maxValues[i]=-0;
            avgValues[i]=0;
        }
    }
    else{
        for(int j=0;j<featureNum;j++){
            avgValues[j]/=enabledSpineNum;
        }
    }
    viewChange(-2);
}
void Window::runCheckSpineEnable(bool a){
    checkSpineEnable();
    synchronization();

}

void Window::generateFlow(){
    for(int g=0;g<4;g++){
        for(int i=0;i<featureNum;i++){
            minFlow[g][i]=1000;
            maxFlow[g][i]=-1000;
            avgFlow[g][i]=0;
        }
    }
    for(int i=0;i<4;i++)nFlow[i]=0;
    for(int i=0;i<data.length();i++){
        if(indexOfSelectedSpines[i]!=-1){
            nFlow[indexOfSelectedSpines[i]]++;

            for(int j=0;j<featureNum;j++){
                avgFlow[indexOfSelectedSpines[i]][j]+=data[i][j];
                if(data[i][j]<minFlow[indexOfSelectedSpines[i]][j]){
                    minFlow[indexOfSelectedSpines[i]][j]=data[i][j];
                }
                if(data[i][j]>maxFlow[indexOfSelectedSpines[i]][j]){
                    maxFlow[indexOfSelectedSpines[i]][j]=data[i][j];
                }
            }
        }
    }

    for(int g=0;g<4;g++){
        if(nFlow[g]==0){
            for(int i=0;i<featureNum;i++){
                minFlow[g][i]=0;
                maxFlow[g][i]=-0;
                avgFlow[g][i]=0;
            }
        }
        else{
            for(int j=0;j<featureNum;j++){
                avgFlow[g][j]/=nFlow[g];
            }
        }
    }
}

