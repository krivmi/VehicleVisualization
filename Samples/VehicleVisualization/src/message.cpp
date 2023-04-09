#include "message.h"

Message::Message(qreal longitude, qreal latitude, qreal altitude,
    int messageID, long stationID, int stationType) : longitude(longitude),
    latitude(latitude), altitude(altitude), messageID(messageID), stationID(stationID),
    stationType(stationType), refPoint(PointWorldCoord(longitude, latitude)){}

