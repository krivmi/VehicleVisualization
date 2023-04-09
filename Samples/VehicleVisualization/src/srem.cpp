#include "srem.h"

Srem::Srem(double longitude, double latitude, double altitude, int messageID, long stationID, int stationType,
     float timeStamp, int sequenceNumber, QVector <Request> requests, long requestorID,
     int requestorRole, int requestorSubRole, QString requestorName, QString routeName)
    : Message(longitude, latitude, altitude, messageID, stationID, stationType), timeStamp(timeStamp),
      sequenceNumber(sequenceNumber), requests(requests), requestorID(requestorID), requestorRole(requestorRole),
      requestorSubRole(requestorSubRole), requestorName(requestorName), routeName(routeName) { /* body */ }
