#pragma once

#include "message.h"

#include "QVector"
#include <QMap>

#include <QMapControl/QMapControl.h>
#include <QMapControl/GeometryLineString.h>
#include <QMapControl/GeometryPoint.h>

struct SignalGroupInfo {
    long laneID;
    long connectingLaneID;
    QString manouver;
};

struct Node {
    int x, y, delta;
};

struct ConnectingLane {
    int laneNumber;
    int connectionID; // needed only for some special technologies
    int signalGroup;

    bool maneuverStraightAllowed;
    bool maneuverLeftAllowed;
    bool maneuverRightAllowed;
};

struct Lane {
    int type;
    QString strType;
    long laneID;
    int ingressApproach;
    int egressApproach;

    bool directionIngressPath;
    bool directionEgressPath;

    QVector <Node> nodes;
    QVector <ConnectingLane> connectingLanes;
    std::shared_ptr<GeometryLineString> laneMapPtr;
    QVector<std::shared_ptr<GeometryPoint>> lanePointsVectorPtr;

    QVector <QPair<int, QVector<SignalGroupInfo>>> signalGroupInfoPairVector;
};


class Mapem : public Message {
    public:    
        Mapem(qreal longitude, qreal latitude, qreal altitude, int messageID, long stationID, int stationType,
              float laneWidth, QString crossroadName, long crossroadID, QVector<Lane> lanes);

        int findAdjIngVehLaneByOrientation(float orientation);

        static Lane getLaneByID(long id, QVector<Lane> lanes);
        static long getConnectedLaneID(ConnectingLane lane, QVector <Lane> lanes);
        static PointWorldCoord getFirstPointOfLane(Lane lane, PointWorldCoord refPoint);
        static PointWorldCoord getLastPointOfLane(Lane lane, PointWorldCoord refPoint);
        static std::vector <PointWorldCoord> getLanePoints(Lane lane, PointWorldCoord refPoint);

        bool haveAdjacentLanesSameSignalGroup(QVector<Lane> adjacentIngressLanes);
        void prepareCrossroadSignalGroups();
        QVector <QVector<Lane>> findAdjacentIngressVehicleLanes();
        QVector <Lane> getAllIngressVehicleLanes();
        QVector <QPair<int, QVector<SignalGroupInfo>>> getLaneSignalGroupsVector(Lane lane);

        QString GetProtocol() { return "Mapem"; };

        int crossroadID;
        QString crossroadName;
        float laneWidth;
        QVector <Lane> lanes;

        QVector <QVector<Lane>> adjacentIngressLanes;

        static QVector<QString> laneTypes;
};
