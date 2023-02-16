#ifndef DENM_H
#define DENM_H

#include "message.h"
#include "QVector"
#include <QMapControl/GeometryPointImageScaled.h>

using namespace qmapcontrol;

class Denm : public Message {
public:
    Denm(double longitude, double latitude, double altitude, int messageID, long stationID, int stationType, long itsOriginatingStationID,
         int itsSequenceNumber, long denmDetectionTime, long denmReferenceTime, bool denmTermination, int itsCauseCode, int itsSubCauseCode)
        : Message(longitude, latitude, altitude, messageID, stationID, stationType){

        this->originatingStationID = itsOriginatingStationID;
        this->sequenceNumber = itsSequenceNumber;
        this->detectionTime = denmDetectionTime;
        this->referenceTime = denmReferenceTime;
        this->termination = denmTermination;
        this->causeCode = itsCauseCode;
        this->subCauseCode = itsSubCauseCode;
        refPoint = PointWorldCoord(longitude, latitude);

        getCauseCodeStrings();
    }
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

    void getCauseCodeStrings();
    QString GetProtocol() { return "Denm"; };
};

#endif
