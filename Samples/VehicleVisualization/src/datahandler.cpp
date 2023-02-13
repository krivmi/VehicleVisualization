#include "datahandler.h"
#include "trafficlightwidget.h"
#include "QMapControl/Projection.h"
#include <math.h>
#include <algorithm>
#include <memory>

# define M_PI 3.14159265358979323846  /* pi */

DataHandler::DataHandler(Visualizer * visualizer, QWidget* parent) : QObject(parent)
{
    this->visualizer = visualizer;
    this->gpsInfo.longitude = -1;
    this->gpsInfo.latitude = -1;

    QTimer::singleShot(1000, this, SLOT(handleCrossroadProximity()));
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

    this->addMessage(message);
    emit MessageToLog(message);
}
void DataHandler::clearData(){
    deleteCamUnits();
    deleteMapemUnits();
    deleteSpatemMessages();
    deleteDenmMessages();
    deleteAllMessages();
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
void DataHandler::handleCrossroadProximity(){
    if(autoModeOn){
        Mapem * nearerCrossroad = getClosestCrossroad();

        if(nearerCrossroad != nullptr){
            emit openTLW();
        } else{
            emit closeTLW();
        }
    }

    QTimer::singleShot(1000, this, SLOT(handleCrossroadProximity()));
}
double measure(qreal lat1, qreal lon1, qreal lat2, qreal lon2){  // generally used geo measurement function
    double R = 6378.137; // Radius of earth in KM
    double dLat = lat2 * M_PI / 180 - lat1 * M_PI / 180;
    double dLon = lon2 * M_PI / 180 - lon1 * M_PI / 180;
    double a = sin(dLat/2) * sin(dLat/2) +
    cos(lat1 * M_PI / 180) * cos(lat2 * M_PI / 180) *
    sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = R * c;
    return d * 1000; // meters
}
Mapem * DataHandler::getClosestCrossroad(){
    QMutex mutex;

    Mapem * crossroad = nullptr;
    double minDistance = 10000000000;
    // ensure integrity if mapem is to be deleted
    mutex.lock();
    for(int i = 0; i < mapemUnits.size(); i++){
        PointWorldCoord gpsPosn = PointWorldCoord(gpsInfo.longitude, gpsInfo.latitude);
        PointWorldCoord crossPos = mapemUnits.at(i)->refPoint;

        double distance = measure(gpsPosn.latitude(), gpsPosn.longitude(), crossPos.latitude(), crossPos.longitude());

        if(distance < minDistance){
            crossroad = mapemUnits.at(i).get();
            minDistance = distance;
        }
    }
    mutex.unlock();


    if(minDistance < 60.0){
        qInfo() << crossroad;
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

void DataHandler::SPATEMReceived(std::shared_ptr<Spatem> newSPATEM){

    Spatem * oldSpatem = findSpatemByTime(newSPATEM->moy, newSPATEM->timeStamp);
    // check if the same SPATEM has been already received

    if(oldSpatem != nullptr) {
        qInfo() << "the same SPATEM has been already received";
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
void DataHandler::SREMReceived(std::shared_ptr<Srem> newSrem){
    qInfo() << "SREM received";
}
void DataHandler::DENMReceived(std::shared_ptr<Denm> newDENM){

    Denm * oldDenm = getDenmByTimeAndCode(newDENM->detectionTime, newDENM->causeCode);

    if(oldDenm == nullptr){
        this->addDenmMessage(newDENM);

        visualizer->addDenmHazardToLayer(newDENM.get());

    }
    else {
        //qInfo() << "This event message has been already received...";
    }

}

void DataHandler::addTrafficLightsFromSignalGroups(Mapem * crossroad){

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

void DataHandler::createTrafficLightWidgets(int crossroadID, int adjacentLaneIndex, bool sameTrafficLight){
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

Message * DataHandler::getMessageByID(long id){
    for(int i = 0; i < allMessages.size(); i++){
        if(allMessages.at(i)->stationID == id){
            return allMessages.at(i).get();
        }
    }
    return nullptr;
}

Cam * DataHandler::getCamUnitByID(long id){
    for(int i = 0; i < camUnits.size(); i++){
        if(camUnits.at(i)->stationID == id){
            return camUnits.at(i).get();
        }
    }
    return nullptr;
}
Mapem * DataHandler::getMapemUnitByID(long id){
    for(int i = 0; i < mapemUnits.size(); i++){
        if(mapemUnits.at(i)->stationID == id){
            return mapemUnits.at(i).get();
        }
    }
    return nullptr;
}

Denm * DataHandler::getDenmByTimeAndCode(long detectionTime, int causeCode){
    for(int i = 0; i < denmMessages.size(); i++){
        if(denmMessages.at(i)->detectionTime == detectionTime &&
                denmMessages.at(i)->causeCode == causeCode){
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
void DataHandler::deleteMapemUnitByID(long id){
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
void DataHandler::deleteDenmUnitByTimeAndCode(long detectionTime, int causeCode){
    for(int i = 0; i < denmMessages.size(); i++){
        if(denmMessages.at(i)->detectionTime == detectionTime && denmMessages.at(i)->causeCode == causeCode){
            visualizer->removeGeometry(denmMessages.at(i)->geometryPoint);
            denmMessages.removeAt(i);
            break;
        }
    }
}
void DataHandler::deleteCamUnitByID(long id){
    for(int i = 0; i < camUnits.size(); i++){
        if(camUnits.at(i)->stationID == id){
            visualizer->removeGeometry(camUnits.at(i)->geometryPoint);
            camUnits.removeAt(i);
            break;
        }
    }
}

Mapem * DataHandler::getMapemByCrossroadID(long id){
    for(int i = 0; i < mapemUnits.size(); i++){
        if(mapemUnits.at(i)->crossroadID == id){
            return mapemUnits.at(i).get();
        }
    }
    return nullptr;
}
Spatem * DataHandler::findSpatemByTime(int moy, int timeStamp){
    for(int i = 0; i < spatemMessages.size(); i++){
        if(spatemMessages.at(i)->moy == moy && spatemMessages.at(i)->timeStamp == timeStamp){
            return spatemMessages.at(i).get();
        }
    }
    return nullptr;
}

void DataHandler::GPSPositionReceived(PointWorldCoord position){
    if(position.longitude() == gpsInfo.longitude && position.latitude() == gpsInfo.latitude){
        //visualizer->darkenGPSPositionPoint();
    } else{
        gpsInfo.longitude = position.longitude();
        gpsInfo.latitude = position.latitude();
        gettimeofday(&gpsInfo.lastUpdate, NULL);

        visualizer->updateGPSPositionPoint(position);
    }
}

void DataHandler::messagePlay(int index){
    MessageReceived(allMessages.at(index)); 
}

/*
template <typename T>
int get_index(long id, QVector <T*> messages){
    int stationIndex = -1;

    for(int i = 0; i < messages.size(); i++){
        if(messages.at(i)->stationID == id){
            stationIndex = i;
            break;
        }
    }
    // if the station was not found, return -1
    return stationIndex;
}
*/
/*
    qInfo() << message;

    QString protocol = message->GetProtocol();

    if(protocol == "Cam"){
        Cam * newCam = dynamic_cast<Cam*>(message);
        qInfo() << newCam;
        this->CAMReceived(newCam);
    } else if(protocol == "Mapem"){
        Mapem * newMapem = dynamic_cast<Mapem*>(message);
        this->MAPEMReceived(newMapem);
    } else if(protocol == "Spatem"){
        Spatem * newSpatem = dynamic_cast<Spatem*>(message);
        this->SPATEMReceived(newSpatem);
    }
    else if(protocol == "Srem"){
        Srem * newSrem = dynamic_cast<Srem*>(message);
        this->SREMReceived(newSrem);
    }
    else{
        qInfo() << "Something went wrong";
    }*/

/*
QString protocol = allMessages.at(index)->GetProtocol();

if(protocol == "Cam"){
    std::shared_ptr<Cam> newCam = std::make_shared<Cam>(*std::static_pointer_cast<Cam>(allMessages.at(index)));
    this->CAMReceived(newCam);
} else if(protocol == "Mapem"){
    std::shared_ptr<Mapem> newMapem = std::make_shared<Mapem>(*std::static_pointer_cast<Mapem>(allMessages.at(index)));
    this->MAPEMReceived(newMapem);
} else if(protocol == "Spatem"){
    std::shared_ptr<Spatem> newSpatem = std::make_shared<Spatem>(*std::static_pointer_cast<Spatem>(allMessages.at(index)));
    this->SPATEMReceived(newSpatem);
}
else if(protocol == "Srem"){
    //Srem * newSrem = dynamic_cast<Srem*>(allMessages.at(index));
    //this->SREMReceived(newSrem);
}
else{
    qInfo() << "Something went wrong";
}*/
/*
        QString text;
        for(int i = 0; i < crossroad->lanes.size(); i++){
            std::vector <PointWorldCoord> pts = visualizer->getLanePoints(crossroad->lanes[i], crossroad->refPoint);

            for(int j = 0; j < pts.size(); j++){
                text += QString::number(pts[j].longitude(), 'd') + "," + QString::number(pts[j].latitude(), 'd') + "\n";
            }
        }
        qInfo() << text;
*/

/*
    if(crossroad != nullptr){
        // VISUALIZE POINTS

        // run through all the lanes of the crossroad
        for (int i = 0; i < crossroad->lanes.size(); i++){
            if(crossroad->lanes[i].type == 0){
                //                                                        ↓
                // get first point of the lane (one closer to refPoint) --| RP
                PointWorldCoord ptFrom = visualizer->getFirstPointOfLane(crossroad->lanes[i], crossroad->refPoint);

                std::vector<PointWorldCoord> points = visualizer->getLanePoints(crossroad->lanes[i], crossroad->refPoint);
                visualizer->drawLane(points, QColor(50, 180, 230));

                // run through all connectTo lanes that the current lane is connected to
                for (int j = 0; j < crossroad->lanes[i].connectingLanes.size(); j++){
                    // get ID of the current connected lane
                    long connectedLaneId = visualizer->getConnectedLaneID(crossroad->lanes[i].connectingLanes[j], crossroad->lanes);

                    if(connectedLaneId >= 0){
                        Lane connectionLane = visualizer->getLane(connectedLaneId, crossroad->lanes);

                        int connectionSignalGroup = crossroad->lanes[i].connectingLanes[j].signalGroup;

                        QColor colorLane;
                        for(int k = 0; k < newSPATEM->movementStates.size(); k++){
                            if(newSPATEM->movementStates[k].signalGroup == connectionSignalGroup){
                                int signalGroupState = newSPATEM->movementStates[k].movementEvents[0].eventState;


                                if(signalGroupState == 5){ colorLane = QColor("green");}
                                else if(signalGroupState == 3){ colorLane = QColor("red");}
                                else if(signalGroupState == 4){ colorLane = QColor("purple");} // pre-movement == 4
                                else if(signalGroupState == 7){ colorLane = QColor("orange");}
                                else{colorLane = QColor(0, 100, 255);}
                                //qInfo() << "Found signal group";
                                break;
                            }
                        }
                        visualizer->drawPoint(ptFrom, colorLane, QSize(15.0, 15.0));
                    }
                    else{
                        qInfo() << "Something is wrong with the lanes connections";
                        throw std::invalid_argument("Something is wrong with the lanes connections");
                    }
                }
            }
        }
    }*/
    /*
    if(crossroad != nullptr){
        // VISUALIZE CONNECTIONS

        // run through all the lanes of the crossroad
        for (int i = 0; i < crossroad->lanes.size(); i++){
            if(crossroad->lanes[i].type != 0){
                continue;
            }

            //                                                        ↓
            // get first point of the lane (one closer to refPoint) --| RP
            PointWorldCoord ptFrom = visualizer->getFirstPointOfLane(crossroad->lanes[i], crossroad->refPoint);

            // run through all connectTo lanes that the current lane is connected to
            for (int j = 0; j < crossroad->lanes[i].connectingLanes.size(); j++){
                // get ID of the current connected lane
                long connectedLaneId = visualizer->getConnectedLaneID(crossroad->lanes[i].connectingLanes[j], crossroad->lanes);
                //                                                                                        ↓
                // if the lane is found, also get first point of the lane (one closer to refPoint) --| RP |--
                if(connectedLaneId >= 0){
                    Lane connectionLane = visualizer->getLane(connectedLaneId, crossroad->lanes);
                    PointWorldCoord ptTo = visualizer->getFirstPointOfLane(connectionLane, crossroad->refPoint);

                    int connectionSignalGroup = crossroad->lanes[i].connectingLanes[j].signalGroup;

                    QColor colorLane;
                    for(int k = 0; k < newSPATEM->movementStates.size(); k++){
                        if(newSPATEM->movementStates[k].signalGroup == connectionSignalGroup){
                            int signalGroupState = newSPATEM->movementStates[k].movementEvents[0].eventState;


                            if(signalGroupState == 5){ colorLane = QColor("green");}
                            else if(signalGroupState == 3){ colorLane = QColor("red");}
                            else if(signalGroupState == 4){ colorLane = QColor("purple");} // pre-movement == 4
                            else if(signalGroupState == 7){ colorLane = QColor("orange");}
                            else{colorLane = QColor(0, 100, 255);}
                            //qInfo() << "Found signal group";
                            break;
                        }
                    }

                    std::vector<PointWorldCoord> raw_points;
                    raw_points.emplace_back(ptFrom);
                    raw_points.emplace_back(ptTo);

                    visualizer->drawLane(crossroad->lanes[i], raw_points, colorLane);
                }
                else{
                    qInfo() << "Something is wrong with the lanes connections";
                    throw std::invalid_argument("Something is wrong with the lanes connections");
                }
            }
        }
    } else{
        qInfo() << "There is no MAP information for this SPATEM message";
    }//*/
