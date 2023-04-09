#pragma once

#include "message.h"
#include "QVector"
#include <QMapControl/GeometryPointImageScaled.h>

using namespace qmapcontrol;

class Denm : public Message {
public:
    Denm(double longitude, double latitude, double altitude, int messageID, long stationID, int stationType, long itsOriginatingStationID,
         int itsSequenceNumber, long denmDetectionTime, long denmReferenceTime, bool denmTermination, int itsCauseCode, int itsSubCauseCode);

    void getCauseCodeStrings(); // should be renamed
    QString GetProtocol() { return "Denm"; };

    long originatingStationID;
    int sequenceNumber;
    long detectionTime;
    long referenceTime;
    bool termination; // for negation of DENM
    int causeCode;
    int subCauseCode;
    PointWorldCoord refPoint;
    QString causeCodeStr;
    QString subCauseCodeStr;

    std::shared_ptr<GeometryPointImageScaled> geometryPoint;
};
