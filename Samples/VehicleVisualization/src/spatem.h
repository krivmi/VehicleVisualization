#ifndef SPATEM_H
#define SPATEM_H

#include "message.h"
#include "QVector"

#include <QMapControl/QMapControl.h>

struct MovementEvent {
    int eventState;
    int startTime;
    int minEndTime;
    int maxEndTime;
    int likelyTime;
    int confidence;
};

struct MovementState {
    int signalGroup;
    QString movementName;
    QVector <MovementEvent> movementEvents;
};

class Spatem : public Message
{
public:
    Spatem();

    QString crossroadName;
    int crossroadID;
    int timeStamp;
    int moy;

    QVector <MovementState> movementStates;
    // SPATEM messages does not contain location information, it is based on the corresponding MAPEM

    Spatem(qreal longitude, qreal latitude, qreal altitude, int messageID, long stationID, int stationType, QString name, int id, int dsrcTimeStamp, int moy, QVector <MovementState> movementStates) :
          Message(longitude, latitude, altitude, messageID, stationID, stationType){
        this->movementStates = movementStates;
        this->crossroadName = name;
        this->crossroadID = id;
        this->timeStamp = dsrcTimeStamp;
        this->moy = moy;
    };

    QString GetProtocol() { return "Spatem"; };
};

#endif // SPATEM_H
