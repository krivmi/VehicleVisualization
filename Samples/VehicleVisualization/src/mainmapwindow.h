#pragma once

#include "message.h"
#include "eventcounter.h"
#include "messageparser.h"
#include "processhandler.h"
#include "visualizer.h"
#include "datahandler.h"
#include "gpstracker.h"
#include "trafficlightwidget.h"
#include "logwidget.h"

// Qt includes.
#include <QApplication>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QThread>
#include <QVector>
#include <QLabel>

// QMapControl includes.
#include <QMapControl/QMapControl.h>
#include <QMapControl/GeometryLineString.h>
#include <QMapControl/GeometryPointCircle.h>
#include <QMapControl/GeometryWidget.h>
#include <QMapControl/Layer.h>
#include <QMapControl/LayerGeometry.h>

class MainMapWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainMapWindow(QWidget *parent = 0);
    ~MainMapWindow();

    void toggleFollowGPS(bool enable);

    QThread gpsThread;

public slots:
    void resizeEvent(QResizeEvent* resize_event);

    void resetPlaying();
    void tooglePlay();
    void playNextMessage();
    void playingStarted();
    void messagesPlayed();
    void messageEmitted(int index);

    void handleError(QString error);
    void focusPointChanged();
    void openTLW();
    void closeTLW();
    void changeInfo();
    void unitClicked(long stationID);
    void hazardClicked(long originationgStationID, int sequenceNumber);
    void trafficLightClick(int crossroadID, int adjacentIngressLanesIndex, bool sameTrafficLight);
    void handleGPSData(float longitude, float latitude, float orientation);
    void GPSstopped();
    void toogleReceivingMessages();
    void newMessageToLog(std::shared_ptr<Message> message);
    void toogleGPS();
    void toogleInfo(bool open);
    void recenterGPSPoint();
    void modeSelected(QAction* action);
    void logUnitClicked(std::shared_ptr<Message> message);
    void unitLifeTimeExceeded(std::shared_ptr<Message> message);

signals:
    void startTracker();
    void GPSPositionReceived(PointWorldCoord position, float orientation);
    void changeGPSButtonGeometry(int w, int h, QSize size);

private:    
    void changeFont(int size);
    void initSettingsDialog();
    void initSettings();

    void setupTopMenu();
    void setupTopBar();
    void setupMainLayout();
    void setupLeftMiddleLayout();
    void setupRightMiddleLayout();
    void setupMiddleSection();
    void setupInfoWidget();

    void removeTrafficLights();

    void deleteLogWidgets();
    void deleteLogWidgetByMessage(std::shared_ptr<Message> message);

    void positionChanged(const PointWorldCoord& point);
    void setupLayoutTrafficLights();
    void setupMaps();
    void openFile();
    QString getFileNameFromPath(QString path);

    ProcessHandler processHandler;
    EventCounter eventCounter;

    std::shared_ptr <Visualizer> visualizer;
    std::shared_ptr <DataHandler> dataHandler;

    GPSTracker * tracker; // must be normal pointer, it is deleted when the thread is finished

    QMapControl * m_map_control;

    QPushButton * btn_play;
    QPushButton * btn_nextMsg;
    QPushButton * btn_resetPlaying;
    QPushButton * btn_toogle_receiving;
    QPushButton * btnToogleGPS;
    QPushButton * btnFollowGeometry;

    QLabel * infoLbl;
    QLabel * typeLbl;
    QLabel * vehicleImageLbl;
    QLabel * lblFileName;
    QLabel * lblMessageIndex;
    QLabel * GPSstatusLbl;
    QLabel * receivingStatusLbl;
    QLabel * lblCrossroadName;

    QVBoxLayout * mainAppLayout;
    QVBoxLayout * logWidgetUnitsLayout;
    QVBoxLayout * scrollVerticalLayout;
    QHBoxLayout * middleLayout;
    QHBoxLayout * rightMiddleLayout; // layoutH
    QHBoxLayout * layoutInTLBottom;

    QWidget * topBar;
    QWidget * middleWidget;
    QWidget * leftMiddleWidget;
    QWidget * rightMiddleWidget;
    QWidget * crossroadW;
    QWidget * infoW;
    QWidget * trafficLightsW;

    LogWidget * lastMessageLogW;

    QAction * mode_manual;
    QAction * mode_auto;

    bool gpsEnabled = false;
    bool receivingEnabled = false;
    bool gpsRecentered = false;
    bool fontLarge = false;
    int currentDisplayedStructIndex = -1;
};
