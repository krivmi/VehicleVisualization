#include "message.h"

Message::Message(double longitude, double latitude, double altitude,
                 int messageID, long stationID, int stationType)
{
    this->longitude = longitude;
    this->latitude = latitude;
    this->altitude = altitude;
    this->messageID = messageID;
    this->stationID = stationID;
    this->stationType = stationType;
    this->refPoint = PointWorldCoord(longitude, latitude);
}

