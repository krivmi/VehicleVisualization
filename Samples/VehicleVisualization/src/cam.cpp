#include "cam.h"
#include <ctime>

QString Cam::getTimeFormatted(double timeEpoch){
    time_t time = static_cast<time_t>(timeEpoch);

    // Convert time_t to tm structure
    tm *tm = std::gmtime(&time);

    // Create a buffer to hold the date string
    char buffer[20];

    // Format the date string using strftime function
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);

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
