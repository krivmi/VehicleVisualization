#include "datahandler.h"

#include <math.h>
#include <algorithm>
#include <memory>

# define M_PI 3.14159265358979323846

const double R = 6378.137; // Radius of earth in KM

DataHandler::DataHandler(Visualizer * visualizer, QWidget* parent) : QObject(parent),
    autoModeOn(false), visualizer(visualizer)
{
    this->gpsInfo.longitude = -1;
    this->gpsInfo.latitude = -1;

    // create timer that is going to handle crossroad proximity every second
    handleCrossroadProximity();

    // create timer that checks if a Cam unit has a request status based on receiving SREM
    checkCamsForSrem();
}
DataHandler::~DataHandler(){
    for(int i = 0; i < trafficLightStructVector.size(); i++){
        for(int j = 0; j < trafficLightStructVector[i].widgets.size(); j++){
            delete trafficLightStructVector[i].widgets[j];
        }
    }
}
void DataHandler::MessageReceived(std::shared_ptr<Message> message){

    QString protocol = message->GetProtocol();

    // create a deep copy of object after casting
    if(protocol == "Cam"){
        std::shared_ptr<Cam> newCam = std::make_shared<Cam>(*std::static_pointer_cast<Cam>(message));
        this->CAMReceived(newCam);
    } else if(protocol == "Mapem"){
        std::shared_ptr<Mapem> newMapem = std::make_shared<Mapem>(*std::static_pointer_cast<Mapem>(message));
        this->MAPEMReceived(newMapem);
    } else if(protocol == "Spatem"){
        std::shared_ptr<Spatem> newSpatem = std::make_shared<Spatem>(*std::static_pointer_cast<Spatem>(message));
        this->SPATEMReceived(newSpatem);
    } else if(protocol == "Srem"){
        std::shared_ptr<Srem> newSrem = std::make_shared<Srem>(*std::static_pointer_cast<Srem>(message));
        this->SREMReceived(newSrem);
    } else if(protocol == "Denm"){
        std::shared_ptr<Denm> newDenm = std::make_shared<Denm>(*std::static_pointer_cast<Denm>(message));
        this->DENMReceived(newDenm);
    }
    else{ qInfo() << "Something went wrong"; }

    if(true /* !autoModeOn*/){
        this->addMessage(message);
    }

    emit MessageToLog(message);
}
void DataHandler::clearData(){
    clearUnitsAndMessages();
    deleteAllMessages();
}
void DataHandler::clearUnitsAndMessages(){
    deleteCamUnits();
    deleteMapemUnits();
    deleteSpatemMessages();
    deleteDenmMessages();
}
void DataHandler::deleteCamUnits(){
    camUnits.clear();
}
void DataHandler::deleteMapemUnits(){
    mapemUnits.clear();
}
void DataHandler::deleteSpatemMessages(){
    spatemMessages.clear();
}
void DataHandler::deleteDenmMessages(){
    denmMessages.clear();
}
void DataHandler::deleteAllMessages(){
    allMessages.clear();
}

void DataHandler::CAMReceived(std::shared_ptr<Cam> newCam){

    Cam * oldCam = this->getCamUnitByID(newCam->stationID);

    if(oldCam == nullptr){
        // station was not found in other units, display this new station on the map
        visualizer->addCamStationToLayer(newCam.get());

        this->addCamUnit(newCam);
    } else {
        // if the station is already in the list, update the unit
        oldCam->update(newCam);

        // TODO - vymyslet jak to zbytečně nemazat a nealokovat pořád znovu
        visualizer->removeGeometry(oldCam->geometryPoint);
        visualizer->addCamStationToLayer(oldCam);

        //oldCam->geometryPoint->setCoord(PointWorldCoord(oldCam->longitude, oldCam->latitude));
        //oldCam->point->setCoord(newUnit->refPoint);

        // display bounding box
        //RectWorldCoord rect = oldCam->geometryPoint->boundingBox(19);
        //const std::vector<PointWorldCoord> points = {rect.topLeftCoord(), rect.topRightCoord(), rect.bottomRightCoord(), rect.bottomLeftCoord()};
        //oldCam->polygon->setPoints(points);

        // Rotation will be applied to the center of the shape.
        //oldCam->geometryPoint->setRotation(oldUnit->heading);

        // if there is a station currently displayed in the info element, make update
        if(currentInfoStation != nullptr){
            if(oldCam->stationID == currentInfoStation->stationID){
                emit changeInfo(oldCam->stationID);
            }
        }
    }
}
void DataHandler::handleCrossroadProximity()
{
    if(autoModeOn)
    {
        Mapem * currentCrossroad = getClosestCrossroad();

        if(currentCrossroad != nullptr ){
            if(!trafficLightShown){
                int orientations[gpsHistory.size()];
                for(int i = 0; i < gpsHistory.size(); i++){
                     orientations[i] = gpsHistory.at(i).orientation;
                }
                std::sort(orientations, orientations + gpsHistory.size());
                // take median of sorted orientations
                float median = orientations[(int)gpsHistory.size() / 2];

                if(median > 0){
                    int index = currentCrossroad->findAdjIngVehLaneByOrientation(median);

                    if(index != -1){
                        trafficLightShown = true;
                        emit trafficLightShow(currentCrossroad->crossroadID, index, true);
                    }
                }
            }
        } else{
            if(trafficLightShown){
                trafficLightShown = false;
                emit trafficLightHide();
            }
        }
    }

    QTimer::singleShot(1000, this, SLOT(handleCrossroadProximity()));
}
double measureDistance(qreal lat1, qreal lon1, qreal lat2, qreal lon2)
{
    // generally used geo measurement function
    double dLat = lat2 * M_PI / 180 - lat1 * M_PI / 180;
    double dLon = lon2 * M_PI / 180 - lon1 * M_PI / 180;
    double a = sin(dLat/2) * sin(dLat/2) +
        cos(lat1 * M_PI / 180) * cos(lat2 * M_PI / 180) *
        sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = R * c;

    return d * 1000; // convert to meters
}
Mapem * DataHandler::getClosestCrossroad()
{
    // TODO - pass GPS position as parameter
    QMutex mutex;

    Mapem * crossroad = nullptr;
    double minDistance = 10000000000;
    // ensure integrity if mapem is to be deleted
    mutex.lock();
    for(int i = 0; i < mapemUnits.size(); i++){
        PointWorldCoord gpsPosn = PointWorldCoord(gpsInfo.longitude, gpsInfo.latitude);
        PointWorldCoord crossPos = mapemUnits.at(i)->refPoint;

        double distance = measureDistance(gpsPosn.latitude(), gpsPosn.longitude(), crossPos.latitude(), crossPos.longitude());

        if(distance < minDistance){
            crossroad = mapemUnits.at(i).get();
            minDistance = distance;
        }
    }
    mutex.unlock();


    if(minDistance < 60.0){
        //qInfo() << crossroad;
        return crossroad;
    } else {
        return nullptr;
    }
}

void DataHandler::MAPEMReceived(std::shared_ptr<Mapem> newMapem){
    Mapem * oldMapem = getMapemByCrossroadID(newMapem->crossroadID);

    if(oldMapem == nullptr){
        this->addMapemUnit(newMapem);

        //visualizer->drawReferencePoint(newMapem->refPoint, QColor("red"));
        visualizer->drawLanes(newMapem->lanes, newMapem->refPoint, 0, true); // lanes, refPoint, laneType, drawPointsOfLane
        //visualizer->drawLanesConnections(newMapem->lanes, newMapem->refPoint, QColor("green"));

        newMapem->prepareCrossroadSignalGroups();

        addTrafficLightsFromSignalGroups(newMapem.get());
    }
    else {
        //qInfo() << "This crossroad has been already visualized...";
    }
}

void DataHandler::SPATEMReceived(std::shared_ptr<Spatem> newSPATEM)
{
    Spatem * oldSpatem = findSpatemByTime(newSPATEM->moy, newSPATEM->timeStamp);
    // check if the same SPATEM has been already received

    if(oldSpatem != nullptr) {
        //qInfo() << "the same SPATEM has been already received";
        return;
    }

    spatemMessages.append(newSPATEM);

    // find according crossroad for SPATEM
    Mapem * crossroad = getMapemByCrossroadID(newSPATEM->crossroadID);

    if(crossroad != nullptr){
        // get the time information of SPATEM
        int timeStamp = newSPATEM->timeStamp;
        int moy = newSPATEM->moy;

        // go through all the widgets that defines different directions in the crossroad
        // and change their state by signal group
        for(int i = 0; i < trafficLightStructVector.size(); i++){
            for(int j = 0; j < trafficLightStructVector[i].widgets.size(); j++){
                TrafficLightWidget * w = trafficLightStructVector[i].widgets[j];
                int trafficLightWidgetSignalGroup = w->signalGroup;

                // for each signal state, change residual time and trafficLight image of the widget
                for(int k = 0; k < newSPATEM->movementStates.size(); k++){
                    if(newSPATEM->movementStates[k].signalGroup == trafficLightWidgetSignalGroup){
                        int signalGroupState = newSPATEM->movementStates[k].movementEvents[0].eventState;

                        int likelyTime = newSPATEM->movementStates[k].movementEvents[0].likelyTime;

                        w->setResidualTime(timeStamp, moy, likelyTime);
                        w->setTrafficLightPixmap(signalGroupState);

                        //visualizer->changeTrafficLight(w->trafficLightMapPoint, signalGroupState);
                        break;
                    }
                }
            }
        }
    }
}

void DataHandler::SREMReceived(std::shared_ptr<Srem> newSrem)
{
    Cam * senderCam = getCamUnitByID(newSrem->requestorID);

    if(senderCam != nullptr){
        QPixmap img(senderCam->imgSrcAttention);
        senderCam->geometryPoint->setImage(img); // update shape to !!!
        senderCam->lastSremUpdate = QTime::currentTime();
        senderCam->isSrcAttention = true;
    }
}
void DataHandler::checkCamsForSrem(){
    // *** should I use mutex because of deleting?
    for(int i = 0 ; i < camUnits.size(); i++){
        if(!camUnits.at(i)->lastSremUpdate.isNull()){
            QTime now = QTime::currentTime();
            int diff = camUnits.at(i)->lastSremUpdate.secsTo(now);
            //qInfo() << diff;

            if(diff >= 10){
                camUnits.at(i)->lastSremUpdate = QTime(); // set to null
                camUnits.at(i)->isSrcAttention = false;

                QPixmap img(camUnits.at(i)->imgSrcDefault);
                camUnits.at(i)->geometryPoint->setImage(img); // update shape back
            }
        }
    }
    QTimer::singleShot(2000, this, SLOT(checkCamsForSrem()));
}
void DataHandler::DENMReceived(std::shared_ptr<Denm> newDENM){

    Denm * oldDenm = getDenmByActionID(newDENM->stationID, newDENM->sequenceNumber);

    if(newDENM->termination){
        // TODO: terminate denm
    }

    if(oldDenm == nullptr){
        this->addDenmMessage(newDENM);

        visualizer->addDenmHazardToLayer(newDENM.get());
    }
    else {
        // TODO: update denm
    }

}

void DataHandler::addTrafficLightsFromSignalGroups(Mapem * crossroad)
{
    // for every set of adjacent ingress lanes (for every direction || or | or |||)
    for(int i = 0; i < crossroad->adjacentIngressLanes.size(); i++){
        // if adjacent ingress lanes have the same traffic light group
        if(crossroad->haveAdjacentLanesSameSignalGroup(crossroad->adjacentIngressLanes[i])){
            // bool sameTrafficLight = true if lanes in the group are adjacent
            // int index = crossroad->adjacentIngressLanes[i]; <- i
            // create just one traffic light for all adjacent lanes
            createTrafficLightWidgets(crossroad->crossroadID, i, true);
        }
        else{
            // create corresponding number of traffic ligths for all adjacent lanes
            createTrafficLightWidgets(crossroad->crossroadID, i, false);
        }
    }
}

void DataHandler::createTrafficLightWidgets(int crossroadID, int adjacentLaneIndex, bool sameTrafficLight)
{
    Mapem * crossroad = getMapemByCrossroadID(crossroadID);

    QVector<Lane> adjacentIngressLanes = crossroad->adjacentIngressLanes[adjacentLaneIndex];

    if(sameTrafficLight){
        int signalGroup = adjacentIngressLanes[0].signalGroupInfoPairVector[0].first;

        QVector <TrafficLightWidget*> trafficLightWidgetVector;
        // for every lane that is in adjacent lanes
        // go from the last element to ensure right lane direction when visualizing
        for(int i = adjacentIngressLanes.size() - 1; i >= 0; i--){
            Lane lane = adjacentIngressLanes[i];

            // get maneuvers for each lane
            QVector <QString> maneuvers;
            for(int j = 0; j < lane.signalGroupInfoPairVector.size(); j++){
                for(int k = 0; k < lane.signalGroupInfoPairVector[j].second.size(); k++){
                    if(!maneuvers.contains(lane.signalGroupInfoPairVector[j].second[k].manouver)){
                        maneuvers.append(lane.signalGroupInfoPairVector[j].second[k].manouver);
                    }
                }
            }

            // create new widget for every lane and add it to the array
            TrafficLightWidget * light = new TrafficLightWidget(signalGroup, maneuvers, qobject_cast <QWidget*>(this->parent()));
            trafficLightWidgetVector.append(light);

            visualizer->addTrafficLight(crossroad, light->trafficLightMapPoint, adjacentLaneIndex, true);
        }
        // one structure can contain more traffic light widgets for more lanes even if they are the same signal group
        TrafficLightStruct trafficLightStruct;
        trafficLightStruct.crossroadID = crossroad->crossroadID;
        trafficLightStruct.adjacentIngressLaneIndex = adjacentLaneIndex;
        trafficLightStruct.sameSignalGroupForWidgets = sameTrafficLight;
        trafficLightStruct.widgets = trafficLightWidgetVector;

        this->trafficLightStructVector.append(trafficLightStruct);

    } else{
        // create the number of traffic lights according to number of signal groups of each lane
        // for each lane in adjacent ingress lanes

        QVector <TrafficLightWidget*> trafficLightWidgetVector;

        // for every lane that is in adjacent lanes
        // go from the last element to ensure right lane direction when visualizing
        for(int i = adjacentIngressLanes.size() - 1; i >= 0; i--){
            Lane lane = adjacentIngressLanes[i];

            // for every signal lane groups
            for(int j = 0; j < lane.signalGroupInfoPairVector.size(); j++){
                int signalGroup = lane.signalGroupInfoPairVector[j].first;

                // get maneuvers for each lane
                QVector <QString> maneuvers;
                for(int k = 0; k < lane.signalGroupInfoPairVector[j].second.size(); k++){
                    if(!maneuvers.contains(lane.signalGroupInfoPairVector[j].second[k].manouver)){
                        maneuvers.append(lane.signalGroupInfoPairVector[j].second[k].manouver);
                    }
                }

                // create new widget for every signalGroup and add it to the array
                TrafficLightWidget * light = new TrafficLightWidget(signalGroup, maneuvers, qobject_cast <QWidget*>(this->parent()));
                trafficLightWidgetVector.append(light);

                visualizer->addTrafficLight(crossroad, light->trafficLightMapPoint, adjacentLaneIndex, true);
            }
        }
    }
}

Message * DataHandler::getMessageByID(long id)
{
    for(int i = 0; i < allMessages.size(); i++){
        if(allMessages.at(i)->stationID == id){
            return allMessages.at(i).get();
        }
    }
    return nullptr;
}

Cam * DataHandler::getCamUnitByID(long id)
{
    for(int i = 0; i < camUnits.size(); i++){
        if(camUnits.at(i)->stationID == id){
            return camUnits.at(i).get();
        }
    }
    return nullptr;
}
Mapem * DataHandler::getMapemUnitByID(long id)
{
    for(int i = 0; i < mapemUnits.size(); i++){
        if(mapemUnits.at(i)->stationID == id){
            return mapemUnits.at(i).get();
        }
    }
    return nullptr;
}

Denm * DataHandler::getDenmByActionID(long stationID, int sequenceNumber)
{
    for(int i = 0; i < denmMessages.size(); i++){
        if(denmMessages.at(i)->originatingStationID == stationID &&
                denmMessages.at(i)->sequenceNumber == sequenceNumber){
            return denmMessages.at(i).get();
        }
    }
    return nullptr;
}
void DataHandler::addMessage(std::shared_ptr<Message> message){
    allMessages.push_back(message);
}
void DataHandler::addCamUnit(std::shared_ptr<Cam> unit){
    camUnits.push_back(unit);
}
void DataHandler::addMapemUnit(std::shared_ptr<Mapem> unit){
    mapemUnits.push_back(unit);
}
void DataHandler::addDenmMessage(std::shared_ptr<Denm> message){
    denmMessages.push_back(message);
}
void DataHandler::deleteMapemUnitByID(long id)
{
    for(int i = 0; i < mapemUnits.size(); i++){
        if(mapemUnits.at(i)->stationID == id){
            // remove lanes
            for(int j = 0; j < mapemUnits.at(i)->lanes.size(); j++){
                visualizer->removeGeometry(mapemUnits.at(i)->lanes[j].laneMapPtr);

                // remove lane points
                for(int k = 0; k < mapemUnits.at(i)->lanes.at(j).lanePointsVectorPtr.size(); k++){
                    visualizer->removeGeometry(mapemUnits.at(i)->lanes.at(j).lanePointsVectorPtr.at(k));
                }
            }

            // remove TL from map
            for(int j = 0; j < trafficLightStructVector.size(); j++){
                if(trafficLightStructVector.at(j).crossroadID == mapemUnits.at(i)->crossroadID){

                    // loop through every widget
                    for(int k = 0; k < trafficLightStructVector.at(j).widgets.size(); k++){
                        visualizer->removeGeometry(trafficLightStructVector.at(j).widgets.at(k)->trafficLightMapPoint);
                    }

                }
            }
            mapemUnits.removeAt(i);
            break;
        }
    }
}
void DataHandler::deleteDenmMessageByActionID(long stationID, int sequenceNumber)
{
    for(int i = 0; i < denmMessages.size(); i++){
        if(denmMessages.at(i)->originatingStationID == stationID && denmMessages.at(i)->sequenceNumber == sequenceNumber){
            visualizer->removeGeometry(denmMessages.at(i)->geometryPoint);
            denmMessages.removeAt(i);
            break;
        }
    }
}
void DataHandler::deleteCamUnitByID(long id)
{
    for(int i = 0; i < camUnits.size(); i++){
        if(camUnits.at(i)->stationID == id){
            visualizer->removeGeometry(camUnits.at(i)->geometryPoint);
            camUnits.removeAt(i);
            break;
        }
    }
}

Mapem * DataHandler::getMapemByCrossroadID(long id)
{
    for(int i = 0; i < mapemUnits.size(); i++){
        if(mapemUnits.at(i)->crossroadID == id){
            return mapemUnits.at(i).get();
        }
    }
    return nullptr;
}
Spatem * DataHandler::findSpatemByTime(int moy, int timeStamp)
{
    for(int i = 0; i < spatemMessages.size(); i++){
        if(spatemMessages.at(i)->moy == moy && spatemMessages.at(i)->timeStamp == timeStamp){
            return spatemMessages.at(i).get();
        }
    }
    return nullptr;
}

void DataHandler::GPSPositionReceived(PointWorldCoord position, float orientation)
{
    qInfo() << "Longitude: " << position.longitude() << "latitude: " << position.latitude() << "orientation: " << orientation;

    gpsInfo.longitude = position.longitude();
    gpsInfo.latitude = position.latitude();
    gpsInfo.orientation = orientation;
    gettimeofday(&gpsInfo.lastUpdate, NULL);

    GPSInfo newGpsInfo = gpsInfo;
    gpsHistory.enqueue(newGpsInfo);

    if(gpsHistory.size() > 5){
        gpsHistory.dequeue();
    }
    //qInfo() << gpsHistory.size();

    visualizer->updateGPSPositionPoint(position);
}

void DataHandler::messagePlay(int index){
    MessageReceived(allMessages.at(index)); 
}
