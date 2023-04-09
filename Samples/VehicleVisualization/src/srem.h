#pragma once

#include "message.h"
#include "QVector"

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
             int requestorRole, int requestorSubRole, QString requestorName, QString routeName);

        QString GetProtocol() { return "Srem"; };

        float timeStamp;
        int sequenceNumber;
        QVector <Request> requests;
        long requestorID;
        int requestorRole;
        int requestorSubRole;
        QString requestorName;
        QString routeName;
};
