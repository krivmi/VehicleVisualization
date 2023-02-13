#include "mapem.h"

QVector<QString> Mapem::laneTypes = {"Vehicle", "CrossWalk", "BikeLane", "Unknown", "Unknown", "Unknown", "TrackedVehicle"};

QVector <Lane> Mapem::getAllIngressVehicleLanes(){
    QVector <Lane> ingressVehicleLanes;

    for(int i = 0; i < this->lanes.size(); i++){
        // if the lane is INGRESS LANE and it is for VEHICLES
        if(this->lanes[i].directionIngressPath && this->lanes[i].type == 0){
            ingressVehicleLanes.append(this->lanes[i]);
        }
    }
    return ingressVehicleLanes;
}

QVector <QVector<Lane>> Mapem::findAdjacentIngressVehicleLanes(){
    QVector <Lane> ingressVehicleLanes = getAllIngressVehicleLanes();
    QVector <Lane> adjacentLanes;
    QVector <QVector<Lane>> adjacentLanesList;

    for(int i = 0; i < ingressVehicleLanes.size(); i++){
        adjacentLanes.append(ingressVehicleLanes[i]);

        int ingressApproach = ingressVehicleLanes[i].ingressApproach;

        for(int j = 0; j < ingressVehicleLanes.size(); j++){
            if(ingressVehicleLanes[i].laneID == ingressVehicleLanes[j].laneID){
                continue;
            }
            if(ingressApproach == ingressVehicleLanes[j].ingressApproach){
                adjacentLanes.append(ingressVehicleLanes[j]);
            }
        }

        // check if the lanes are already in the list -> I cannot have [[1], [5,6], [6,5]], I only need [[1], [5,6]]
        bool laneAlreadyInTheList = false;
        for(int j = 0; j < adjacentLanesList.size(); j++){
            for(int k = 0; k < adjacentLanesList[j].size(); k++){
                for(int l = 0; l < adjacentLanes.size(); l++){
                    if(adjacentLanesList[j][k].laneID == adjacentLanes[l].laneID){
                        laneAlreadyInTheList = true;
                        break;
                    }
                }
                if(laneAlreadyInTheList){
                    break;
                }
            }
            if(laneAlreadyInTheList){
                break;
            }
        }

        if(laneAlreadyInTheList){
            adjacentLanes.clear();
        } else{
            adjacentLanesList.append(adjacentLanes);
            adjacentLanes.clear();
        }
    }
    return adjacentLanesList;
}

QVector <QPair<int, QVector<SignalGroupInfo>>> Mapem::getLaneSignalGroupsVector(Lane lane){

    QVector <QPair<int, QVector<SignalGroupInfo>>> signalGroupPairVector;

    for (int i = 0; i < lane.connectingLanes.size(); i++){
        QPair<int, QVector<SignalGroupInfo>> signalGroupPair;
        signalGroupPair.first = lane.connectingLanes[i].signalGroup;

        SignalGroupInfo signalGroupInfo;
        signalGroupInfo.laneID = lane.laneID;
        signalGroupInfo.connectingLaneID = lane.connectingLanes[i].laneNumber;

        if(lane.connectingLanes[i].maneuverStraightAllowed){
            signalGroupInfo.manouver = "straight";
            signalGroupPair.second.append(signalGroupInfo);
        }
        if(lane.connectingLanes[i].maneuverLeftAllowed){
            signalGroupInfo.manouver = "left";
            signalGroupPair.second.append(signalGroupInfo);
        }
        if(lane.connectingLanes[i].maneuverRightAllowed){
            signalGroupInfo.manouver = "right";
            signalGroupPair.second.append(signalGroupInfo);
        }
        signalGroupPairVector.append(signalGroupPair);
    }

    QVector <QPair<int, QVector<SignalGroupInfo>>> signalGroupPairVectorChanged;

    // make proper groups
    QVector <int> iteratedGroups;
    for (int i = 0; i < signalGroupPairVector.size(); i++){
        QPair<int, QVector<SignalGroupInfo>> currentPair = signalGroupPairVector[i];

        if(iteratedGroups.contains(signalGroupPairVector[i].first)){
            continue;
        }

        for (int j = 0; j < signalGroupPairVector.size(); j++){
            qInfo() << &currentPair.first << ", " << &signalGroupPairVector[j].first;

            if(&signalGroupPairVector[i].first == &signalGroupPairVector[j].first /*|| iteratedGroups.contains(signalGroupPairVector[j].first)*/){
                continue;
            }
            if(currentPair.first == signalGroupPairVector[j].first){
                iteratedGroups.append(signalGroupPairVector[j].first);

                for(int k = 0; k < signalGroupPairVector[j].second.size(); k++){
                    currentPair.second.append(signalGroupPairVector[j].second[k]);
                }
            }
        }
        signalGroupPairVectorChanged.append(currentPair);

    }

    return signalGroupPairVectorChanged;
/*
    QVector <SignalGroupInfo> signalGroupInfoArray;
    QVector <QVector<SignalGroupInfo>> signalGroupInfoArrayList;


    for (int i = 0; i < lane.connectingLanes.size(); i++){
        SignalGroupInfo signalGroupInfo;
        signalGroupInfo.laneID = lane.laneID;
        signalGroupInfo.connectingLaneID = lane.connectingLanes[i].laneNumber;
        signalGroupInfo.signalGroup = lane.connectingLanes[i].signalGroup;

        if(lane.connectingLanes[i].maneuverStraightAllowed){
            signalGroupInfo.manouver = "straight";
            signalGroupInfoArray.append(signalGroupInfo);
        }
        if(lane.connectingLanes[i].maneuverLeftAllowed){
            signalGroupInfo.manouver = "left";
            signalGroupInfoArray.append(signalGroupInfo);
        }
        if(lane.connectingLanes[i].maneuverRightAllowed){
            signalGroupInfo.manouver = "right";
            signalGroupInfoArray.append(signalGroupInfo);
        }

    }
    if(signalGroupInfoArray.isEmpty()){
        return {};
    }
    // sort an array to finally create
    std::sort(signalGroupInfoArray.begin(), signalGroupInfoArray.end());
    // TODO - find out if it is well sorted

    int firstGroupID = signalGroupInfoArray[0].signalGroup;
    QVector <SignalGroupInfo> currentGroup = {signalGroupInfoArray[0]};

    for(int i = 1; i < signalGroupInfoArray.size(); i++){
        if(firstGroupID == signalGroupInfoArray[i].signalGroup){
            currentGroup.append(signalGroupInfoArray[i]);
        } else{
            signalGroupInfoArrayList.append(currentGroup);
            currentGroup.empty();
            firstGroupID = signalGroupInfoArray[i].signalGroup;
            currentGroup.append(signalGroupInfoArray[i]);
        }
    }
    signalGroupInfoArrayList.append(currentGroup);

    return signalGroupInfoArrayList;*/
}
bool Mapem::haveAdjacentLanesSameSignalGroup(QVector<Lane> adjacentIngressLanes){
    QVector <int> signalGroupVector;

    // for every lane in adjacent lanes
    for(int i = 0; i < adjacentIngressLanes.size(); i++){
        Lane lane = adjacentIngressLanes[i];

        // for every signal group that is connected to the lane
        for(int k = 0; k < lane.signalGroupInfoPairVector.size(); k++){
            // if the vector does not contain the group, append it
            if(!signalGroupVector.contains(lane.signalGroupInfoPairVector[k].first)){
                signalGroupVector.append(lane.signalGroupInfoPairVector[k].first);
            }
        }
    }
    if(signalGroupVector.size() == 1){
        // all lanes describes the same signal group (they have one traffic light)

        return true;
    }
    return false;
}
void Mapem::prepareCrossroadSignalGroups(){

    adjacentIngressLanes = this->findAdjacentIngressVehicleLanes();

    for(int i = 0; i < adjacentIngressLanes.size(); i++){
        for(int j = 0; j < adjacentIngressLanes[i].size(); j++){
            //e.g. [SG3,[{l5,MR}], SG4[{l5,MS}, {l4,MR}]]
            adjacentIngressLanes[i][j].signalGroupInfoPairVector = getLaneSignalGroupsVector(adjacentIngressLanes[i][j]);
        }
    }

}
