#include "cam.h"
#include <ctime>

const std::unordered_map<int, std::tuple<QString, QString, QString>> Cam::stationTypeStringMap = {
    {1, std::make_tuple(":/resources/images/unknown_black.png",
                        ":/resources/images/unknown_black_srem.png",
                        "Pedestrian")},
    {2, std::make_tuple(":/resources/images/unknown_black.png",
                        ":/resources/images/unknown_black_srem.png",
                        "Cyclist")},
    {3, std::make_tuple(":/resources/images/unknown_black.png",
                        ":/resources/images/unknown_black_srem.png",
                        "Moped")},
    {4, std::make_tuple(":/resources/images/unknown_black.png",
                        ":/resources/images/unknown_black_srem.png",
                        "Motorcycle")},
    {5, std::make_tuple(":/resources/images/car_black.png",
                        ":/resources/images/car_black_srem.png",
                        "Car")},
    {6, std::make_tuple(":/resources/images/bus_black.png",
                        ":/resources/images/bus_black_srem.png",
                        "Bus")},
    {7, std::make_tuple(":/resources/images/unknown_black.png",
                        ":/resources/images/unknown_black_srem.png",
                        "Light truck")},
    {8, std::make_tuple(":/resources/images/unknown_black.png",
                        ":/resources/images/unknown_black_srem.png",
                        "Heavy truck")},
    {9, std::make_tuple(":/resources/images/unknown_black.png",
                        ":/resources/images/unknown_black_srem.png",
                        "Trailer")},
    {10, std::make_tuple(":/resources/images/unknown_black.png",
                         ":/resources/images/unknown_black_srem.png",
                         "Special vehicle")},
    {11, std::make_tuple(":/resources/images/tram_black.png",
                         ":/resources/images/tram_black_srem.png",
                         "Tram")},
    {15, std::make_tuple(":/resources/images/rsu_black.png",
                         ":/resources/images/rsu_black_srem.png",
                         "RSU")},
    {0, std::make_tuple(":/resources/images/unknown_black.png",
                        ":/resources/images/unknown_black_srem.png",
                        "Unknown")}
};


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
    // get station type string
    // see: en 302 636-04 1.2.1
    setStationStringValues(this->stationType);

    // vehicle role string
    if(this->vehicleRole == 1)
    {
        this->vehicleRoleStr = "Public transport";
    }
    // TODO - vehicle role strings map
};
void Cam::setStationStringValues(int stationType)
{
    auto it = stationTypeStringMap.find(stationType);

    if (it != stationTypeStringMap.end())
    {
        std::tie(this->imgSrcDefault, this->imgSrcAttention, this->typeStr) = it->second;
    } else {
        this->imgSrcDefault = ":/resources/images/unknown_black.png";
        this->imgSrcAttention = ":/resources/images/unknown_black_srem.png";
        this->typeStr = "Unknown";
    }
}
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
