#pragma once

#include "stdafx.h"

using namespace qmapcontrol;

class Message {
public:

    Message(qreal longitude, qreal latitude, qreal altitude,
            int messageID, long stationID, int stationType);

    virtual ~Message(){};
    virtual QString GetProtocol() = 0;

    qreal longitude;
    qreal latitude;
    qreal altitude;
    int messageID;
    long stationID;
    int stationType;
    PointWorldCoord refPoint;
};
