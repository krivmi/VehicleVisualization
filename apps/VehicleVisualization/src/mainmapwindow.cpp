#include "mainmapwindow.h"
#include "initdialog.h"
#include "cam.h"
#include "mapem.h"
#include "math.h"

#include <QMenu>
#include <QStyle>
#include <QMenuBar>
#include <QStatusBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QFileDialog>
#include <QProcess>
#include <QScrollArea>
#include <QSettings>

#include <QMapControl/LayerGeometry.h>
#include <QMapControl/LayerMapAdapter.h>
//#include <QMapControl/MapAdapterGoogle.h>
#include <QMapControl/MapAdapterOSM.h>

MainMapWindow::MainMapWindow(QWidget *parent) : QMainWindow(parent)
{
    initSettings();
    setupMaps();
    setupTopMenu();
    setupMainLayout();

    // QMapControl connects
    QObject::connect(m_map_control, &QMapControl::recenterGPSPoint, this, &MainMapWindow::recenterGPSPoint);
    QObject::connect(this, &MainMapWindow::changeGPSButtonGeometry, m_map_control, &QMapControl::changeGPSButtonGeometry);
    QObject::connect(m_map_control, &QMapControl::scrollViewChangedByMouse, this, &MainMapWindow::focusPointChanged);

    visualizer = std::make_shared<Visualizer>(m_map_control);
    QObject::connect(visualizer.get(), &Visualizer::unitClick, this, &MainMapWindow::unitClicked);
    QObject::connect(visualizer.get(), &Visualizer::hazardClick, this, &MainMapWindow::hazardClicked);
    QObject::connect(visualizer.get(), &Visualizer::trafficLightClick, this, &MainMapWindow::trafficLightClick);

    dataHandler = std::make_shared<DataHandler>(visualizer.get(), this);
    QObject::connect(dataHandler.get(), &DataHandler::changeInfo, this, &MainMapWindow::changeInfo);
    QObject::connect(dataHandler.get(), &DataHandler::MessageToLog, this, &MainMapWindow::newMessageToLog);
    QObject::connect(dataHandler.get(), &DataHandler::trafficLightHide, this, &MainMapWindow::closeTLW);
    QObject::connect(dataHandler.get(), &DataHandler::trafficLightShow, this, &MainMapWindow::trafficLightClick);

    // message parsed event
    QObject::connect(&MessageParser::getInstance(), &MessageParser::messageParsed, dataHandler.get(), &DataHandler::MessageReceived);

    // GPS position received
    QObject::connect(this, &MainMapWindow::GPSPositionReceived, dataHandler.get(), &DataHandler::GPSPositionReceived);

    // event counter connects
    QObject::connect(&eventCounter, &EventCounter::messageShow, dataHandler.get(), &DataHandler::messagePlay);
    QObject::connect(&eventCounter, &EventCounter::messageShow, this, &MainMapWindow::messageEmitted);
    QObject::connect(&eventCounter, &EventCounter::messagesPlayed, this, &MainMapWindow::messagesPlayed);
    QObject::connect(&eventCounter, &EventCounter::playingStarted, this, &MainMapWindow::playingStarted);

    // GPS tracker connects
    tracker = new GPSTracker();
    tracker->moveToThread(&gpsThread);
    QObject::connect(&gpsThread, &QThread::finished, tracker, &QObject::deleteLater);
    QObject::connect(this, &MainMapWindow::startTracker, tracker, &GPSTracker::start);
    QObject::connect(tracker, &GPSTracker::resultReady, this, &MainMapWindow::handleGPSData);
    QObject::connect(tracker, &GPSTracker::GPSstopped, this, &MainMapWindow::GPSstopped);
    gpsThread.start();

    // process handler connect
    QObject::connect(&processHandler, &ProcessHandler::error, this, &MainMapWindow::handleError);

    // *** delete before release
    processHandler.currentFile = "/home/krivmi/QT_projects/VehicleVisualization/apps/resources/trafficFiles/capture_X1_02.pcap";
    lblFileName->setText("Current file: " + getFileNameFromPath(processHandler.currentFile));
    processHandler.startLoading();
    eventCounter.setMessageSize(dataHandler->allMessages.size());
}
void MainMapWindow::changeFont(int size){
    middleWidget->setStyleSheet("font-size: " + QString::number(size) + "px;");
}
void MainMapWindow::GPSstopped(){
    gpsEnabled = false;
    btnToogleGPS->setIcon(QIcon(":/resources/images/gps_off.png"));
    visualizer->removeGPSPositionPoint();
    GPSstatusLbl->setText("GPS: off");
    statusBar()->showMessage("No GPSD running, GPS could not be started...");
}
void MainMapWindow::focusPointChanged()
{
    if(dataHandler->autoModeOn){
        toggleFollowGPS(false);
    }
}
void MainMapWindow::handleError(QString error)
{
    receivingEnabled = false;
    receivingStatusLbl->setText("Receiving: off");
    btn_toogle_receiving->setIcon(QIcon(":/resources/images/rec_off.png"));
    statusBar()->showMessage("Receiving could not be started, error: " + error);
}
void MainMapWindow::handleGPSData(float longitude, float latitude, float orientation)
{
    PointWorldCoord GPSPosition = PointWorldCoord(longitude, latitude);
    emit GPSPositionReceived(GPSPosition, orientation);
    GPSstatusLbl->setText("GPS: on");
}
void MainMapWindow::toogleReceivingMessages()
{
    if(!receivingEnabled){
        if(processHandler.startReceiving() == 0){
            statusBar()->showMessage("Receiving started...");
            receivingEnabled = true;
            btn_toogle_receiving->setIcon(QIcon(":/resources/images/rec_on.png"));
            receivingStatusLbl->setText("Receiving: on");
        }
    } else {
        receivingEnabled = false;
        btn_toogle_receiving->setIcon(QIcon(":/resources/images/rec_off.png"));
        receivingStatusLbl->setText("Receiving: off");

        processHandler.stopReceiving();
        statusBar()->showMessage("Receiving stopped...");
    }
}
void MainMapWindow::setupMaps()
{
    m_map_control = new QMapControl(QSizeF(1280.0f, 640.0f));

    // layer for openstreet maps
    m_map_control->addLayer(std::make_shared<LayerMapAdapter>("Map", std::make_shared<MapAdapterOSM>()));
    //m_map_control->addLayer(std::make_shared<LayerMapAdapter>("Map", std::make_shared<MapAdapterGoogle>()));

    // set focus on Ostrava
    m_map_control->setMapFocusPoint(PointWorldCoord(18.284743, 49.838337));
    m_map_control->setZoom(18);
}
void MainMapWindow::toogleGPS()
{
    if(!gpsEnabled){
        gpsEnabled = true;
        btnToogleGPS->setIcon(QIcon(":/resources/images/gps_on.png"));
        GPSstatusLbl->setText("GPS: connecting");
        emit startTracker();
        statusBar()->showMessage("GPS started...");
    } else {
        gpsEnabled = false;
        btnToogleGPS->setIcon(QIcon(":/resources/images/gps_off.png"));
        visualizer->removeGPSPositionPoint();
        tracker->stop();
        GPSstatusLbl->setText("GPS: off");
        statusBar()->showMessage("GPS stopped...");
    }
}
void MainMapWindow::modeSelected(QAction* action){
    if ( action == mode_manual )
    {
        // stop following GPS point
        toggleFollowGPS(false);

        // show top bar
        topBar->setVisible(true);

        // turn off the auto mode
        dataHandler->autoModeOn = false;

        statusBar()->showMessage("Mode manual...");

        leftMiddleWidget->setMinimumWidth(300);
        leftMiddleWidget->setMaximumWidth(300);
        changeFont(17);

        emit changeGPSButtonGeometry(0, 90, QSize(50, 50));
    }
    else if( mode_auto )
    {
        closeTLW();
        toogleInfo(false);

        // start GPS
        if(!gpsEnabled){ toogleGPS(); }

        // hide top bar
        topBar->setVisible(false);

        // clear data from playing
        eventCounter.reset();
        eventCounter.setMessageSize(0);
        eventCounter.newPlayingCycle = true;

        // clear data on canvas
        visualizer->removeAllGeometries(false);

        dataHandler->clearData();
        deleteLogWidgets();
        lblFileName->setText("Current file: none");
        lblMessageIndex->setText("Messages: 0");

        // start auto mode for crossroads
        dataHandler->autoModeOn = true;

        // start following GPS
        toggleFollowGPS(true);

        // start receiving process
        if( !receivingEnabled )
        {
            toogleReceivingMessages();
            statusBar()->showMessage("Mode auto, GPS started, receiving started...");
        }

        leftMiddleWidget->setMinimumWidth(340);
        leftMiddleWidget->setMaximumWidth(340);
        changeFont(22);

        emit changeGPSButtonGeometry(0, 58, QSize(50, 50));

        statusBar()->showMessage("Mode auto, GPS started, already receiving");
    } else { qInfo() << "That mode has not been added yet..."; }
}
void MainMapWindow::setupTopMenu()
{
    qApp->setAttribute(Qt::AA_DontShowIconsInMenus, false);

    // define actions
    QAction *load = new QAction(QIcon::fromTheme("document-open"), "&Load traffic file", this);
    QAction *quit = new QAction(QIcon::fromTheme("application-exit"), "&Quit", this);
    QAction *fullscreen = new QAction(QIcon::fromTheme("view-fullscreen"), "&Toogle fullscreeen", this);
    QAction *maximize = new QAction("&Maximize window", this); // this->style()->standardIcon(QStyle::SP_TitleBarMaxButton)
    QAction *minimize = new QAction("&Minimize window", this); // this->style()->standardIcon(QStyle::SP_TitleBarMinButton)
    QAction *font = new QAction("&Font toogle", this);
    QAction *init = new QAction("&Change settings", this);

    // set shortcuts
    quit->setShortcut(tr("CTRL+Q"));
    load->setShortcut(tr("CTRL+L"));
    fullscreen->setShortcut(tr("CTRL+F"));

    // *** FILE *** //
    QMenu *file = menuBar()->addMenu("&File");
    file->addAction(load);
    file->addSeparator();
    file->addAction(quit);

    // *** MODE *** //
    QActionGroup* mode_group = new QActionGroup(this);
    mode_manual = new QAction("Manual", mode_group);
    mode_auto = new QAction("Auto", mode_group);

    // Ensure the mode actions are checkable.
    mode_manual->setCheckable(true);
    mode_manual->setChecked(true);
    mode_auto->setCheckable(true);

    QMenu *mode = menuBar()->addMenu("&Mode");
    mode->addAction(mode_manual);
    mode->addAction(mode_auto);

    // *** SETTINGS *** //
    QMenu *settings = menuBar()->addMenu("&Settings");
    settings->addAction(fullscreen);
    settings->addAction(minimize);
    settings->addAction(maximize);
    settings->addAction(font);
    settings->addAction(init);

    // *** Connects *** /
    QObject::connect(mode_group, &QActionGroup::triggered, this, &MainMapWindow::modeSelected);
    QObject::connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    QObject::connect(load, &QAction::triggered, this, &MainMapWindow::openFile);
    QObject::connect(maximize, &QAction::triggered, this, [=]() { this->showMaximized();});
    QObject::connect(minimize, &QAction::triggered, this, [=]() { this->showMinimized();});
    QObject::connect(fullscreen, &QAction::triggered, this, [=]() { (!this->isFullScreen()) ? this->showFullScreen() : this->showNormal();});
    QObject::connect(font, &QAction::triggered, this, [=]() { (fontLarge) ? this->changeFont(17) : this->changeFont(22); fontLarge = !fontLarge;});
    QObject::connect(init, &QAction::triggered, this, &MainMapWindow::initSettingsDialog);
}
void MainMapWindow::initSettings()
{
    QSettings settings("krivmi", "VehicleVisualization");
    QString path = settings.value("projectPath").toString();
    QString recCommand = settings.value("receivingCommand").toString();
    QString loadCommand = settings.value("loadingCommand").toString();
    QString host = settings.value("GPSD/host").toString();
    QString port = settings.value("GPSD/port").toString();
    qInfo() << settings.fileName();

    if (path.isEmpty() || recCommand.isEmpty() || loadCommand.isEmpty() ||
            host.isEmpty() || port.isEmpty()) {
        settings.setValue("projectPath", "/home/krivmi/QT_projects/QMapControl/");
        settings.setValue("receivingCommand", "/bin/sh -c \"tshark -r /tmp/tcpdump_data -T json");
        settings.setValue("loadingCommand", "/bin/sh -c \"tshark -r %FILE -T json");
        settings.setValue("GPSD/host", "127.0.0.1");
        settings.setValue("GPSD/port", "2947");

        statusBar()->showMessage("Default settings loaded...");
        return;
    }
    statusBar()->showMessage("Settings have been already loaded...");
}
void MainMapWindow::initSettingsDialog()
{
    InitDialog * initDialog = new InitDialog(this->m_map_control);
    QObject::connect(initDialog, &InitDialog::submitOK, &processHandler, &ProcessHandler::changeCommandsFromSettings);

    initDialog->show();
}
void MainMapWindow::setupMainLayout()
{
    QWidget * main = new QWidget(this);
    mainAppLayout = new QVBoxLayout(main);
    mainAppLayout->setMargin(0);
    mainAppLayout->setSpacing(0);

    setupTopBar();
    setupMiddleSection();

    mainAppLayout->addWidget(topBar);
    mainAppLayout->addWidget(middleWidget);

    infoW->setVisible(false);
    trafficLightsW->setVisible(false);

    setCentralWidget(main);
}
void MainMapWindow::setupMiddleSection()
{
    middleWidget = new QWidget();
    middleLayout = new QHBoxLayout(middleWidget);
    //middleWidget->setStyleSheet("border: 2px solid orange");

    setupLeftMiddleLayout();
    setupRightMiddleLayout();

    middleLayout->addWidget(leftMiddleWidget, 2);
    middleLayout->addWidget(rightMiddleWidget, 3);
}
void MainMapWindow::setupTopBar()
{
    topBar = new QWidget();
    QHBoxLayout * topBarLayout = new QHBoxLayout(topBar);
    topBar->setStyleSheet("border-bottom: 1px solid gray");
    topBar->setFixedHeight(35);
    topBar->setContentsMargins(5, 5, 5, 5);
    topBarLayout->setMargin(0);

    btn_play = new QPushButton(QIcon(":/resources/images/play.png"), "");
    btn_play->setMaximumWidth(32);
    btn_play->setStyleSheet("border: none;");
    QObject::connect(btn_play, &QPushButton::clicked, this, &MainMapWindow::tooglePlay);

    btn_nextMsg = new QPushButton(QIcon(":/resources/images/next.png"), "");
    btn_nextMsg->setMaximumWidth(32);
    btn_nextMsg->setStyleSheet("border: none");
    QObject::connect(btn_nextMsg, &QPushButton::clicked, this, &MainMapWindow::playNextMessage);

    btn_resetPlaying = new QPushButton(QIcon(":/resources/images/reset.png"), "");
    btn_resetPlaying->setMaximumWidth(32);
    btn_resetPlaying->setStyleSheet("border: none");
    //btn_resetPlaying->setFont(QFont("Verdana", 12));
    QObject::connect(btn_resetPlaying, &QPushButton::clicked, this, &MainMapWindow::resetPlaying);

    lblFileName = new QLabel("Current file: " + getFileNameFromPath(processHandler.currentFile));
    lblFileName->setFont(QFont("Verdana", 12));
    lblFileName->setContentsMargins(15, 0, 0, 0);

    lblMessageIndex = new QLabel("Message: 0");
    lblMessageIndex->setAlignment(Qt::AlignCenter);
    lblMessageIndex->setMaximumWidth(130);
    lblMessageIndex->setFont(QFont("Verdana", 12));

    QLabel * line1 = new QLabel();
    line1->setStyleSheet("border: 1px solid gray");
    line1->setFixedWidth(1);
    QLabel * line2 = new QLabel();
    line2->setStyleSheet("border: 1px solid gray");
    line2->setFixedWidth(1);
    QLabel * line3 = new QLabel();
    line3->setStyleSheet("border: 1px solid gray");
    line3->setFixedWidth(1);

    topBarLayout->addWidget(btn_play);
    topBarLayout->addWidget(btn_nextMsg);
    topBarLayout->addWidget(line1);
    topBarLayout->addWidget(btn_resetPlaying);
    topBarLayout->addWidget(line2);
    topBarLayout->addWidget(lblMessageIndex);
    topBarLayout->addWidget(line3);
    topBarLayout->addWidget(lblFileName);

}
void MainMapWindow::setupLeftMiddleLayout()
{
    leftMiddleWidget = new QWidget();
    QVBoxLayout * leftMiddleLayout = new QVBoxLayout(leftMiddleWidget);
    //leftMiddleWidget->setStyleSheet("border: 2px solid red");
    leftMiddleWidget->setStyleSheet("background-color: #f5f5f5");
    //leftMiddleWidget->setStyleSheet("background-color: green");
    leftMiddleWidget->setMinimumWidth(300);
    leftMiddleWidget->setMaximumWidth(300);

    QWidget * gpsToogleWidget = new QWidget();
    QHBoxLayout * gpsToogleLayout = new QHBoxLayout(gpsToogleWidget);
    //gpsToogleLayout->setSpacing(0);
    gpsToogleLayout->setMargin(0);

    btnToogleGPS = new QPushButton(QIcon(":/resources/images/gps_off.png"), "");
    btnToogleGPS->setIconSize(QSize(80, 80));
    btnToogleGPS->setMinimumSize(80, 80);
    //btnToogleGPS->setStyleSheet("border: none");
    QObject::connect(btnToogleGPS, &QPushButton::clicked, this, &MainMapWindow::toogleGPS);

    btn_toogle_receiving = new QPushButton(QIcon(":/resources/images/rec_off.png"), "");
    btn_toogle_receiving->setIconSize(QSize(80, 80));
    btn_toogle_receiving->setMinimumSize(80, 80);
    //btnToogleGPS->setStyleSheet("border: none");
    QObject::connect(btn_toogle_receiving, &QPushButton::clicked, this, &MainMapWindow::toogleReceivingMessages);

    QWidget * logWidget = new QWidget();
    logWidget->setContentsMargins(0, 8, 0, 0);
    //logWidget->setMaximumHeight(400);
    //logWidget->setStyleSheet("background-color: blue");
    QVBoxLayout * logWidgetLayout = new QVBoxLayout(logWidget);
    logWidgetLayout->setMargin(0);

    // LAST MESSAGE LABEL
    QLabel * lblLastMessage = new QLabel("<strong>Last received message</strong>");
    // LAST MESSAGE HOLDER
    lastMessageLogW = new LogWidget();

    // STATION UNITS LABEL
    QLabel * lblUnitLog = new QLabel("<strong>Station and warning log</strong>");
    // STATION UNITS HODLER
    QScrollArea* techScroll = new QScrollArea();
    techScroll->setBackgroundRole(QPalette::Window);
    techScroll->setFrameShadow(QFrame::Plain);
    techScroll->setFrameShape(QFrame::NoFrame);
    techScroll->setWidgetResizable(true);
    //techScroll->setStyleSheet("background-color: red");

    QWidget * techArea = new QWidget();
    scrollVerticalLayout = new QVBoxLayout(techArea);
    scrollVerticalLayout->setMargin(0);
    scrollVerticalLayout->setSpacing(4);
    scrollVerticalLayout->setAlignment(Qt::AlignTop);
    //techArea->setObjectName("techarea");
    //techArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    techScroll->setWidget(techArea);

    // *** STATUS *** //
    QWidget * logStatusWidget = new QWidget();
    logStatusWidget->setStyleSheet("border: none; border-bottom: 1px solid grey;");
    logStatusWidget->setContentsMargins(0, 0, 0, 8);

    QHBoxLayout * logWidgetStatusLayout = new QHBoxLayout(logStatusWidget);
    logWidgetLayout->setMargin(0);

    GPSstatusLbl = new QLabel("GPS: off");
    GPSstatusLbl->setAlignment(Qt::AlignCenter);
    GPSstatusLbl->setStyleSheet("border: none;");

    receivingStatusLbl = new QLabel("Receiving: off");
    receivingStatusLbl->setAlignment(Qt::AlignCenter);
    receivingStatusLbl->setStyleSheet("border: none;");


    logWidgetStatusLayout->addWidget(GPSstatusLbl);
    logWidgetStatusLayout->addWidget(receivingStatusLbl);
    // *** END OF STATUS *** //

    logWidgetLayout->addWidget(lblLastMessage);
    logWidgetLayout->addWidget(lastMessageLogW);
    logWidgetLayout->addWidget(lblUnitLog);
    logWidgetLayout->addWidget(techScroll);

    gpsToogleLayout->addWidget(btnToogleGPS);
    gpsToogleLayout->addWidget(btn_toogle_receiving);

    leftMiddleLayout->addWidget(gpsToogleWidget);
    leftMiddleLayout->addWidget(logStatusWidget);
    leftMiddleLayout->addWidget(logWidget);
    //leftMiddleLayout->addStretch();
}
void MainMapWindow::setupRightMiddleLayout()
{
    rightMiddleWidget = new QWidget();
    QHBoxLayout * rightMiddleLayout = new QHBoxLayout(rightMiddleWidget);
    //rightMiddleWidget->setStyleSheet("border: 2px solid green");
    rightMiddleLayout->setMargin(0);

    rightMiddleLayout->addWidget(m_map_control);

    // setup map features containers
    QHBoxLayout* layoutH = new QHBoxLayout(m_map_control);
    layoutH->setSpacing(0);

    QWidget * left = new QWidget();
    QWidget * right = new QWidget();
    //left->setStyleSheet("border: 1px solid red");
    //right->setStyleSheet("border: 1px solid red");
    right->setContentsMargins(0, 0, 0, 0);

    // hide widget without changing position of the other elements
    QSizePolicy sp_retain = left->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    left->setSizePolicy(sp_retain);
    left->setVisible(false);

    QVBoxLayout * layoutV = new QVBoxLayout(right);
    layoutV->setMargin(0);

    setupInfoWidget(); // on top
    setupLayoutTrafficLights(); // on bottom

    //layoutV->addWidget(miniMap, 1);
    layoutV->addWidget(trafficLightsW, 1);
    layoutV->addWidget(infoW, 1);

    layoutH->addWidget(left, 2);
    layoutH->addWidget(right, 1);
}
void MainMapWindow::setupInfoWidget()
{
    infoW = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout(infoW);
    infoLayout->setMargin(0);
    //infoLayout->setSpacing(0); // removes spaces between widgets
    //infoLayout->setSizeConstraint(QLayout::SetMinimumSize);

    //infoW->setStyleSheet("border: 1px solid blue");
    infoW->setStyleSheet("background-color: #efefef");
    //infoW->setStyleSheet("background-color: green");

    QSizePolicy sp_retain = infoW->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    infoW->setSizePolicy(sp_retain);

    typeLbl = new QLabel();
    typeLbl->setContentsMargins(5, 0, 0, 0);
    typeLbl->setFixedHeight(30);
    typeLbl->setStyleSheet("border-bottom: 1px solid grey;");

    infoLbl = new QLabel();
    infoLbl->setAlignment(Qt::AlignTop);
    infoLbl->setContentsMargins(5, 0, 0, 0);
    //infoLbl->setStyleSheet("background-color: red");

    vehicleImageLbl = new QLabel();
    vehicleImageLbl->setAlignment(Qt::AlignCenter);
    vehicleImageLbl->setMinimumHeight(150); // for some reason, 150 is a maximum that I can define as minimum
    //vehicleImageLbl->setStyleSheet("background-color: blue");
    vehicleImageLbl->setScaledContents(false);

    QPushButton * closeInfoBtn = new QPushButton(QIcon(":/resources/images/close.png"), "");
    closeInfoBtn->setFixedSize(30, 30);
    closeInfoBtn->setIconSize(QSize(closeInfoBtn->width() - 10, closeInfoBtn->height() - 10));
    closeInfoBtn->setStyleSheet("border: none; border-bottom: 1px solid grey;");
    connect(closeInfoBtn, &QPushButton::clicked, this, &MainMapWindow::toogleInfo);

    QWidget * topW = new QWidget();
    QHBoxLayout* layoutInInfoTop = new QHBoxLayout(topW);
    layoutInInfoTop->setSpacing(0);
    layoutInInfoTop->setMargin(0);
    layoutInInfoTop->addWidget(typeLbl, 5, Qt::AlignTop);
    layoutInInfoTop->addWidget(closeInfoBtn, 1, Qt::AlignTop);

    topW->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    infoLbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    vehicleImageLbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    infoLayout->addWidget(topW);
    infoLayout->addWidget(infoLbl);
    infoLayout->addWidget(vehicleImageLbl);
    infoLayout->setAlignment(Qt::AlignTop);
}
void MainMapWindow::setupLayoutTrafficLights()
{
    trafficLightsW = new QWidget();
    QVBoxLayout* layoutInTrafficLights = new QVBoxLayout(trafficLightsW);
    trafficLightsW->setMinimumSize(260, 270);

    layoutInTrafficLights->setMargin(0);
    layoutInTrafficLights->setSpacing(0);

    QSizePolicy sp_retain = trafficLightsW->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    trafficLightsW->setSizePolicy(sp_retain);

    lblCrossroadName = new QLabel();
    lblCrossroadName->setContentsMargins(5, 0, 0, 0);

    QPushButton * closeTLBtn = new QPushButton(QIcon(":/resources/images/close.png"), "");
    closeTLBtn->setFixedSize(30, 30);
    closeTLBtn->setIconSize(QSize(closeTLBtn->width() - 10, closeTLBtn->height() - 10));
    closeTLBtn->setStyleSheet("border: none; border-bottom: 1px solid grey;");
    connect(closeTLBtn, &QPushButton::clicked, this, &MainMapWindow::closeTLW);

    QWidget * topW = new QWidget();
    QHBoxLayout* layoutInTLTop = new QHBoxLayout(topW);
    topW->setFixedHeight(30);
    topW->setStyleSheet("background-color: #efefef; border-bottom: 1px solid grey;");

    layoutInTLTop->setSpacing(0);
    layoutInTLTop->setMargin(0);
    layoutInTLTop->addWidget(lblCrossroadName, 5);
    layoutInTLTop->addWidget(closeTLBtn, 1, Qt::AlignTop);

    QWidget * bottomW = new QWidget();
    layoutInTLBottom = new QHBoxLayout(bottomW);
    layoutInTLBottom->setSpacing(0);
    layoutInTLBottom->setMargin(0);
    layoutInTLBottom->setContentsMargins(0, 0, 0, 0);
    bottomW->setStyleSheet("background-color: #efefef");
    bottomW->setMinimumHeight(220);

    layoutInTrafficLights->addWidget(topW);
    layoutInTrafficLights->addWidget(bottomW);
}
void MainMapWindow::toogleInfo(bool open)
{
    if(open){
        infoW->setVisible(true);
    } else {
        if(dataHandler->currentInfoStation != nullptr){
            visualizer->removeVehiclePath(dataHandler->currentInfoStation);
        }
        dataHandler->currentInfoStation = nullptr;
        infoW->setVisible(false);
    }
}
void MainMapWindow::closeTLW()
{
    trafficLightsW->setVisible(false);
    removeTrafficLights();
    currentDisplayedStructIndex = -1;
}
void MainMapWindow::openTLW()
{
    trafficLightsW->setVisible(true);
}

void MainMapWindow::hazardClicked(long originatingStationID, int sequenceNumber)
{
    Denm * hazard = dataHandler->getDenmByActionID(originatingStationID, sequenceNumber);

    if(hazard == nullptr){
        return;
    }

    if(dataHandler->currentInfoStation != nullptr){
        // remove path and information from preveiously selected station
        visualizer->removeVehiclePath(dataHandler->currentInfoStation);
        typeLbl->setText("");
        infoLbl->setText("");
        dataHandler->currentInfoStation = nullptr;
    }
    vehicleImageLbl->clear();

    typeLbl->setText("<strong>" + hazard->causeCodeStr + "</strong>");

    QString boxText;

    if(hazard->detectionTime > 1680523524){
        boxText += "Detection time: unknown\n";
    }
    else{
        boxText += "Detection time: " + QString::number(hazard->detectionTime) + "\n";
    }

    boxText += "Specification: " + hazard->subCauseCodeStr + "\n";

    if(hazard->termination){
        boxText += "Has terminated? yes\n";
    }
    else{
       boxText  += "Has terminated? no\n";
    }

    infoLbl->setText(boxText);
    toogleInfo(true);

}

void MainMapWindow::unitClicked(long stationID)
{
    Cam * unit = dataHandler->getCamUnitByID(stationID);

    if(dataHandler->currentInfoStation == nullptr){
        dataHandler->currentInfoStation = unit;
    }
    else{
        // remove path and information from preveiously selected station
        if(dataHandler->currentInfoStation != nullptr){
            visualizer->removeVehiclePath(dataHandler->currentInfoStation);
        }
        typeLbl->setText("");
        infoLbl->setText("");
        vehicleImageLbl->clear();
        dataHandler->currentInfoStation = unit;
    }

    visualizer->drawVehiclePath(dataHandler->currentInfoStation);

    changeInfo();
    toogleInfo(true);
}
void MainMapWindow::changeInfo()
{
    QString boxText;
    QPixmap pixmap;
    QMatrix rm;

    typeLbl->setText("<strong>" + dataHandler->currentInfoStation->typeStr + "</strong>");

    if(dataHandler->currentInfoStation->stationType == 11)
    {
        // Tram
        if(dataHandler->currentInfoStation->leftTurnSignalOn)
        {
            pixmap.load(":/resources/images/tram_top_left.png");
        }
        else if(dataHandler->currentInfoStation->rightTurnSignalOn)
        {
            pixmap.load(":/resources/images/tram_top_right.png");
        }
        else
        {
            pixmap.load(":/resources/images/tram_top.png");
        }
        boxText += "Speed: " + QString::number(dataHandler->currentInfoStation->speed) + " km/h\n";
        boxText += "Length, width: " + QString::number(dataHandler->currentInfoStation->vehicleLength) + ", " +
                QString::number(dataHandler->currentInfoStation->vehicleWidth) + " m\n";
        boxText += "Role: " + dataHandler->currentInfoStation->vehicleRoleStr + "\n";
    }
    else if(dataHandler->currentInfoStation->stationType == 15)
    {
        // RSU
        pixmap.load(":/resources/images/rsu_info.png");
    }
    else
    {
        // anything else
        if(dataHandler->currentInfoStation->leftTurnSignalOn)
        {
            pixmap.load(":/resources/images/car_top_left.png");
        }
        else if(dataHandler->currentInfoStation->rightTurnSignalOn)
        {
            pixmap.load(":/resources/images/car_top_right.png");
        }
        else
        {
            pixmap.load(":/resources/images/car_top.png");
        }
        boxText += "Speed: " + QString::number(dataHandler->currentInfoStation->speed) + " km/h\n";
        boxText += "Length, width: " + QString::number(dataHandler->currentInfoStation->vehicleLength) + ", " +
                QString::number(dataHandler->currentInfoStation->vehicleWidth) + " m\n";
        boxText += "Role: " + dataHandler->currentInfoStation->vehicleRoleStr + "\n";
    }

    boxText += "Last update: " + dataHandler->currentInfoStation->getTimeFormatted(dataHandler->currentInfoStation->timeEpoch, true);

    float angle = dataHandler->currentInfoStation->heading - 270.0f;
    float dimension = 150;

    rm.rotate(angle);
    pixmap = pixmap.scaled(dimension, dimension);
    pixmap = pixmap.transformed(rm, Qt::SmoothTransformation);
    vehicleImageLbl->setPixmap(pixmap);

    infoLbl->setText(boxText);
}

void MainMapWindow::newMessageToLog(std::shared_ptr<Message> message)
{
    // update last message
    lastMessageLogW->setWidgetInfo(message);

    // add or update CAM or MAPEM unit
    QString protocol = message->GetProtocol();

    if(protocol != "Cam" && protocol != "Mapem" && protocol != "Denm"){
        //qInfo() << "Not a good protocol";
        return;
    }

    LogWidget * widget = nullptr;

    if(protocol == "Denm"){
        for (int i = 0; i < scrollVerticalLayout->count(); i++) {
          LogWidget * tmp = (LogWidget*) scrollVerticalLayout->itemAt(i)->widget();

          if(tmp->message->GetProtocol() == "Denm"){
              std::shared_ptr <Denm> oldDenm = std::static_pointer_cast<Denm>(tmp->message);
              std::shared_ptr <Denm> newDenm = std::static_pointer_cast<Denm>(message);

              if (oldDenm->originatingStationID == newDenm->originatingStationID &&
                      oldDenm->sequenceNumber == newDenm->sequenceNumber) {
                  widget = tmp;
                  break;
              }
          }
        }
    } else {
        for (int i = 0; i < scrollVerticalLayout->count(); i++) {
          LogWidget * tmp = (LogWidget*) scrollVerticalLayout->itemAt(i)->widget();
          // if it has the same ID and has the same protocol (for RSUs that send both MAPEM and CAM)
          if (tmp->message->stationID == message->stationID && tmp->message->GetProtocol() == protocol) {
              widget = tmp;
              break;
          }
        }
    }

    // if the widget with the corresponding station number is in the list
    if(widget == nullptr){ // add
        widget = new LogWidget();
        widget->setWidgetInfo(message);
        scrollVerticalLayout->addWidget(widget);
        QObject::connect(widget, &LogWidget::clicked, this, &MainMapWindow::logUnitClicked);
        QObject::connect(widget, &LogWidget::lifeTimeExceeded, this, &MainMapWindow::unitLifeTimeExceeded);
    } else { // update
        widget->setWidgetInfo(message);
    }
}
void MainMapWindow::unitLifeTimeExceeded(std::shared_ptr<Message> message)
{
    QString protocol = message->GetProtocol();

    if(protocol == "Cam"){
        if(dataHandler->currentInfoStation != nullptr){
            if(dataHandler->currentInfoStation->stationID == message->stationID){
                // close info station with that unit
                toogleInfo(false);
            }
        }
        dataHandler->deleteCamUnitByID(message->stationID);
    } else if(protocol == "Mapem"){
        if(currentDisplayedStructIndex >= 0){
            Mapem * crossroad = dataHandler->getMapemByCrossroadID(dataHandler->trafficLightStructVector[currentDisplayedStructIndex].crossroadID);
            if(crossroad->stationID == message->stationID){
                // close Traffic light widget
                closeTLW();
            }
         }
        dataHandler->deleteMapemUnitByID(message->stationID);
    } else if(protocol == "Denm"){
        std::shared_ptr <Denm> hazard = std::static_pointer_cast<Denm>(message);
        dataHandler->deleteDenmMessageByActionID(hazard->originatingStationID, hazard->sequenceNumber);
    }

    deleteLogWidgetByMessage(message);
}
void MainMapWindow::deleteLogWidgetByMessage(std::shared_ptr<Message> message)
{
    QString protocol = message->GetProtocol();

    if(protocol == "Denm"){
        for (int i = 0; i < scrollVerticalLayout->count(); i++) {
            LogWidget * tmp = (LogWidget*) scrollVerticalLayout->itemAt(i)->widget();

            if(tmp->message->GetProtocol() == "Denm"){
                std::shared_ptr <Denm> oldDenm = std::static_pointer_cast<Denm>(tmp->message);
                std::shared_ptr <Denm> newDenm = std::static_pointer_cast<Denm>(message);

                if (oldDenm->originatingStationID == newDenm->originatingStationID &&
                      oldDenm->sequenceNumber == newDenm->sequenceNumber) {
                    scrollVerticalLayout->removeWidget(tmp);
                    tmp->timerRunning = false;
                    tmp->setVisible(false);
                    delete tmp;
                    break;
                }
            }
        }
    } else {
        for (int i = 0; i < scrollVerticalLayout->count(); i++) {
          LogWidget * tmp = (LogWidget*) scrollVerticalLayout->itemAt(i)->widget();
          if (tmp->message->stationID == message->stationID && tmp->message->GetProtocol() == protocol) {
              scrollVerticalLayout->removeWidget(tmp);
              tmp->timerRunning = false;
              tmp->setVisible(false);
              delete tmp;
              break;
          }
        }
    }
}

void MainMapWindow::deleteLogWidgets()
{
    lastMessageLogW->setWidgetInfo(nullptr);

    QLayoutItem *child;
    while ((child = scrollVerticalLayout->takeAt(0)) != 0)  {
        LogWidget * tmp = (LogWidget*) child->widget();
        scrollVerticalLayout->removeWidget(tmp);
        tmp->setVisible(false);
        tmp->timerRunning = false;
        delete tmp;
        delete child;
    }
}
void MainMapWindow::logUnitClicked(std::shared_ptr <Message> message)
{
    QString protocol = message->GetProtocol();

    if(protocol != "Cam" && protocol != "Mapem" && protocol != "Denm"){
        //qInfo() << "Not a good protocol";
        return;
    }

    m_map_control->setMapFocusPoint(PointWorldCoord(message->longitude, message->latitude));

}
void MainMapWindow::playingStarted()
{
    visualizer->removeAllGeometries(false);
    dataHandler->currentInfoStation = nullptr;
    dataHandler->clearUnitsAndMessages();

    infoW->setVisible(false);
    lblMessageIndex->setText("Messages: 0");

    deleteLogWidgets();
}
void MainMapWindow::messageEmitted(int index){
    lblMessageIndex->setText("Message: " + QString::number(index + 1));
}
void MainMapWindow::resetPlaying()
{
    eventCounter.reset();

    visualizer->removeAllGeometries(false);
    dataHandler->currentInfoStation = nullptr;
    dataHandler->clearUnitsAndMessages();

    infoW->setVisible(false);
    lblMessageIndex->setText("Messages: 0");

    deleteLogWidgets();

}
void MainMapWindow::messagesPlayed()
{
    statusBar()->showMessage("Messages have been played...");
    btn_play->setIcon(QIcon(":/resources/images/play.png"));
}
void MainMapWindow::tooglePlay()
{
    if(!eventCounter.isRunning()){
        if(eventCounter.areMessagesPlayed()){
            // check if the mode was switched and
            if(eventCounter.messagesNotSet() && !eventCounter.newPlayingCycle){
               statusBar()->showMessage("You have to load a file...");
               return;
            }
            // play messages
            eventCounter.newPlayingCycle = false;
            eventCounter.start();
            statusBar()->showMessage("Playing messages...");
        }
        //btn_play->setText("Stop playing");
        btn_play->setIcon(QIcon(":/resources/images/pause.png"));
        eventCounter.start();
    }
    else {
        //btn_play->setText("Continue playing");
        btn_play->setIcon(QIcon(":/resources/images/play.png"));
        eventCounter.stop();
    }

}
void MainMapWindow::playNextMessage()
{
    if(!eventCounter.isRunning()){
        if(eventCounter.areMessagesPlayed()){
            if(eventCounter.messagesNotSet() && !eventCounter.newPlayingCycle){
               statusBar()->showMessage("You have to load a file...");
               return;
            }
        }
    }
    eventCounter.tick();
}

void MainMapWindow::resizeEvent(QResizeEvent * resize_event)
{
    // Set the new viewport size.
    m_map_control->setViewportSize(resize_event->size());
    //miniMap->setViewportSize(resize_event->size());
}

void MainMapWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "/home/krivmi/Desktop/", tr("Traffic Files (*.pcap *.pcapng)"));
    // TODO - delete /home/krivmi/Desktop/ at the end

    if(!fileName.isEmpty()){
        qInfo() << fileName;
        processHandler.fileChanged = true;
        processHandler.currentFile = fileName;

        // clear data from playing
        eventCounter.reset();
        eventCounter.setMessageSize(0);
        eventCounter.newPlayingCycle = true;

        // display info status to user
        lblFileName->setText("Current file: " + getFileNameFromPath(fileName));
        lblMessageIndex->setText("Message: 0");

        // delete all stored messages
        dataHandler->deleteAllMessages();

        // load messages here
        statusBar()->showMessage("Loading messages...");
        processHandler.startLoading();
        eventCounter.setMessageSize(dataHandler->allMessages.size());

        // delete messages so it can be sent again TODO - change
        visualizer->removeAllGeometries(false);
        dataHandler->clearUnitsAndMessages();

        // delete log widgets
        deleteLogWidgets();
        statusBar()->showMessage("Messages loaded, ready for playing...");
    }

}
QString MainMapWindow::getFileNameFromPath(QString path)
{
    QRegExp rx("(\\/)"); //RegEx for ' ' or ',' or '.' or ':' or '\t'
    QStringList query = path.split(rx);

    return query.back();
}

void MainMapWindow::trafficLightClick(int crossroadID, int adjacentIngressLanesIndex, bool sameTrafficLight)
{
    Mapem * crossroad = dataHandler->getMapemByCrossroadID(crossroadID);

    if(crossroad == nullptr){
        qInfo() << "Could not find corresponding crossroad after click";
        return;
    }
    this->lblCrossroadName->setText("<strong>" + crossroad->crossroadName + "</strong>");
    this->removeTrafficLights();
    this->openTLW();

    for(int i = 0; i < dataHandler->trafficLightStructVector.size(); i++){
        if(dataHandler->trafficLightStructVector[i].crossroadID == crossroad->crossroadID &&
           dataHandler->trafficLightStructVector[i].adjacentIngressLaneIndex == adjacentIngressLanesIndex){

            for(int j = 0; j < dataHandler->trafficLightStructVector[i].widgets.size(); j++){
                layoutInTLBottom->addWidget(dataHandler->trafficLightStructVector[i].widgets[j]);
                dataHandler->trafficLightStructVector[i].widgets[j]->setVisible(true);
                //qInfo() << "add: " << dataHandler->trafficLightStructVector[i].widgets[j];
            }
            //qInfo() << "";
            currentDisplayedStructIndex = i;

            break;
        }
    }


}
void MainMapWindow::removeTrafficLights()
{
    if(currentDisplayedStructIndex < 0){
        return;
    }

    for(int i = 0; i < dataHandler->trafficLightStructVector[currentDisplayedStructIndex].widgets.size(); i++){
        layoutInTLBottom->removeWidget(dataHandler->trafficLightStructVector[currentDisplayedStructIndex].widgets[i]);
        dataHandler->trafficLightStructVector[currentDisplayedStructIndex].widgets[i]->setVisible(false);
        //qInfo() << "delete: " << dataHandler->trafficLightStructVector[currentDisplayedStructIndex].widgets[i];
    }
}

void MainMapWindow::recenterGPSPoint()
{
    if(!gpsEnabled){
        statusBar()->showMessage("You have to start GPS...");
        //qInfo() << "You have to start GPS";
        return;
    }
    if(dataHandler->gpsInfo.latitude != -1 && dataHandler->gpsInfo.longitude != -1){        
        m_map_control->setMapFocusPoint(PointWorldCoord(dataHandler->gpsInfo.longitude, dataHandler->gpsInfo.latitude));

        if(dataHandler->autoModeOn){
            toggleFollowGPS(true);
        }
    } else {
        statusBar()->showMessage("No valid data...");
    }
}

void MainMapWindow::toggleFollowGPS(bool enable)
{
    // Should we follow the GPS Modul point?
    if(enable){
        m_map_control->followGeometry(visualizer->GPSpositionPoint);
        statusBar()->showMessage("Following geometry...");
    } else {
        m_map_control->stopFollowingGeometry();
        statusBar()->showMessage("Following geometry was stopped...");
    }
}

MainMapWindow::~MainMapWindow(){
    tracker->stop();

    gpsThread.quit();
    gpsThread.wait();
};
