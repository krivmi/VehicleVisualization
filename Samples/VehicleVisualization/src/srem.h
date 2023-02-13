#ifndef SREM_H
#define SREM_H

#include "message.h"
#include "QVector"

using namespace qmapcontrol;

struct Request {
    int ID;
    int requestID;
    int requestType;
    int inBoundLane;
    int outBoundLane;
    int approachInBound;
    int approachOutBound;
};

class Srem : public Message {
public:
    Srem(double longitude, double latitude, double altitude, int messageID, long stationID, int stationType,
         float timeStamp, int sequenceNumber, QVector <Request> requests, long requestorID,
         int requestorRole, int requestorSubRole, QString requestorName, QString routeName)
        : Message(longitude, latitude, altitude, messageID, stationID, stationType){

        this->timeStamp = timeStamp;
        this->sequenceNumber = sequenceNumber;
        this->requests = requests;
        this->requestorID = requestorID;
        this->requestorRole = requestorRole;
        this->requestorSubRole = requestorSubRole;
        this->requestorName = requestorName;
        this->routeName = routeName;
    }
    float timeStamp;
    int sequenceNumber;
    QVector <Request> requests;
    long requestorID;
    int requestorRole;
    int requestorSubRole;
    QString requestorName;
    QString routeName;

    QString GetProtocol() { return "Srem"; };
};

#endif // SREM_H
