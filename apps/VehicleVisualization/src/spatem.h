#pragma once

#include "message.h"
#include "QVector"

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
    // SPATEM messages does not contain location information, it is based on the corresponding MAPEM
    Spatem(qreal longitude, qreal latitude, qreal altitude, int messageID, long stationID, int stationType, QString name, int id, int dsrcTimeStamp, int moy, QVector <MovementState> movementStates);

    QString GetProtocol() { return "Spatem"; };

    QString crossroadName;
    int crossroadID;
    int timeStamp;
    int moy;
    QVector <MovementState> movementStates;
};
