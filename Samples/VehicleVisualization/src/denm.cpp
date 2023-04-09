#include "denm.h"

Denm::Denm(double longitude, double latitude, double altitude, int messageID, long stationID, int stationType, long itsOriginatingStationID,
     int itsSequenceNumber, long denmDetectionTime, long denmReferenceTime, bool denmTermination, int itsCauseCode, int itsSubCauseCode)
    : Message(longitude, latitude, altitude, messageID, stationID, stationType),
      originatingStationID(itsOriginatingStationID), sequenceNumber(itsSequenceNumber),
      detectionTime(denmDetectionTime), referenceTime(denmReferenceTime), termination(denmTermination),
      causeCode(itsCauseCode), subCauseCode(itsSubCauseCode), refPoint(PointWorldCoord(longitude, latitude))
{
    getCauseCodeStrings();
}

void Denm::getCauseCodeStrings(){
    if(this->subCauseCode == 0){
        this->subCauseCodeStr = "No specific information";
    }

    if(this->causeCode == 1){
        this->causeCodeStr = "Traffic condition";

        if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Traffic jam slowly increasing";
        }
        else if(this->subCauseCode == 3){
            this->subCauseCodeStr = "Traffic jam increasing";
        }
        else if(this->subCauseCode == 4){
            this->subCauseCodeStr = "Traffic jam strongly increasing";
        }
        else if(this->subCauseCode == 5){
            this->subCauseCodeStr = "Traffic stationary";
        }
        else if(this->subCauseCode == 6){
            this->subCauseCodeStr = "Traffic jam slightly decreasing";
        }
        else if(this->subCauseCode == 7){
            this->subCauseCodeStr = "Traffic jam decreasing";
        }
        else if(this->subCauseCode == 8){
            this->subCauseCodeStr = "Traffic jam strongly decreasing";
        }
    }
    else if(this->causeCode == 2){
        this->causeCodeStr = "Accident";
    }
    else if(this->causeCode == 3){
        this->causeCodeStr = "Roadworks";

        if(this->subCauseCode == 4){
            this->subCauseCodeStr = "Short-term stationary roadWorks";
        }
        else if(this->subCauseCode == 5){
            this->subCauseCodeStr = "Street cleaning";
        }
        else if(this->subCauseCode == 6){
            this->subCauseCodeStr = "Winter service";
        }
    }
    else if(this->causeCode == 6){
        this->causeCodeStr = "Adverse Weather Condition - Adhesion";
    }
    else if(this->causeCode == 9){
        this->causeCodeStr = "Hazardous Location - Surface Condition";
    }
    else if(this->causeCode == 10){
        this->causeCodeStr = "Hazardous Location - Obstacle On The Road";
    }
    else if(this->causeCode == 11){
        this->causeCodeStr = "Hazardous Location - Animal On The Road";
    }
    else if(this->causeCode == 12){
        this->causeCodeStr = "Human Presence On The Road";
    }
    else if(this->causeCode == 14){
        this->causeCodeStr = "Wrong Way Driving";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Vehicle driving in wrong lane";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Vehicle driving in wrong driving direction";
        }
    }
    else if(this->causeCode == 15){
        this->causeCodeStr = "Rescue And Recovery Work In Progress";
    }
    else if(this->causeCode == 17){
        this->causeCodeStr = "Adverse Weather Condition - Extreme Weather Condition";
    }
    else if(this->causeCode == 19){
        this->causeCodeStr = "Adverse Weather Condition - Precipitation";
    }
    else if(this->causeCode == 26){
        this->causeCodeStr = "Slow Vehicle";
    }
    else if(this->causeCode == 27){
        this->causeCodeStr = "Dangerous End Of Queue";
    }
    else if(this->causeCode == 91){
        this->causeCodeStr = "Vehicle Breakdown";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Lack of fuel";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Lack of battery";
        }
        else if(this->subCauseCode == 3){
            this->subCauseCodeStr = "Engine problem";
        }
        else if(this->subCauseCode == 4){
            this->subCauseCodeStr = "Transmission problem";
        }
        else if(this->subCauseCode == 5){
            this->subCauseCodeStr = "Engine cooling problem";
        }
        else if(this->subCauseCode == 6){
            this->subCauseCodeStr = "Braking system problem";
        }
        else if(this->subCauseCode == 7){
            this->subCauseCodeStr = "Steering problem";
        }
        else if(this->subCauseCode == 8){
            this->subCauseCodeStr = "Tyre puncture";
        }
    }
    else if(this->causeCode == 92){
        this->causeCodeStr = "Post Crash";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Accident without e-Call triggered";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Accident with e-Call manually triggered";
        }
        else if(this->subCauseCode == 3){
            this->subCauseCodeStr = "Accident with e-Call automatically triggered";
        }
        else if(this->subCauseCode == 4){
            this->subCauseCodeStr = "Accident with e-Call triggered without a possible access to a cell network.";
        }
    }
    else if(this->causeCode == 93){
        this->causeCodeStr = "Human Problem";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Glycaemia problem";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Heart problem";
        }
    }
    else if(this->causeCode == 94){
        this->causeCodeStr = "Stationary Vehicle";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Human Problem";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Vehicle breakdown";
        }
        else if(this->subCauseCode == 3){
            this->subCauseCodeStr = "Post crash";
        }
        else if(this->subCauseCode == 4){
            this->subCauseCodeStr = "On public transport stop";
        }
        else if(this->subCauseCode == 5){
            this->subCauseCodeStr = "Carrying dangerous goods";
        }
    }
    else if(this->causeCode == 95){
        this->causeCodeStr = "Emergency Vehicle Approaching";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Emergency vehicle approaching";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Prioritized vehicle approaching";
        }
    }
    else if(this->causeCode == 96){
        this->causeCodeStr = "Hazardous Location - Dangerous Curve";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Dangerous left turn curve";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Dangerous right turn curve";
        }
        else if(this->subCauseCode == 3){
            this->subCauseCodeStr = "Multiple curves starting with unknown turning direction";
        }
        else if(this->subCauseCode == 4){
            this->subCauseCodeStr = "Multiple curves starting with left turn";
        }
        else if(this->subCauseCode == 5){
            this->subCauseCodeStr = "Multiple curves starting with right turn";
        }
    }
    else if(this->causeCode == 97){
        this->causeCodeStr = "Collision Risk";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Longitudinal collision risk";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Crossing collision risk";
        }
        else if(this->subCauseCode == 3){
            this->subCauseCodeStr = "Lateral collision risk";
        }
        else if(this->subCauseCode == 4){
            this->subCauseCodeStr = "Collision risk involving vulnerable road user";
        }
    }
    else if(this->causeCode == 98){
        this->causeCodeStr = "Signal Violation";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Stop sign violation";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Traffic light violation";
        }
        else if(this->subCauseCode == 3){
            this->subCauseCodeStr = "Turning regulation violation";
        }
    }
    else if(this->causeCode == 99){
        this->causeCodeStr = "Dangerous Situation";

        if(this->subCauseCode == 1){
            this->subCauseCodeStr = "Emergency electronic brake lights";
        }
        else if(this->subCauseCode == 2){
            this->subCauseCodeStr = "Pre-crash system activated";
        }
        else if(this->subCauseCode == 3){
            this->subCauseCodeStr = "ESP (Electronic Stability Program) activated";
        }
        else if(this->subCauseCode == 4){
            this->subCauseCodeStr = "ABS (Anti-lock braking system) activated";
        }
        else if(this->subCauseCode == 5){
            this->subCauseCodeStr = "AEB (Automatic Emergency Braking) activated";
        }
        else if(this->subCauseCode == 6){
            this->subCauseCodeStr = "Brake warning activated";
        }
        else if(this->subCauseCode == 7){
            this->subCauseCodeStr = "Collision risk warning activated";
        }
    }
    else {
        this->causeCodeStr = "Unknown";
        this->subCauseCodeStr = "Unknown";
    }
}
