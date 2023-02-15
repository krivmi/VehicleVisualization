#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <sys/time.h>
#include <QQueue>

#include "visualizer.h"
#include "message.h"
#include "cam.h"
#include "spatem.h"
#include "mapem.h"
#include "srem.h"
#include "denm.h"
#include "trafficlightwidget.h"

struct GPSInfo {
    float longitude;
    float latitude;
    float orientation;
    struct timeval lastUpdate;
};

struct TrafficLightStruct{
    int crossroadID;
    int adjacentIngressLaneIndex;
    bool sameSignalGroupForWidgets;

    QVector<TrafficLightWidget*> widgets;
};

class DataHandler : public QObject
{
    Q_OBJECT
public:
    DataHandler(Visualizer * visualizer, QWidget* parent = nullptr);
    ~DataHandler();

    Message * getMessageByID(long id);
    Cam * getCamUnitByID(long id);
    Mapem * getMapemUnitByID(long id);
    Mapem * getMapemByCrossroadID(long id);
    Spatem * findSpatemByTime(int moy, int timeStamp);
    Denm * getDenmByTimeAndCode(long detectionTime, int causeCode);

    void addTrafficLightsFromSignalGroups(Mapem * crossroad);
    void createTrafficLightWidgets(int crossroadID, int adjacentLaneIndex, bool sameTrafficLight);

    void addCamUnit(std::shared_ptr<Cam> unit);
    void addMapemUnit(std::shared_ptr<Mapem> unit);
    void addDenmMessage(std::shared_ptr<Denm> message);
    void addMessage(std::shared_ptr<Message> message);

    void deleteCamUnits();
    void deleteMapemUnits();
    void deleteSpatemMessages();
    void deleteDenmMessages();
    void deleteAllMessages();
    void clearData();

    void deleteCamUnitByID(long id);
    void deleteMapemUnitByID(long id);
    void deleteDenmUnitByTimeAndCode(long detectionTime, int causeCode);

    QVector <std::shared_ptr<Message>> allMessages;
    QVector <std::shared_ptr<Cam>> camUnits;
    QVector <std::shared_ptr<Mapem>> mapemUnits;
    QVector <std::shared_ptr<Spatem>> spatemMessages;
    QVector <std::shared_ptr<Denm>> denmMessages;

    Cam * currentInfoStation = nullptr;
    Mapem * currentCrossroad = nullptr;
    bool trafficLightShown = false;

    QVector <TrafficLightStruct> trafficLightStructVector;

    GPSInfo gpsInfo;

    bool autoModeOn = true;

    void GPSPositionReceived(PointWorldCoord position, float orientation);

    Mapem * getClosestCrossroad();

signals:
    void trafficLightShow(int crossroadID, int adjacentIngressLanesIndex, bool sameTrafficLight);
    void trafficLightHide();
    void changeInfo(long stationID);
    void MessageToLog(std::shared_ptr<Message> message);

public slots:
    void MessageReceived(std::shared_ptr<Message> message);
    void CAMReceived(std::shared_ptr<Cam> newCam);
    void MAPEMReceived(std::shared_ptr<Mapem> newMAPEM);
    void SPATEMReceived(std::shared_ptr<Spatem> newSPATEM);
    void SREMReceived(std::shared_ptr<Srem> newSREM);
    void DENMReceived(std::shared_ptr<Denm> newDENM);
    void messagePlay(int index);
    void handleCrossroadProximity();
    void updateCamBySREM();
private:
    Visualizer * visualizer;
    QQueue <GPSInfo> gpsHistory;
};

#endif // DATAHANDLER_H
