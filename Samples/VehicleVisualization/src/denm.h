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

        if(this->causeCode == 94){
            //this->imgSrc = ":/resources/images/car_black.png";
            this->causeCodeStr = "Stationary vehicle";

            if(this->subCauseCode == 4){
                this->subCauseCodeStr = "On public transport stop";
            }
        }
        else if(this->causeCode == 97){
            //this->imgSrc = ":/resources/images/bus_black.png";
            this->causeCodeStr = "Collision risk";

            if(this->subCauseCode == 1){
                this->subCauseCodeStr = "Longitudinal collision risk";
            }
        }
        else {
            //this->imgSrc = ":/resources/images/rsu_black.png";
            this->causeCodeStr = "Unknown";
            this->subCauseCodeStr = "Unknown";
            //throw std::invalid_argument("This station type is not known");
        }
    }
    long originatingStationID;
    int sequenceNumber;
    long detectionTime;
    long referenceTime;
    bool termination;
    int causeCode;
    int subCauseCode;
    PointWorldCoord refPoint;
    QString causeCodeStr;
    QString subCauseCodeStr;

    std::shared_ptr<GeometryPointImageScaled> geometryPoint;


    QString GetProtocol() { return "Denm"; };
};

#endif
