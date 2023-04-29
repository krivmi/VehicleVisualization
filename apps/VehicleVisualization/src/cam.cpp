#include "cam.h"
#include <ctime>

Cam::Cam(qreal longitude, qreal latitude, qreal altitude, int messageID, long stationID, int stationType,
    float speed, float heading, QVector<VehiclePath> vehiclePath, float vehicleLength, float vehicleWidth,
    int vehicleRole, bool lowBeamHeadlightsOn, bool highBeamHeadlightsOn, bool leftTurnSignalOn,
    bool rightTurnSignalOn, bool daytimeRunningLightsOn, bool reverseLightOn, bool fogLightOn, bool parkingLightsOn,
    double timeEpoch) : Message(longitude, latitude, altitude,messageID, stationID, stationType),
    speed(speed), heading(heading), path(vehiclePath), vehicleLength(vehicleLength), vehicleWidth(vehicleWidth),
    vehicleRole(vehicleRole), vehicleRoleStr("Unknown"), lowBeamHeadlightsOn(lowBeamHeadlightsOn),
    highBeamHeadlightsOn(highBeamHeadlightsOn), leftTurnSignalOn(leftTurnSignalOn), rightTurnSignalOn(rightTurnSignalOn),
    daytimeRunningLightsOn(daytimeRunningLightsOn), reverseLightOn(reverseLightOn), fogLightOn(fogLightOn),
    parkingLightsOn(parkingLightsOn), timeEpoch(timeEpoch), isSrcAttention(false), refPoint(PointWorldCoord(longitude, latitude))
{
    // vehicle role string
    if(this->vehicleRole == 1)
    {
        this->vehicleRoleStr = "Public transport";
    }
    // ...TODO

    // station type string
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
    else if(this->stationType == 6){ // BUS
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

QString Cam::getTimeFormatted(double timeEpoch, bool justHMS)
{
    time_t time = static_cast<time_t>(timeEpoch);

    // Convert time_t to tm structure
    tm *tm = std::gmtime(&time);

    // Create a buffer to hold the date string
    char buffer[20];

    // Format the date string using strftime function
    if(justHMS)
    {
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", tm);
    }
    else
    {
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    }

    return buffer;
}

void Cam::update(std::shared_ptr<Cam> newUnit){
    this->longitude = newUnit->longitude;
    this->latitude = newUnit->latitude;
    this->altitude = newUnit->altitude;
    this->speed = newUnit->speed;
    this->heading = newUnit->heading;
    this->lowBeamHeadlightsOn = newUnit->lowBeamHeadlightsOn;
    this->highBeamHeadlightsOn = newUnit->highBeamHeadlightsOn;
    this->leftTurnSignalOn = newUnit->leftTurnSignalOn;
    this->rightTurnSignalOn = newUnit->rightTurnSignalOn;
    this->daytimeRunningLightsOn = newUnit->daytimeRunningLightsOn;
    this->reverseLightOn = newUnit->reverseLightOn;
    this->fogLightOn = newUnit->fogLightOn;
    this->parkingLightsOn = newUnit->parkingLightsOn;
    this->timeEpoch = newUnit->timeEpoch;
    this->refPoint.setLatitude(newUnit->latitude);
    this->refPoint.setLongitude(newUnit->longitude);
}
