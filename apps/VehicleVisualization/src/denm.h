#pragma once

#include "stdafx.h"

#include "message.h"

using namespace qmapcontrol;

class Denm : public Message {
public:
    static const std::unordered_map<int, std::pair<QString, std::unordered_map<int, QString>>> causeCodeStringMap;

    Denm(double longitude, double latitude, double altitude, int messageID, long stationID, int stationType, long itsOriginatingStationID,
         int itsSequenceNumber, long denmDetectionTime, long denmReferenceTime, bool denmTermination, int itsCauseCode, int itsSubCauseCode);

    void setCauseStringValues(int causeCode, int subCauseCode);
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
