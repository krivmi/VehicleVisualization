#include "spatem.h"

Spatem::Spatem(qreal longitude, qreal latitude, qreal altitude, int messageID, long stationID,
      int stationType, QString name, int id, int dsrcTimeStamp, int moy, QVector <MovementState> movementStates)
      : Message(longitude, latitude, altitude, messageID, stationID, stationType), crossroadName(name),
      crossroadID(id), timeStamp(dsrcTimeStamp), moy(moy),movementStates(movementStates) { }
