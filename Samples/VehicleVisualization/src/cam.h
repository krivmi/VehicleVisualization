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
#include <QMapControl/GeometryLineString.h>

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
        std::shared_ptr<GeometryLineString> pathString;

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
            this->vehicleRoleStr = "Unknown";
            this->lowBeamHeadlightsOn = lowBeamHeadlightsOn;
            this->highBeamHeadlightsOn = highBeamHeadlightsOn;
            this->leftTurnSignalOn = leftTurnSignalOn;
            this->rightTurnSignalOn = rightTurnSignalOn;
            this->daytimeRunningLightsOn = daytimeRunningLightsOn;
            this->reverseLightOn = reverseLightOn;
            this->fogLightOn = fogLightOn;
            this->parkingLightsOn = parkingLightsOn;
            this->timeEpoch = timeEpoch;

            if(this->vehicleRole == 1)
            {
                this->vehicleRoleStr = "Public transport";
            }
            // ...TODO

            // see: en 302 636-04 1.2.1
            if(this->stationType == 1){
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Pedestrian";
            }
            else if(this->stationType == 2){
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Cyclist";
            }
            else if(this->stationType == 3){
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Moped";
            }
            else if(this->stationType == 4){
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Motorcycle";
            }
            else if(this->stationType == 5){
                this->imgSrcDefault = ":/resources/images/car_black.png";
                this->imgSrcAttention = ":/resources/images/car_black_srem.png";
                this->typeStr = "Car";                
            }
            else if(this->stationType == 6){
                this->imgSrcDefault = ":/resources/images/bus_black.png";
                this->imgSrcAttention = ":/resources/images/bus_black_srem.png";
                this->typeStr = "Bus";
            }
            else if(this->stationType == 7){
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Light truck";
            }
            else if(this->stationType == 8){
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Heavy truck";
            }
            else if(this->stationType == 9){
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Trailer";
            }
            else if(this->stationType == 10){
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Special vehicle";
            }
            else if(this->stationType == 11){
                this->imgSrcDefault = ":/resources/images/tram_black.png";
                this->imgSrcAttention = ":/resources/images/tram_black_srem.png";
                this->typeStr = "Tram";
            }
            else if(this->stationType == 15){ // ROAD SIDE UNIT
                this->imgSrcDefault = ":/resources/images/rsu_black.png";
                this->imgSrcAttention = ":/resources/images/rsu_black_srem.png";
                this->typeStr = "RSU";
            }
            else if(this->stationType == 0){ // UNKNOWN
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Unknown";
            }
            else {
                this->imgSrcDefault = ":/resources/images/unknown_black.png";
                this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
                this->typeStr = "Unknown";
            }
        };
        void update(std::shared_ptr<Cam> newUnit);

        QString GetProtocol() { return "Cam"; };
        QString getTimeFormatted(double timeEpoch, bool justHMS = false);
};

#endif // CAM_H
