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

using namespace qmapcontrol;

class MainMapWindow : public QMainWindow
{
    Q_OBJECT
    QThread gpsThread;
public:
    MainMapWindow(QWidget *parent = 0);
    ~MainMapWindow(){
        tracker->stop();

        gpsThread.quit();
        gpsThread.wait();
    };

    void toggleFollowGPS(bool enable);

public slots:
    void resizeEvent(QResizeEvent* resize_event);

    void playMessages();
    void resetPlaying();
    void tooglePlay();
    void playNextMessage();
    void messagesPlayed();

    void openTLW();
    void closeTLW();
    void changeInfo();
    void unitClicked(long stationID);
    void hazardClicked(long time, int code);
    void trafficLightClick(int crossroadID, int adjacentIngressLanesIndex, bool sameTrafficLight);
    void handleGPSData(float longitude, float latitude);
    void startReceivingMessages();
    void newMessageToLog(std::shared_ptr<Message> message);
    void toogleGPS();
    void toogleInfo(bool open);
    void recenterGPSPoint();
    void modeSelected(QAction* action);
    void logUnitClicked(std::shared_ptr<Message> message);
    void unitLifeTimeExceeded(std::shared_ptr<Message> message);

signals:
    void startTracker();
    void GPSPositionReceived(PointWorldCoord position);
private:    
    ProcessHandler processHandler;
    EventCounter eventCounter;

    std::shared_ptr <Visualizer> visualizer;
    std::shared_ptr <DataHandler> dataHandler;

    GPSTracker * tracker; // must be normal pointer, it is deleted when the thread finishes

    QMapControl * m_map_control;
    QMapControl * miniMap;
    QPushButton * btn_play;
    QPushButton * btn_nextMsg;
    QPushButton * btn_resetPlaying;
    QPushButton * btn_start_receiving;
    QPushButton * btnToogleGPS;
    QPushButton * btnFollowGeometry;
    QTextEdit * infoTe;
    QLabel * typeTe;
    QLabel * lb_file;

    QVBoxLayout * logWidgetUnitsLayout;
    QVBoxLayout * mainAppLayout;
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
    QLabel * lblCrossroadName;
    int currentDisplayedStructIndex = -1;

    bool gpsEnabled = false;
    bool gpsRecentered = false;

    QAction * mode_manual;
    QAction * mode_auto;

    LogWidget * lastMessageLogW;
    QVBoxLayout * scrollVerticalLayout;

    void setupTopMenu();
    void setupTopBar();
    void setupMainLayout();
    void setupLeftMiddleLayout();
    void setupRightMiddleLayout();
    void setupMiddleSection();
    void setupInfoWidget();

    void removeTrafficLights();

    void deleteLogWidgets();
    void deleteLogWidgetByID(long id);

    void positionChanged(const PointWorldCoord& point);
    void setupLayoutTrafficLights();
    void setupMaps();
    void openFile();
    QString getFileNameFromPath(QString path);
};
