#include "denm.h"

const std::unordered_map<int, std::pair<QString, std::unordered_map<int, QString>>> Denm::causeCodeStringMap = {
    {0, {"No specific information", {}}},
    {1, {"Traffic condition", {
        {2, "Traffic jam slowly increasing"},
        {3, "Traffic jam increasing"},
        {4, "Traffic jam strongly increasing"},
        {5, "Traffic stationary"},
        {6, "Traffic jam slightly decreasing"},
        {7, "Traffic jam decreasing"},
        {8, "Traffic jam strongly decreasing"}
    }}},
    {2, {"Accident", {}}},
    {3, {"Roadworks", {
        {4, "Short-term stationary roadWorks"},
        {5, "Street cleaning"},
        {6, "Winter service"}
    }}},
    {6, {"Adverse Weather Condition - Adhesion", {}}},
    {9, {"Hazardous Location - Surface Condition", {}}},
    {10, {"Hazardous Location - Obstacle On The Road", {}}},
    {11, {"Hazardous Location - Animal On The Road", {}}},
    {12, {"Human Presence On The Road", {}}},
    {14, {"Wrong Way Driving", {
        {1, "Vehicle driving in wrong lane"},
        {2, "Vehicle driving in wrong driving direction"}
    }}},
    {15, {"Rescue And Recovery Work In Progress", {}}},
    {17, {"Adverse Weather Condition - Extreme Weather Condition", {}}},
    {19, {"Adverse Weather Condition - Precipitation", {}}},
    {26, {"Slow Vehicle", {}}},
    {27, {"Dangerous End Of Queue", {}}},
    {91, {"Vehicle Breakdown", {
        {1, "Lack of fuel"},
        {2, "Lack of battery"},
        {3, "Engine problem"},
        {4, "Transmission problem"},
        {5, "Engine cooling problem"},
        {6, "Braking system problem"},
        {7, "Steering problem"},
        {8, "Tyre puncture"}
    }}},
    {92, {"Post Crash", {
        {1, "Accident without e-Call triggered"},
        {2, "Accident with e-Call manually triggered"},
        {3, "Accident with e-Call automatically triggered"},
        {4, "Accident with e-Call triggered without a possible access to a cell network."}
    }}},
    {93, {"Human Problem", {
        {1, "Glycaemia problem"},
        {2, "Heart problem"}
    }}},
    {94, {"Stationary Vehicle", {
        {1, "Human Problem"},
        {2, "Vehicle breakdown"},
        {3, "Post crash"},
        {4, "On public transport stop"},
        {5, "Carrying dangerous goods"}
    }}},
    {95, {"Emergency Vehicle Approaching", {
        {1, "Emergency vehicle approaching"},
        {2, "Prioritized vehicle approaching"}
    }}},
    {96, {"Hazardous Location - Dangerous Curve", {
        {1, "Dangerous left turn curve"},
        {2, "Dangerous right turn curve"},
        {3, "Multiple curves starting with unknown turning direction"},
        {4, "Multiple curves starting with left turn"},
        {5, "Multiple curves starting with right turn"}
    }}},
    {97, {"Collision Risk", {
        {1, "Longitudinal collision risk"},
        {2, "Crossing collision risk"},
        {3, "Lateral collision risk"},
        {4, "Collision risk involving vulnerable road user"}
    }}},
    {98, {"Signal Violation", {
        {1, "Stop sign violation"},
        {2, "Traffic light violation"},
        {3, "Turning regulation violation"}
    }}},
    {99, {"Dangerous Situation", {
        {1, "Emergency electronic brake lights"},
        {2, "Pre-crash system activated"},
        {3, "ESP (Electronic Stability Program) activated"},
        {4, "ABS (Anti-lock braking system) activated"},
        {5, "AEB (Automatic Emergency Braking) activated"},
        {6, "Brake warning activated"},
        {7, "Collision risk warning activated"}
    }}}
    // ... and more
};
Denm::Denm(double longitude, double latitude, double altitude, int messageID, long stationID, int stationType, long itsOriginatingStationID,
     int itsSequenceNumber, long denmDetectionTime, long denmReferenceTime, bool denmTermination, int itsCauseCode, int itsSubCauseCode)
    : Message(longitude, latitude, altitude, messageID, stationID, stationType),
      originatingStationID(itsOriginatingStationID), sequenceNumber(itsSequenceNumber),
      detectionTime(denmDetectionTime), referenceTime(denmReferenceTime), termination(denmTermination),
      causeCode(itsCauseCode), subCauseCode(itsSubCauseCode), refPoint(PointWorldCoord(longitude, latitude))
{
    setCauseStringValues(causeCode, subCauseCode);
    printf("");
}

void Denm::setCauseStringValues(int causeCode, int subCauseCode)
{
    auto causeIt = causeCodeStringMap.find(causeCode);

    if (causeIt != causeCodeStringMap.end()) {
        causeCodeStr = causeIt->second.first;

        auto subCauseIt = causeIt->second.second.find(subCauseCode);

        if (subCauseIt != causeIt->second.second.end()) {
            subCauseCodeStr = subCauseIt->second;
        } else {
            subCauseCodeStr = "Unknown";
        }
    } else {
        causeCodeStr = "Unknown";
        subCauseCodeStr = "Unknown";
    }
}
