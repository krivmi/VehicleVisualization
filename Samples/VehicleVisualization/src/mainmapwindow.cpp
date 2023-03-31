#include "mainmapwindow.h"
#include "dialogs.h"
#include "cam.h"
#include "mapem.h"
#include "messageparser.h"
#include "processhandler.h"
#include "eventcounter.h"
#include "visualizer.h"

// Qt includes.
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QScrollArea>

// QMapControl includes.
#include <QMapControl/GeometryLineString.h>
#include <QMapControl/GeometryPointCircle.h>
#include <QMapControl/GeometryPointImage.h>
#include <QMapControl/GeometryPointImageScaled.h>
#include <QMapControl/GeometryPolygon.h>
#include <QMapControl/LayerGeometry.h>
#include <QMapControl/LayerMapAdapter.h>
#include <QMapControl/MapAdapterGoogle.h>
#include <QMapControl/MapAdapterOSM.h>

MainMapWindow::MainMapWindow(QWidget *parent) : QMainWindow(parent) {

    setupMaps(); // Setup the maps
    setupTopMenu();
    setupMainLayout();

    QObject::connect(m_map_control, &QMapControl::recenterGPSPoint, this, &MainMapWindow::recenterGPSPoint);
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

    // connect message parsed event
    QObject::connect(&MessageParser::getInstance(), &MessageParser::messageParsed, dataHandler.get(), &DataHandler::MessageReceived);

    QObject::connect(this, &MainMapWindow::GPSPositionReceived, dataHandler.get(), &DataHandler::GPSPositionReceived);

    QObject::connect(&eventCounter, &EventCounter::messageShow, dataHandler.get(), &DataHandler::messagePlay);
    QObject::connect(&eventCounter, &EventCounter::messageShow, this, &MainMapWindow::messageEmitted);
    QObject::connect(&eventCounter, &EventCounter::messagesPlayed, this, &MainMapWindow::messagesPlayed);
    QObject::connect(&eventCounter, &EventCounter::playingStarted, this, &MainMapWindow::playingStarted);

    tracker = new GPSTracker();
    tracker->moveToThread(&gpsThread);

    QObject::connect(&gpsThread, &QThread::finished, tracker, &QObject::deleteLater);
    QObject::connect(this, &MainMapWindow::startTracker, tracker, &GPSTracker::start);
    QObject::connect(tracker, &GPSTracker::resultReady, this, &MainMapWindow::handleGPSData);
    QObject::connect(tracker, &GPSTracker::GPSstopped, this, &MainMapWindow::GPSstopped);
    gpsThread.start();

    QObject::connect(&processHandler, &ProcessHandler::error, this, &MainMapWindow::handleError);

    processHandler.startLoading();
    eventCounter.setMessageSize(dataHandler->allMessages.size());
}
void MainMapWindow::GPSstopped(){
    gpsEnabled = false;
    btnToogleGPS->setIcon(QIcon(":/resources/images/gps_off.png"));
    visualizer->removeGPSPositionPoint();
    statusBar()->showMessage("No GPSD running, GPS could not be started...");
}
void MainMapWindow::focusPointChanged(){
    if(dataHandler->autoModeOn){
        toggleFollowGPS(false);
    }
}
void MainMapWindow::handleError(QString error){
    statusBar()->showMessage("Receiving could not be started, error: " + error);
}
void MainMapWindow::handleGPSData(float longitude, float latitude, float orientation){
    PointWorldCoord GPSPosition = PointWorldCoord(longitude, latitude);
    emit GPSPositionReceived(GPSPosition, orientation);
}
void MainMapWindow::toogleReceivingMessages(){
    if(!receivingEnabled){
        if(processHandler.startReceiving() == 0){
            statusBar()->showMessage("Receiving started...");
            receivingEnabled = true;
            btn_toogle_receiving->setText("Stop receiving");
        }
    } else {
        receivingEnabled = false;
        btn_toogle_receiving->setText("Start receiving");

        processHandler.stopReceiving();
        statusBar()->showMessage("Receiving stopped...");
    }
}
void MainMapWindow::setupMaps(){
    m_map_control = new QMapControl(QSizeF(1280.0f, 640.0f));
    //m_map_control = new QMapControl(QSizeF(1520.0, 740.0));

    // layer pro openstreet mapy
    m_map_control->addLayer(std::make_shared<LayerMapAdapter>("Map", std::make_shared<MapAdapterOSM>()));
    //m_map_control->addLayer(std::make_shared<LayerMapAdapter>("Map", std::make_shared<MapAdapterGoogle>()));

    // nastavení bodu zaměření a přiblížení na Ostravu
    m_map_control->setMapFocusPoint(PointWorldCoord(18.284743, 49.838337));
    m_map_control->setZoom(18);
}
void MainMapWindow::toogleGPS()
{
    if(!gpsEnabled){
        gpsEnabled = true;
        btnToogleGPS->setIcon(QIcon(":/resources/images/gps_on.png"));
        emit startTracker();
        statusBar()->showMessage("GPS started...");
    } else {
        gpsEnabled = false;
        btnToogleGPS->setIcon(QIcon(":/resources/images/gps_off.png"));
        visualizer->removeGPSPositionPoint();
        tracker->stop();
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
    }
    else if( mode_auto )
    {
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

    // *** Connects *** /
    QObject::connect(mode_group, &QActionGroup::triggered, this, &MainMapWindow::modeSelected);
    QObject::connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    QObject::connect(load, &QAction::triggered, this, &MainMapWindow::openFile);
    QObject::connect(maximize, &QAction::triggered, this, [=]() { this->showMaximized();});
    QObject::connect(minimize, &QAction::triggered, this, [=]() { this->showMinimized();});
    QObject::connect(fullscreen, &QAction::triggered, this, [=]() { (!this->isFullScreen()) ? this->showFullScreen() : this->showNormal();});
}

void MainMapWindow::setupMainLayout(){
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
void MainMapWindow::setupMiddleSection(){
    middleWidget = new QWidget();
    middleLayout = new QHBoxLayout(middleWidget);
    //middleWidget->setStyleSheet("border: 2px solid orange");

    setupLeftMiddleLayout();
    setupRightMiddleLayout();

    middleLayout->addWidget(leftMiddleWidget, 2);
    middleLayout->addWidget(rightMiddleWidget, 3);
}
void MainMapWindow::setupTopBar(){
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
void MainMapWindow::setupLeftMiddleLayout(){

    leftMiddleWidget = new QWidget();
    QVBoxLayout * leftMiddleLayout = new QVBoxLayout(leftMiddleWidget);
    //leftMiddleWidget->setStyleSheet("border: 2px solid red");
    leftMiddleWidget->setStyleSheet("background-color: #f5f5f5");
    //leftMiddleWidget->setStyleSheet("background-color: green");
    leftMiddleWidget->setMinimumWidth(260);
    leftMiddleWidget->setMaximumWidth(280);

    QWidget * gpsToogleWidget = new QWidget();
    QHBoxLayout * gpsToogleLayout = new QHBoxLayout(gpsToogleWidget);
    //gpsToogleLayout->setSpacing(0);
    gpsToogleLayout->setMargin(0);

    btnToogleGPS = new QPushButton(QIcon(":/resources/images/gps_off.png"), "");
    btnToogleGPS->setIconSize(QSize(64, 64));
    btnToogleGPS->setMinimumSize(80, 80);
    //btnToogleGPS->setStyleSheet("border: none");
    QObject::connect(btnToogleGPS, &QPushButton::clicked, this, &MainMapWindow::toogleGPS);

    btn_toogle_receiving = new QPushButton("Start\nreceiving");
    btn_toogle_receiving->setMinimumSize(80, 80);
    //btn_start_receiving->setMaximumHeight(20);
    btn_toogle_receiving->setFont(QFont("Verdana", 12));
    QObject::connect(btn_toogle_receiving, &QPushButton::clicked, this, &MainMapWindow::toogleReceivingMessages);

    QWidget * logWidget = new QWidget();
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

    logWidgetLayout->addWidget(lblLastMessage);
    logWidgetLayout->addWidget(lastMessageLogW);
    logWidgetLayout->addWidget(lblUnitLog);
    logWidgetLayout->addWidget(techScroll);

    gpsToogleLayout->addWidget(btnToogleGPS);
    gpsToogleLayout->addWidget(btn_toogle_receiving);

    leftMiddleLayout->addWidget(gpsToogleWidget);
    leftMiddleLayout->addWidget(logWidget);
    //leftMiddleLayout->addStretch();
}
void MainMapWindow::setupRightMiddleLayout(){

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
void MainMapWindow::setupInfoWidget(){
    infoW = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout(infoW);
    infoW->setMinimumSize(260, 200);
    infoLayout->setMargin(0);
    infoLayout->setSpacing(0); // removes spaces between widgets

    //infoW->setStyleSheet("border: 1px solid blue");
    infoW->setStyleSheet("background-color: #efefef");

    QSizePolicy sp_retain = infoW->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    infoW->setSizePolicy(sp_retain);
    /*
    sp_retain = miniMap->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    miniMap->setSizePolicy(sp_retain);
    miniMap->hide();*/
    //miniMap->setStyleSheet("border: 1px solid blue");

    typeTe = new QLabel();
    typeTe->setContentsMargins(5, 0, 0, 0);
    infoTe = new QTextEdit();

    QPushButton * closeInfoBtn = new QPushButton(QIcon(":/resources/images/close.png"), "");
    closeInfoBtn->setFixedSize(30, 30);
    closeInfoBtn->setIconSize(QSize(closeInfoBtn->width() - 10, closeInfoBtn->height() - 10));
    closeInfoBtn->setStyleSheet("border: none");
    connect(closeInfoBtn, &QPushButton::clicked, this, &MainMapWindow::toogleInfo);

    QWidget * topW = new QWidget();
    QHBoxLayout* layoutInInfoTop = new QHBoxLayout(topW);
    layoutInInfoTop->setSpacing(0);
    layoutInInfoTop->setMargin(0);
    layoutInInfoTop->addWidget(typeTe, 5);
    layoutInInfoTop->addWidget(closeInfoBtn, 1, Qt::AlignTop);

    infoLayout->addWidget(topW);
    infoLayout->addWidget(infoTe);
}
void MainMapWindow::setupLayoutTrafficLights(){
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
void MainMapWindow::toogleInfo(bool open){
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
void MainMapWindow::closeTLW(){
    trafficLightsW->setVisible(false);
    removeTrafficLights();
    currentDisplayedStructIndex = -1;
}
void MainMapWindow::openTLW(){
    trafficLightsW->setVisible(true);
}

void MainMapWindow::hazardClicked(long originatingStationID, int sequenceNumber){
    Denm * hazard = dataHandler->getDenmByActionID(originatingStationID, sequenceNumber);

    if(hazard == nullptr){
        return;
    }

    if(dataHandler->currentInfoStation != nullptr){
        // remove path and information from preveiously selected station
        visualizer->removeVehiclePath(dataHandler->currentInfoStation);
        infoTe->setText("");
        typeTe->setText("");
        dataHandler->currentInfoStation = nullptr;
    }

    QString boxText;
    // show information about new station

    typeTe->setText("<strong>" + hazard->causeCodeStr + "</strong>");
    boxText += "Detection time: " + QString::number(hazard->detectionTime) + "\n";
    boxText += "Specification: " + hazard->subCauseCodeStr + "\n";

    infoTe->setText(boxText);
    toogleInfo(true);

}

void MainMapWindow::unitClicked(long stationID){

    Cam * unit = dataHandler->getCamUnitByID(stationID);

    if(dataHandler->currentInfoStation == nullptr){
        dataHandler->currentInfoStation = unit;
    }
    else{
        // remove path and information from preveiously selected station
        if(dataHandler->currentInfoStation != nullptr){
            visualizer->removeVehiclePath(dataHandler->currentInfoStation);
        }
        infoTe->setText("");
        typeTe->setText("");
        dataHandler->currentInfoStation = unit;
    }

    visualizer->drawVehiclePath(dataHandler->currentInfoStation);

    changeInfo();
    toogleInfo(true);
}
void MainMapWindow::changeInfo(){
    QString boxText;
    // show information about new station

    typeTe->setText("<strong>" + dataHandler->currentInfoStation->typeStr + "</strong>");
    boxText += "Speed: " + QString::number(dataHandler->currentInfoStation->speed) + " km/h\n";
    boxText += "Heading: " + QString::number(dataHandler->currentInfoStation->heading) + "˚\n";
    boxText += "Vehicle length: " + QString::number(dataHandler->currentInfoStation->vehicleLength) + "m\n";
    boxText += "Vehicle width: " + QString::number(dataHandler->currentInfoStation->vehicleWidth) + "m\n";
    boxText += "Vehicle role: " + dataHandler->currentInfoStation->vehicleRoleStr + "\n";
    boxText += "Left turn signal on: " + QString::number(dataHandler->currentInfoStation->leftTurnSignalOn) + "\n";
    boxText += "Right turn signal on: " + QString::number(dataHandler->currentInfoStation->rightTurnSignalOn) + "\n";
    boxText += "Position: " + QString::number(dataHandler->currentInfoStation->longitude, 'f', 7) + ", " + QString::number(dataHandler->currentInfoStation->latitude, 'f', 7) + "\n";
    boxText += "Last update: " + dataHandler->currentInfoStation->getTimeFormatted(dataHandler->currentInfoStation->timeEpoch) + "\n";

    infoTe->setText(boxText);
}

void MainMapWindow::newMessageToLog(std::shared_ptr<Message> message){

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
void MainMapWindow::unitLifeTimeExceeded(std::shared_ptr<Message> message){

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
void MainMapWindow::deleteLogWidgetByMessage(std::shared_ptr<Message> message){

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

void MainMapWindow::deleteLogWidgets(){

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
void MainMapWindow::logUnitClicked(std::shared_ptr <Message> message){
    QString protocol = message->GetProtocol();

    if(protocol != "Cam" && protocol != "Mapem" && protocol != "Denm"){
        //qInfo() << "Not a good protocol";
        return;
    }

    m_map_control->setMapFocusPoint(PointWorldCoord(message->longitude, message->latitude));

}
void MainMapWindow::playingStarted(){
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
void MainMapWindow::resetPlaying(){
    eventCounter.reset();

    visualizer->removeAllGeometries(false);
    dataHandler->currentInfoStation = nullptr;
    dataHandler->clearUnitsAndMessages();

    infoW->setVisible(false);
    lblMessageIndex->setText("Messages: 0");

    deleteLogWidgets();

}
void MainMapWindow::messagesPlayed(){
    statusBar()->showMessage("Messages have been played...");
    btn_play->setIcon(QIcon(":/resources/images/play.png"));
}
void MainMapWindow::tooglePlay(){
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
void MainMapWindow::playNextMessage(){
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

void MainMapWindow::openFile(){
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "/home/krivmi/Desktop/", tr("Traffic Files (*.pcap *.pcapng)"));
    // TODO - delete /home/krivmi/Desktop/ at the end

    if(!fileName.isEmpty()){
        qInfo() << fileName;
        processHandler.fileChanged = true;
        processHandler.currentFile = fileName;
/*
        // clear data from playing
        eventCounter.reset();
        eventCounter.setMessageSize(0);
        eventCounter.newPlayingCycle = true;

        // clear data on canvas
        visualizer->removeAllGeometries(false);

        dataHandler->clearData();
        deleteLogWidgets();

        lblFileName->setText("Current file: " + getFileNameFromPath(fileName));
        lblMessageIndex->setText("Message: 0");

        // load messages here
        statusBar()->showMessage("Loading messages...");
        processHandler.startLoading();
        eventCounter.setMessageSize(dataHandler->allMessages.size());
        visualizer->removeAllGeometries(false);
        dataHandler->deleteCamUnits();
        dataHandler->deleteMapemUnits();
        dataHandler->deleteSpatemMessages();
        dataHandler->deleteDenmMessages();
        deleteLogWidgets();
        statusBar()->showMessage("Messages loaded...");

        statusBar()->showMessage("Ready for playing...");
        */

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
QString MainMapWindow::getFileNameFromPath(QString path){
    QRegExp rx("(\\/)"); //RegEx for ' ' or ',' or '.' or ':' or '\t'
    QStringList query = path.split(rx);

    return query.back();
}

void MainMapWindow::trafficLightClick(int crossroadID, int adjacentIngressLanesIndex, bool sameTrafficLight){

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
void MainMapWindow::removeTrafficLights(){
    if(currentDisplayedStructIndex < 0){
        return;
    }

    for(int i = 0; i < dataHandler->trafficLightStructVector[currentDisplayedStructIndex].widgets.size(); i++){
        layoutInTLBottom->removeWidget(dataHandler->trafficLightStructVector[currentDisplayedStructIndex].widgets[i]);
        dataHandler->trafficLightStructVector[currentDisplayedStructIndex].widgets[i]->setVisible(false);
        //qInfo() << "delete: " << dataHandler->trafficLightStructVector[currentDisplayedStructIndex].widgets[i];
    }
}

void MainMapWindow::recenterGPSPoint(){

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
