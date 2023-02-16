#ifndef CAM_H
#define CAM_H

#include "message.h"
#include "srem.h"

#include "QVector"
#include "QTime"
#include <QMapControl/QMapControl.h>
#include <QMapControl/LayerGeometry.h>
#include <QMapControl/GeometryPointImage.h>
#include <QMapControl/GeometryPolygon.h>
#include <QMapControl/GeometryPointImageScaled.h>

using namespace qmapcontrol;

struct VehiclePath {
    double dLatitude;
    double dLongitude;
    double dAltitude;

    std::shared_ptr<GeometryPoint> point;
};

class Cam: public Message {
    public:
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
        bool isSrcAttention = false;

        QString typeStr;
        QString imgSrcDefault;
        QString imgSrcAttention;
        PointWorldCoord refPoint;
        std::shared_ptr<GeometryPointImageScaled> geometryPoint; // musí být shared ptr
        std::shared_ptr<GeometryPoint> point; // musí být shared ptr
        std::shared_ptr<GeometryPolygon> polygon;

        Cam(qreal longitude, qreal latitude, qreal altitude, int messageID, long stationID, int stationType,
            float speed, float heading, QVector<VehiclePath> vehiclePath, float vehicleLength, float vehicleWidth,
            int vehicleRole, bool lowBeamHeadlightsOn, bool highBeamHeadlightsOn, bool leftTurnSignalOn,
            bool rightTurnSignalOn, bool daytimeRunningLightsOn, bool reverseLightOn, bool fogLightOn, bool parkingLightsOn, double timeEpoch)
            : Message(longitude, latitude, altitude,messageID, stationID, stationType){

            this->refPoint = PointWorldCoord(longitude, latitude);
            this->speed = speed;
            this->heading = heading;
            this->path = vehiclePath;
            this->vehicleLength = vehicleLength;
            this->vehicleWidth = vehicleWidth;
            this->vehicleRole = vehicleRole;
            this->vehicleRoleStr = "";
            this->lowBeamHeadlightsOn = lowBeamHeadlightsOn;
            this->highBeamHeadlightsOn = highBeamHeadlightsOn;
            this->leftTurnSignalOn = leftTurnSignalOn;
            this->rightTurnSignalOn = rightTurnSignalOn;
            this->daytimeRunningLightsOn = daytimeRunningLightsOn;
            this->reverseLightOn = reverseLightOn;
            this->fogLightOn = fogLightOn;
            this->parkingLightsOn = parkingLightsOn;
            this->timeEpoch = timeEpoch;

            if(this->vehicleRole == 1){
                this->vehicleRoleStr = "Public transport";
            } // ...TODO

            if(this->stationType == 5){
                this->imgSrcDefault = ":/resources/images/car_black.png";
                this->imgSrcAttention = ":/resources/images/car_black.png";
                this->typeStr = "Car";                
            }
            else if(this->stationType == 6){
                this->imgSrcDefault = ":/resources/images/bus_black.png";
                this->imgSrcAttention = ":/resources/images/bus_black.png";
                this->typeStr = "Bus";
            }
            else if(this->stationType == 11){
                this->imgSrcDefault = ":/resources/images/tram_black.png";
                this->imgSrcAttention = ":/resources/images/tram_black_srem.png";
                this->typeStr = "Tram";
            }
            else if(this->stationType == 15){ // ROAD SIDE UNIT
                this->imgSrcDefault = ":/resources/images/rsu_black.png";
                this->imgSrcAttention = ":/resources/images/rsu_black.png";
                this->typeStr = "RSU";
            }
            else if(this->stationType == 0){ // UNKNOWN
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black.png";
                this->typeStr = "Unknown";
            }
            else {
                this->imgSrcDefault = ":/resources/images/rsu_black.png";
                this->imgSrcAttention = ":/resources/images/rsu_black.png";
                this->typeStr = "Unknown";
                throw std::invalid_argument("This station type is not known");
            }
        };
        void update(std::shared_ptr<Cam> newUnit);

        QString GetProtocol() { return "Cam"; };
        QString getTimeFormatted(double timeEpoch);
};

#endif // CAM_H
