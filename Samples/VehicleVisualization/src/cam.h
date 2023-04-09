#pragma once

#include "message.h"
#include "srem.h"

#include "QVector"
#include "QTime"

#include <QMapControl/GeometryPointImageScaled.h>
#include <QMapControl/GeometryLineString.h>

struct VehiclePath {
    double dLatitude;
    double dLongitude;
    double dAltitude;

    std::shared_ptr<GeometryPoint> point;
};

class Cam: public Message {
    public:
        Cam(qreal longitude, qreal latitude, qreal altitude, int messageID, long stationID, int stationType,
            float speed, float heading, QVector<VehiclePath> vehiclePath, float vehicleLength, float vehicleWidth,
            int vehicleRole, bool lowBeamHeadlightsOn, bool highBeamHeadlightsOn, bool leftTurnSignalOn,
            bool rightTurnSignalOn, bool daytimeRunningLightsOn, bool reverseLightOn, bool fogLightOn, bool parkingLightsOn, double timeEpoch);

        void update(std::shared_ptr<Cam> newUnit);
        QString getTimeFormatted(double timeEpoch, bool justHMS = false);
        QString GetProtocol() { return "Cam"; };

        float speed;
        float heading;
        QVector <VehiclePath> path;
        float vehicleLength;
        float vehicleWidth;
        int vehicleRole;
        QString vehicleRoleStr;
        bool lowBeamHeadlightsOn;
        bool highBeamHeadlightsOn;
        bool leftTurnSignalOn;
        bool rightTurnSignalOn;
        bool daytimeRunningLightsOn;
        bool reverseLightOn;
        bool fogLightOn;
        bool parkingLightsOn;
        double timeEpoch; // last update
        QTime lastSremUpdate;
        bool isSrcAttention;

        // additional
        QString typeStr;
        QString imgSrcDefault;
        QString imgSrcAttention;
        PointWorldCoord refPoint;
        std::shared_ptr<GeometryPointImageScaled> geometryPoint; // musí být shared ptr
        //std::shared_ptr<GeometryPolygon> polygon; // for bounding box
        std::shared_ptr<GeometryLineString> pathString;
};
