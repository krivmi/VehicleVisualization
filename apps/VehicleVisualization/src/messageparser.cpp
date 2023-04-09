#include "messageparser.h"

#include <stdexcept>
#include <memory>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>

MessageParser::MessageParser(){}

MessageParser& MessageParser::getInstance(){
    static MessageParser instance; // Guaranteed to be destroyed, Instantiated on first use.
    return instance;
}

void MessageParser::clear()
{
    json_message = "";
    stack.clear();
}

int MessageParser::loadJSONFromString(QString jsonString){
    QByteArray jsonBytes = jsonString.toLocal8Bit();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);
    QJsonArray loadedMessagesArray = QJsonArray();

    if(!jsonDoc.isArray()){
        qInfo() << "JSON doc is not an array.";
        return 1;
    }

    loadedMessagesArray = jsonDoc.array();

    if(loadedMessagesArray.isEmpty()){
        qInfo() << "The array is empty.";
        return 1;
    }

    // loop over all messages and recognize them
    for(int i = 0; i< loadedMessagesArray.count(); i++){
        QJsonObject packetObj = loadedMessagesArray.at(i).toObject();
        this->recognizeMessage(packetObj);
    }

    // emit success after success
    emit messagesParsed("loadedMessagesArray");

    return 0;
}
void MessageParser::recognizeMessage(QJsonObject jsonObject){
    //qInfo() << jsonMessage << "\n\n\n";

    QJsonObject _source = jsonObject["_source"].toObject();
    QJsonObject layers = _source["layers"].toObject();
    QJsonObject its = layers["its"].toObject();
    QJsonObject its_ItsPduHeader_element = its["its.ItsPduHeader_element"].toObject();

    if(its_ItsPduHeader_element["its.messageID"].toString() == "2"){
        parseCAM(jsonObject);
    }
    else if(its_ItsPduHeader_element["its.messageID"].toString() == "1"){
        parseDENM(jsonObject);
    }
    else if(its_ItsPduHeader_element["its.messageID"].toString() == "9"){
        parseSREM(jsonObject);
    }
    else if(its_ItsPduHeader_element["its.messageID"].toString() == "4"){
        parseSPATEM(jsonObject);
    }
    else if(its_ItsPduHeader_element["its.messageID"].toString() == "5"){
        parseMAPEM(jsonObject);
    }
    else{
        // TODO - recognize this type of message somehow
        //parseGEONW(jsonObject);
        //qInfo() << "GEONW message received";
    }
}
void MessageParser::processMessage()
{
    QString clear_json;
    // find the beginning of the message
    for(int i = 0; i < json_message.length(); i++){
        if(json_message[i] == "{"){
            clear_json = json_message.right(json_message.length() - i);
            break;
        }
    }
    QByteArray json_bytes = clear_json.toLocal8Bit();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_bytes);
    QJsonObject jsonObject = json_doc.object();

    this->recognizeMessage(jsonObject);

    json_message.clear();
}
void MessageParser::findMessagesInStream(QString messageStream){
    bool message_completed = false;
    int completed_at_index = -1;

    for (int i = 0; i < messageStream.length(); i++) {
        if(messageStream[i] == '}'){
            if ( !stack.isEmpty() && (stack.top() == '{')) {
                // if we found any complete pair of bracket then pop
                stack.pop();

                if (stack.empty()) { // if stack is empty, we have a complete message
                    json_message.append(messageStream.mid(0, i + 1)); // append the end of the message
                    processMessage();

                    message_completed = true;
                    completed_at_index = i;
                }
            }
            else {
                stack.push(messageStream[i]);
            }
        }
        else if(messageStream[i] == '{'){
            stack.push(messageStream[i]);
        }
    }
    if(message_completed){ // nebude to fungovat, pokud se do jednoho řádku vleze více zpráv
        json_message.append(messageStream.mid(completed_at_index + 1, messageStream.length() - completed_at_index + 1));
    }
    else{
        json_message.append(messageStream);
    }
}
void MessageParser::parseCAM(QJsonObject packetObj){
    QJsonObject _source = packetObj["_source"].toObject();
    QJsonObject layers = _source["layers"].toObject();
    QJsonObject frame = layers["frame"].toObject();
    QJsonObject gnw = layers["gnw"].toObject();
    QJsonObject gnw_tsb = gnw["geonw.tsb"].toObject();
    QJsonObject gnw_src_pos_tree = gnw_tsb["geonw.src_pos_tree"].toObject();

    //float pLat = (gnw_src_pos_tree["geonw.src_pos.lat"].toString().toFloat())/ 10000000;
    //float pLong = (gnw_src_pos_tree["geonw.src_pos.long"].toString().toFloat())/ 10000000;
    //float pSpeed = (gnw_src_pos_tree["geonw.src_pos.speed"].toString().toFloat())/ 100 * 3.6;
    //float pHeading = ((gnw_src_pos_tree["geonw.src_pos.hdg"].toString().toFloat())/ 10) - 90;
    double timeEpoch = (frame["frame.time_epoch"].toString().toDouble());

    QJsonObject its = layers["its"].toObject();
    QJsonObject cam_CoopAwareness_element = its["cam.CoopAwareness_element"].toObject();
    QJsonObject cam_camParameters_element = cam_CoopAwareness_element["cam.camParameters_element"].toObject();

    // *** LOW FREQUENCY CONTAINER *** //
    QJsonObject camLowFrequencyContainer_tree = cam_camParameters_element["cam.lowFrequencyContainer_tree"].toObject();
    QJsonObject camBasicVehicleContainerLowFrequency_element = camLowFrequencyContainer_tree["cam.basicVehicleContainerLowFrequency_element"].toObject();

    QJsonObject camPathHistory_tree = camBasicVehicleContainerLowFrequency_element["cam.pathHistory_tree"].toObject();
    int pathRecords = camBasicVehicleContainerLowFrequency_element["cam.pathHistory"].toString().toInt();

    QVector <VehiclePath> path;
    for(int i = 0; i < pathRecords; i++) {
        QJsonObject item_i = camPathHistory_tree["Item " + QString::number(i)].toObject();
        QJsonObject itsPathPoint_element = item_i["its.PathPoint_element"].toObject();
        QJsonObject itsPathPosition_element = itsPathPoint_element["its.pathPosition_element"].toObject();

        qreal dLat = (itsPathPosition_element["its.deltaLatitude"].toString().toDouble())/ 10000000.0;
        qreal dLong = (itsPathPosition_element["its.deltaLongitude"].toString().toDouble())/ 10000000.0;
        qreal dAlt = (itsPathPosition_element["its.deltaAltitude"].toString().toDouble())/ 100.0; // TODO - nejsem si jist
        //qInfo() << "Info: " << pLat << ", " << pLong << ", " << pAlt;
        VehiclePath vehiclePath;
        vehiclePath.dLatitude = dLat;
        vehiclePath.dLongitude = dLong;
        vehiclePath.dAltitude = dAlt;
        path.append(vehiclePath);
    }

    QJsonObject camExteriorLightsTree = camBasicVehicleContainerLowFrequency_element["cam.exteriorLights_tree"].toObject();
    bool lowBeamHeadlightsOn = camExteriorLightsTree["its.ExteriorLights.lowBeamHeadlightsOn"].toString().toInt();
    bool highBeamHeadlightsOn = camExteriorLightsTree["its.ExteriorLights.highBeamHeadlightsOn"].toString().toInt();
    bool leftTurnSignalOn = camExteriorLightsTree["its.ExteriorLights.leftTurnSignalOn"].toString().toInt();
    bool rightTurnSignalOn = camExteriorLightsTree["its.ExteriorLights.rightTurnSignalOn"].toString().toInt();
    bool daytimeRunningLightsOn = camExteriorLightsTree["its.ExteriorLights.daytimeRunningLightsOn"].toString().toInt();
    bool reverseLightOn = camExteriorLightsTree["its.ExteriorLights.reverseLightOn"].toString().toInt();
    bool fogLightOn = camExteriorLightsTree["its.ExteriorLights.fogLightOn"].toString().toInt();
    bool parkingLightsOn = camExteriorLightsTree["its.ExteriorLights.parkingLightsOn"].toString().toInt();

    if(leftTurnSignalOn){
        qInfo() << "leftTurnSignalOn works";
    }
    if(rightTurnSignalOn){
        qInfo() << "rightTurnSignalOn works";
    }

    int vehicleRole = camBasicVehicleContainerLowFrequency_element["cam.vehicleRole"].toString().toInt();

    // *** HIGH FREQUENCY CONTAINER *** //
    QJsonObject camHighFrequencyContainerTree = cam_camParameters_element["cam.highFrequencyContainer_tree"].toObject();
    QJsonObject camBasicVehicleContainerHighFrequency_element = camHighFrequencyContainerTree["cam.basicVehicleContainerHighFrequency_element"].toObject();
    QJsonObject camVehicleLengthElement = camBasicVehicleContainerHighFrequency_element["cam.vehicleLength_element"].toObject();

    QJsonObject camHeadingElement = camBasicVehicleContainerHighFrequency_element["cam.heading_element"].toObject();
    QJsonObject camSpeedElement = camBasicVehicleContainerHighFrequency_element["cam.speed_element"].toObject();

    float vehicleLength = (camVehicleLengthElement["its.vehicleLengthValue"].toString().toFloat())/ 10.0f;
    float vehicleWidth = (camBasicVehicleContainerHighFrequency_element["cam.vehicleWidth"].toString().toFloat())/ 10.0f;

    float pHeading = ((camHeadingElement["its.headingValue"].toString().toFloat())/ 10.0f) - 90.0f;
    float pSpeed = ((camSpeedElement["its.speedValue"].toString().toFloat())/ 100.0f) * 3.6f;

    // *** BASIC FREQUENCY CONTAINER *** //
    QJsonObject cam_basicContainer_element = cam_camParameters_element["cam.basicContainer_element"].toObject();
    QJsonObject camReferencePositionElement = cam_basicContainer_element["cam.referencePosition_element"].toObject();
    QJsonObject itsAltitudeElement = camReferencePositionElement["its.altitude_element"].toObject();
    qreal pLat = (camReferencePositionElement["its.latitude"].toString().toDouble())/ 10000000.0;
    qreal pLong = (camReferencePositionElement["its.longitude"].toString().toDouble())/ 10000000.0;
    qreal pAlt = (itsAltitudeElement["its.altitudeValue"].toString().toDouble())/ 100.0;

    // *** ITS PDU HEADER ELEMENT *** //
    QJsonObject its_ItsPduHeader_element = its["its.ItsPduHeader_element"].toObject();
    int stationType = cam_basicContainer_element["cam.stationType"].toString().toInt();
    int messageID = its_ItsPduHeader_element["its.messageID"].toString().toInt();
    long stationID = its_ItsPduHeader_element["its.stationID"].toString().toLong();

    std::shared_ptr<Cam> newMessage = std::make_shared<Cam>(pLong, pLat, pAlt, messageID, stationID, stationType, pSpeed, pHeading,
                           path, vehicleLength, vehicleWidth, vehicleRole, lowBeamHeadlightsOn, highBeamHeadlightsOn,
                           leftTurnSignalOn, rightTurnSignalOn, daytimeRunningLightsOn, reverseLightOn, fogLightOn, parkingLightsOn, timeEpoch);

    emit messageParsed(newMessage);
}
void MessageParser::parseDENM(QJsonObject packetObj){
    QJsonObject _source = packetObj["_source"].toObject();
    QJsonObject layers = _source["layers"].toObject();

    QJsonObject its = layers["its"].toObject();
    QJsonObject its_ItsPduHeader_element = its["its.ItsPduHeader_element"].toObject();

    long stationID = its_ItsPduHeader_element["its.stationID"].toString().toLong();
    int messageID = its_ItsPduHeader_element["its.messageID"].toString().toInt();

    QJsonObject denmDecentralizedEnvironmentalNotificationMessageElement = its["denm.DecentralizedEnvironmentalNotificationMessage_element"].toObject();
    QJsonObject denmManagementElement = denmDecentralizedEnvironmentalNotificationMessageElement["denm.management_element"].toObject();

    QJsonObject denmActionID_element = denmManagementElement["denm.actionID_element"].toObject();
    long itsOriginatingStationID = denmActionID_element["its.originatingStationID"].toString().toLong();
    int itsSequenceNumber = denmActionID_element["its.sequenceNumber"].toString().toInt();

    long denmDetectionTime = denmManagementElement["denm.detectionTime"].toString().toLong();
    long denmReferenceTime = denmManagementElement["denm.referenceTime"].toString().toLong();
    bool denmTermination = denmManagementElement["denm.termination"].toString().toInt(); // TODO - convert to bool

    QJsonObject denmEventPosition_element = denmManagementElement["denm.eventPosition_element"].toObject();
    qreal itsLat = (denmEventPosition_element["its.latitude"].toString().toDouble())/ 10000000.0;
    qreal itsLong = (denmEventPosition_element["its.longitude"].toString().toDouble())/ 10000000.0;

    QJsonObject itsAltitudeElement = denmEventPosition_element["its.altitude_element"].toObject();
    qreal itsAlt = (itsAltitudeElement["its.altitudeValue"].toString().toDouble())/ 100.0;

    int denmStationType = denmManagementElement["denm.stationType"].toString().toInt();

    // SITUATION ELEMENT
    QJsonObject denmSituationElement = denmDecentralizedEnvironmentalNotificationMessageElement["denm.situation_element"].toObject();
    QJsonObject denmEventTypeElement = denmSituationElement["denm.eventType_element"].toObject();

    int itsCauseCode = denmEventTypeElement["its.causeCode"].toString().toInt();
    int itsSubCauseCode = denmEventTypeElement["its.subCauseCode"].toString().toInt();

    // LOCATION ELEMENT
    QJsonObject denmLocationElement = denmDecentralizedEnvironmentalNotificationMessageElement["denm.location_element"].toObject();

    int denmTraces = denmLocationElement["denm.traces"].toString().toInt();
    // TODO - path history

    std::shared_ptr<Denm> newMessage = std::make_shared<Denm>(itsLong, itsLat, itsAlt, messageID, stationID, denmStationType, itsOriginatingStationID, itsSequenceNumber, denmDetectionTime, denmReferenceTime, denmTermination, itsCauseCode, itsSubCauseCode);

    emit messageParsed(newMessage);

}
void MessageParser::parseSREM(QJsonObject packetObj){
    QJsonObject _source = packetObj["_source"].toObject();
    QJsonObject layers = _source["layers"].toObject();

    QJsonObject its = layers["its"].toObject();
    QJsonObject its_ItsPduHeader_element = its["its.ItsPduHeader_element"].toObject();

    long stationID = its_ItsPduHeader_element["its.stationID"].toString().toLong();
    int messageID = its_ItsPduHeader_element["its.messageID"].toString().toInt();

    QJsonObject dsrcSignalRequestMessageElement = its["dsrc.SignalRequestMessage_element"].toObject();
    float dsrcTimeStamp = dsrcSignalRequestMessageElement["dsrc.timeStamp"].toString().toFloat();
    int dsrcSequenceNumber = dsrcSignalRequestMessageElement["dsrc.sequenceNumber"].toString().toInt();

    int dsrcRequests = dsrcSignalRequestMessageElement["dsrc.requests"].toString().toInt();
    QJsonObject dsrcRequestsTree = dsrcSignalRequestMessageElement["dsrc.requests_tree"].toObject();

    QVector <Request> requests;
    for(int i = 0; i < dsrcRequests; i++) {
        QJsonObject item_i = dsrcRequestsTree["Item " + QString::number(i)].toObject();
        QJsonObject dsrcSignalRequestPackageElement = item_i["dsrc.SignalRequestPackage_element"].toObject();
        QJsonObject dsrcRequestElement = dsrcSignalRequestPackageElement["dsrc.request_element"].toObject();
        QJsonObject dsrcIDElement = dsrcRequestElement["dsrc.id_element"].toObject();

        int dsrcID = dsrcIDElement["dsrc.id"].toString().toInt();
        int dsrcRequestID = dsrcRequestElement["dsrc.requestID"].toString().toInt();
        int dsrcRequestType = dsrcRequestElement["dsrc.requestType"].toString().toInt();

        int dsrcInBoundLane = dsrcRequestElement["dsrc.inBoundLane"].toString().toInt();
        int dsrcOutBoundLane = dsrcRequestElement["dsrc.outBoundLane"].toString().toInt();

        QJsonObject dsrcInBoundLaneTree = dsrcRequestElement["dsrc.inBoundLane_tree"].toObject();
        QJsonObject dsrcOutBoundLaneTree = dsrcRequestElement["dsrc.outBoundLane_tree"].toObject();

        int dsrcApproachInBound = dsrcInBoundLaneTree["dsrc.approach"].toString().toInt();
        int dsrcApproachOutBound = dsrcOutBoundLaneTree["dsrc.approach"].toString().toInt();

        Request request;
        request.ID = dsrcID;
        request.requestID = dsrcRequestID;
        request.requestType = dsrcRequestType;
        request.inBoundLane = dsrcInBoundLane;
        request.outBoundLane = dsrcOutBoundLane;
        request.approachInBound = dsrcApproachInBound;
        request.approachOutBound = dsrcApproachOutBound;
        request.requestType = dsrcRequestType;
        requests.append(request);
    }
    QJsonObject dsrcRequestorElement = dsrcSignalRequestMessageElement["dsrc.requestor_element"].toObject();
    QJsonObject dsrcIdTree = dsrcRequestorElement["dsrc.id_tree"].toObject();
    long dsrcStationID = dsrcIdTree["dsrc.stationID"].toString().toLong();

    QJsonObject dsrcTypeElement = dsrcRequestorElement["dsrc.type_element"].toObject();
    int dsrcRole = dsrcTypeElement["dsrc.role"].toString().toInt();
    int dsrcSubrole = dsrcTypeElement["dsrc.subrole"].toString().toInt();

    QString dsrcName = dsrcRequestorElement["dsrc.name"].toString();
    QString dsrcRouteName = dsrcRequestorElement["dsrc.routeName"].toString();

    std::shared_ptr<Srem> newMessage = std::make_shared<Srem>(-1, -1, -1, messageID, stationID, 0, dsrcTimeStamp, dsrcSequenceNumber, requests, dsrcStationID, dsrcRole, dsrcSubrole, dsrcName, dsrcRouteName);

    emit messageParsed(newMessage);
}
void MessageParser::parseSPATEM(QJsonObject packetObj){
    QJsonObject _source = packetObj["_source"].toObject();
    QJsonObject layers = _source["layers"].toObject();

    QJsonObject its = layers["its"].toObject();
    QJsonObject its_ItsPduHeader_element = its["its.ItsPduHeader_element"].toObject();

    QJsonObject dsrcSPATElement = its["dsrc.SPAT_element"].toObject();

    int dsrcIntersections = dsrcSPATElement["dsrc.intersections"].toString().toInt();

    if(dsrcIntersections == 1){
        QJsonObject dsrcIntersectionsTree = dsrcSPATElement["dsrc.intersections_tree"].toObject();
        QJsonObject Item0 = dsrcIntersectionsTree["Item 0"].toObject();

        QJsonObject dsrcIntersectionStateElement = Item0["dsrc.IntersectionState_element"].toObject();
        QString name = dsrcIntersectionStateElement["dsrc.name"].toString();
        int dsrcTimeStamp = dsrcIntersectionStateElement["dsrc.timeStamp"].toString().toInt();
        int moy = dsrcIntersectionStateElement["dsrc.moy"].toString().toInt();

        QJsonObject dsrcIdElement = dsrcIntersectionStateElement["dsrc.id_element"].toObject();
        int dsrcId = dsrcIdElement["dsrc.id"].toString().toInt();

        // TODO intersection status
        int dsrcStates = dsrcIntersectionStateElement["dsrc.states"].toString().toInt();

        QJsonObject dsrcStatesTree = dsrcIntersectionStateElement["dsrc.states_tree"].toObject();
        QVector <MovementState> movementStates;

        for(int i = 0; i < dsrcStates; i++) {
            QJsonObject item_i = dsrcStatesTree["Item " + QString::number(i)].toObject();
            QJsonObject dsrcMovementStateElement = item_i["dsrc.MovementState_element"].toObject();

            QString dsrcMovementName = dsrcMovementStateElement["dsrc.movementName"].toString();
            int dsrcSignalGroup = dsrcMovementStateElement["dsrc.signalGroup"].toString().toInt();

            int dsrcStateTimeSpeed = dsrcMovementStateElement["dsrc.state_time_speed"].toString().toInt();

            QJsonObject dsrcStateTimeSpeedTree = dsrcMovementStateElement["dsrc.state_time_speed_tree"].toObject();
            QVector <MovementEvent> movementEvents;

            for(int j = 0; j < dsrcStateTimeSpeed; j++) {
                QJsonObject item_j = dsrcStateTimeSpeedTree["Item " + QString::number(j)].toObject();
                QJsonObject dsrcMovementEventElement = item_j["dsrc.MovementEvent_element"].toObject();

                int dsrcEventState = dsrcMovementEventElement["dsrc.eventState"].toString().toInt();

                QJsonObject dsrcTimingElement = dsrcMovementEventElement["dsrc.timing_element"].toObject();

                int dsrcStartTime = dsrcTimingElement["dsrc.startTime"].toString().toInt();
                int dsrcMinEndTime = dsrcTimingElement["dsrc.minEndTime"].toString().toInt();
                int dsrcMaxEndTime = dsrcTimingElement["dsrc.maxEndTime"].toString().toInt();
                int dsrcLikelyTime = dsrcTimingElement["dsrc.likelyTime"].toString().toInt();
                int dsrcConfidence = dsrcTimingElement["dsrc.confidence"].toString().toInt();

                MovementEvent movementEvent;
                movementEvent.eventState = dsrcEventState;
                movementEvent.startTime = dsrcStartTime;
                movementEvent.minEndTime = dsrcMinEndTime;
                movementEvent.maxEndTime = dsrcMaxEndTime;
                movementEvent.likelyTime = dsrcLikelyTime;
                movementEvent.confidence = dsrcConfidence;
                movementEvents.append(movementEvent);
            }
            MovementState movementState;
            movementState.signalGroup = dsrcSignalGroup;
            movementState.movementName = dsrcMovementName;
            movementState.movementEvents = movementEvents;
            movementStates.append(movementState);
        }

        long stationID = its_ItsPduHeader_element["its.stationID"].toString().toLong();
        int messageID = its_ItsPduHeader_element["its.messageID"].toString().toInt();

        std::shared_ptr<Spatem> newMessage = std::make_shared<Spatem>(-1, -1, -1, messageID, stationID, 0, name, dsrcId, dsrcTimeStamp, moy, movementStates);

        emit messageParsed(newMessage);
    }
    else{
        // TODO
        throw std::invalid_argument("You have not finished visualizing crossroad with more intersections than one");
    }

}
void getGEONWInfo(QJsonObject geonw, int * geonwSrcPosAddrManual, int * geonwSrcPosAddrType, int * geonwSrcPosAddrCountry,
                  float * geonwSrcPosTst, float * geonwSrcPosLat, float * geonwSrcPosLong, float * geonwSrcPosSpeed, float * geonwSrcPosHeading)
{
    QString geonwSrcPos = geonw["geonw.src_pos"].toString();

    QJsonObject geonwSrcPosTree = geonw["geonw.src_pos_tree"].toObject();
    QString geonwSrcPosAddr = geonwSrcPosTree["geonw.src_pos.addr"].toString();

    QJsonObject geonwSrcPosAddrTree = geonwSrcPosTree["geonw.src_pos.addr_tree"].toObject();
    * geonwSrcPosAddrManual = geonwSrcPosAddrTree["geonw.src_pos.addr.manual"].toString().toInt();
    * geonwSrcPosAddrType = geonwSrcPosAddrTree["geonw.src_pos.addr.type"].toString().toInt();
    * geonwSrcPosAddrCountry = geonwSrcPosAddrTree["geonw.src_pos.addr.country"].toString().toInt();

    * geonwSrcPosTst = geonwSrcPosTree["geonw.src_pos.tst"].toString().toFloat();
    * geonwSrcPosLat = (geonwSrcPosTree["geonw.src_pos.lat"].toString().toFloat())/ 10000000.0f;
    * geonwSrcPosLong = (geonwSrcPosTree["geonw.src_pos.long"].toString().toFloat())/ 10000000.0f;
    * geonwSrcPosSpeed = ((geonwSrcPosTree["geonw.src_pos.speed"].toString().toFloat())/ 100) * 3.6f;
    * geonwSrcPosHeading = ((geonwSrcPosTree["geonw.src_pos.hdg"].toString().toFloat())/ 10) - 90.0f;

}
void MessageParser::parseGEONW(QJsonObject packetObj){
    QJsonObject _source = packetObj["_source"].toObject();
    QJsonObject layers = _source["layers"].toObject();
    QJsonObject gnw = layers["gnw"].toObject();

    // *** BASIC HEADER *** //
    QJsonObject geonwBasicHeader = gnw["geonw.bh"].toObject();
    int geonwBHversion = geonwBasicHeader["geonw.bh.version"].toString().toInt();
    // TODO - get information what is the next header
    int geonwBHRemainingHopCount = geonwBasicHeader["geonw.bh.rhl"].toString().toInt();

    // *** COMMON HEADER *** //
    QJsonObject geonwCommonHeader = gnw["geonw.ch"].toObject();
    int geonwCHTrafficClass = geonwCommonHeader["geonw.ch.tclass"].toString().toInt();
    int geonwCHPayloadLength = geonwCommonHeader["geonw.ch.plength"].toString().toInt();

    int geonwCHNextHeader = geonwCommonHeader["geonw.ch.nh"].toString().toInt();
    QString geonwCHHeaderType = geonwCommonHeader["geonw.ch.htype"].toString();

    int geonwSrcPosAddrManual, geonwSrcPosAddrType, geonwSrcPosAddrCountry;
    float geonwSrcPosTst, geonwSrcPosLat, geonwSrcPosLong, geonwSrcPosSpeed, geonwSrcPosHeading;

    // *** TBC or GBC HEADER *** //
    if(geonwCHNextHeader == 2 && geonwCHHeaderType == "0x50"){
        QJsonObject geonwTopologicallyScopedPacket = gnw["geonw.tsb"].toObject();
        getGEONWInfo(geonwTopologicallyScopedPacket, &geonwSrcPosAddrManual, &geonwSrcPosAddrType, &geonwSrcPosAddrCountry,
                     &geonwSrcPosTst, &geonwSrcPosLat, &geonwSrcPosLong, &geonwSrcPosSpeed, &geonwSrcPosHeading);

    } else if(geonwCHNextHeader == 2 && geonwCHHeaderType == "0x40"){
        QJsonObject geoBroadcastPacket = gnw["geonw.gbc"].toObject();
        getGEONWInfo(geoBroadcastPacket, &geonwSrcPosAddrManual, &geonwSrcPosAddrType, &geonwSrcPosAddrCountry,
                     &geonwSrcPosTst, &geonwSrcPosLat, &geonwSrcPosLong, &geonwSrcPosSpeed, &geonwSrcPosHeading);

        int geonwGxcRadius = geoBroadcastPacket["geonw.gxc.radius"].toString().toInt();
        // TODO - the rest if necessary
    }
    else if(geonwCHNextHeader == 0 && geonwCHHeaderType == "0x10"){
        QJsonObject geoBeaconPacket = gnw["geonw.beacon"].toObject();
        getGEONWInfo(geoBeaconPacket, &geonwSrcPosAddrManual, &geonwSrcPosAddrType, &geonwSrcPosAddrCountry,
                     &geonwSrcPosTst, &geonwSrcPosLat, &geonwSrcPosLong, &geonwSrcPosSpeed, &geonwSrcPosHeading);
    }
    // TODO - what to do with GEONW messages
}
void MessageParser::parseMAPEM(QJsonObject packetObj){
    QJsonObject _source = packetObj["_source"].toObject();
    QJsonObject layers = _source["layers"].toObject();

    QJsonObject its = layers["its"].toObject();
    QJsonObject its_ItsPduHeader_element = its["its.ItsPduHeader_element"].toObject();

    QJsonObject itsDsrcMapDataElement = its["dsrc.MapData_element"].toObject();
    QJsonObject dsrcIntersectionsTree = itsDsrcMapDataElement["dsrc.intersections_tree"].toObject();

    QJsonObject item0 = dsrcIntersectionsTree["Item 0"].toObject();
    QJsonObject dsrcIntersectionGeometryElement = item0["dsrc.IntersectionGeometry_element"].toObject();

    QJsonObject dsrcRefPointElement = dsrcIntersectionGeometryElement["dsrc.refPoint_element"].toObject();
    QJsonObject dsrcLaneSetTree = dsrcIntersectionGeometryElement["dsrc.laneSet_tree"].toObject();

    int dsrcLaneSetNumber = dsrcIntersectionGeometryElement["dsrc.laneSet"].toString().toInt();

    QVector <Lane> lanes;

    for(int i = 0; i < dsrcLaneSetNumber; i++) {

        QJsonObject item_i = dsrcLaneSetTree["Item " + QString::number(i)].toObject();
        QJsonObject dsrcGenericLaneElement = item_i["dsrc.GenericLane_element"].toObject();

        // Lane node points
        QJsonObject dsrcNodeListTree = dsrcGenericLaneElement["dsrc.nodeList_tree"].toObject();
        QJsonObject dsrcNodesTree = dsrcNodeListTree["dsrc.nodes_tree"].toObject();
        int dsrcNodes = dsrcNodeListTree["dsrc.nodes"].toString().toInt();

        QVector <Node> nodes;
        for(int j = 0; j < dsrcNodes; j++) {
            QJsonObject item_j = dsrcNodesTree["Item " + QString::number(j)].toObject();
            QJsonObject dsrcNodeXYElement = item_j["dsrc.NodeXY_element"].toObject();
            QJsonObject dsrcDelta_tree = dsrcNodeXYElement["dsrc.delta_tree"].toObject();

            int dsrcDelta = dsrcNodeXYElement["dsrc.delta"].toString().toInt();

            QJsonObject dsrcNode_XY_num_element = dsrcDelta_tree["dsrc.node_XY" + QString::number(dsrcDelta + 1) + "_element"].toObject();

            int dsrcX = dsrcNode_XY_num_element["dsrc.x"].toString().toInt();
            int dsrcY = dsrcNode_XY_num_element["dsrc.y"].toString().toInt();

            Node node;
            node.x = dsrcX;
            node.y = dsrcY;
            node.delta = dsrcDelta;
            nodes.append(node);
        }

        QJsonObject dsrcLaneAttributesElement = dsrcGenericLaneElement["dsrc.laneAttributes_element"].toObject();
        //QJsonObject dsrcLaneTypeTree = dsrcLaneAttributesElement["dsrc.laneType_tree"].toObject();

        int dsrcLaneType = dsrcLaneAttributesElement["dsrc.laneType"].toString().toInt();
        long dsrcLaneID = dsrcGenericLaneElement["dsrc.laneID"].toString().toLong();
        int dsrcIngressApproach = dsrcGenericLaneElement["dsrc.ingressApproach"].toString().toInt();
        int dsrcEgressApproach = dsrcGenericLaneElement["dsrc.egressApproach"].toString().toInt();

        QJsonObject dsrcDirectionalUseTree = dsrcLaneAttributesElement["dsrc.directionalUse_tree"].toObject();
        bool dsrcLaneDirectionIngressPath = dsrcDirectionalUseTree["dsrc.LaneDirection.ingressPath"].toString().toInt();
        bool dsrcLaneDirectionEgressPath = dsrcDirectionalUseTree["dsrc.LaneDirection.egressPath"].toString().toInt();

        // get connecting lanes
        int dsrcConnectsTo = dsrcGenericLaneElement["dsrc.connectsTo"].toString().toInt();
        QJsonObject dsrcConnectsToTree = dsrcGenericLaneElement["dsrc.connectsTo_tree"].toObject();

        QVector <ConnectingLane> connectingLanes;
        for(int k = 0; k < dsrcConnectsTo; k++) {
            QJsonObject item_k = dsrcConnectsToTree["Item " + QString::number(k)].toObject();
            QJsonObject dsrcConnectionElement = item_k["dsrc.Connection_element"].toObject();
            QJsonObject dsrcConnectingLaneElement = dsrcConnectionElement["dsrc.connectingLane_element"].toObject();

            int dsrcLane = dsrcConnectingLaneElement["dsrc.lane"].toString().toInt();
            int dsrcSignalGroup = dsrcConnectionElement["dsrc.signalGroup"].toString().toInt();
            long dsrcConnectionID = dsrcConnectionElement["dsrc.connectionID"].toString().toLong();

            QJsonObject dsrcManeuverTree = dsrcConnectingLaneElement["dsrc.maneuver_tree"].toObject();
            bool maneuverStraightAllowed = dsrcManeuverTree["dsrc.AllowedManeuvers.maneuverStraightAllowed"].toString().toInt();
            bool maneuverLeftAllowed = dsrcManeuverTree["dsrc.AllowedManeuvers.maneuverLeftAllowed"].toString().toInt();
            bool maneuverRightAllowed = dsrcManeuverTree["dsrc.AllowedManeuvers.maneuverRightAllowed"].toString().toInt();

            ConnectingLane cLane;
            cLane.laneNumber = dsrcLane;
            cLane.connectionID = dsrcConnectionID;
            cLane.signalGroup = dsrcSignalGroup;
            cLane.maneuverStraightAllowed = maneuverStraightAllowed;
            cLane.maneuverLeftAllowed = maneuverLeftAllowed;
            cLane.maneuverRightAllowed = maneuverRightAllowed;
            connectingLanes.append(cLane);
        }

        Lane lane;
        lane.type = dsrcLaneType;
        if(lane.type >= Mapem::laneTypes.size()){
            throw std::invalid_argument("received non existing value");
        }
        lane.strType = Mapem::laneTypes.at(dsrcLaneType);
        lane.laneID = dsrcLaneID;
        lane.ingressApproach = dsrcIngressApproach;
        lane.egressApproach = dsrcEgressApproach;
        lane.directionIngressPath = dsrcLaneDirectionIngressPath;
        lane.directionEgressPath = dsrcLaneDirectionEgressPath;
        lane.nodes = nodes;
        lane.connectingLanes = connectingLanes;
        lanes.append(lane);        
    }

    qreal dsrcLat = (dsrcRefPointElement["dsrc.lat"].toString().toDouble())/ 10000000.0;
    qreal dsrcLong = (dsrcRefPointElement["dsrc.long"].toString().toDouble())/ 10000000.0;
    qreal dsrcElevation = (dsrcRefPointElement["dsrc.position3D.elevation"].toString().toDouble())/ 10.0;

    float dsrcLaneWidth = (dsrcIntersectionGeometryElement["dsrc.laneWidth"].toString().toFloat())/ 100.0f;
    QString dsrcName = dsrcIntersectionGeometryElement["dsrc.name"].toString();

    long stationID = its_ItsPduHeader_element["its.stationID"].toString().toLong();
    int messageID = its_ItsPduHeader_element["its.messageID"].toString().toInt();

    QJsonObject dsrcIdElement = dsrcIntersectionGeometryElement["dsrc.id_element"].toObject();
    long dsrcID = dsrcIdElement["dsrc.id"].toString().toLong();

    // TODO - is 0 as stationType unknown?
    std::shared_ptr<Mapem> newMessage = std::make_shared<Mapem>(dsrcLong, dsrcLat, dsrcElevation, messageID, stationID, 0, dsrcLaneWidth, dsrcName, dsrcID, lanes);
    emit messageParsed(newMessage);
}
