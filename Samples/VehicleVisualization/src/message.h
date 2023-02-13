#ifndef MESSAGE_H
#define MESSAGE_H

#include <QMapControl/QMapControl.h>

using namespace qmapcontrol;

class Message
{
public:
    qreal longitude;
    qreal latitude;
    qreal altitude;
    int messageID;
    long stationID;
    int stationType;

    PointWorldCoord refPoint;

    Message(qreal longitude, qreal latitude, qreal altitude,
            int messageID, long stationID, int stationType);

    virtual ~Message(){};

    //virtual int GetId() = 0;
    virtual QString GetProtocol() = 0;

};

#endif // MESSAGE_H
