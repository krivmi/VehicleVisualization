#include "mapem.h"

// TODO - search for the "unknown types"
QVector<QString> Mapem::laneTypes = {"Vehicle", "CrossWalk", "BikeLane", "Unknown", "Unknown", "Unknown", "TrackedVehicle", "Parking"};

Mapem::Mapem(qreal longitude, qreal latitude, qreal altitude, int messageID, long stationID, int stationType,
      float laneWidth, QString crossroadName, long crossroadID, QVector<Lane> lanes) :
      Message(longitude, latitude, altitude, messageID, stationID, stationType), crossroadID(crossroadID),
      crossroadName(crossroadName), laneWidth(laneWidth), lanes(lanes) { };

QVector <Lane> Mapem::getAllIngressVehicleLanes()
{
    QVector <Lane> ingressVehicleLanes;

    for(int i = 0; i < this->lanes.size(); i++){
        // if the lane is INGRESS LANE and it is for VEHICLES

        if(this->lanes[i].directionIngressPath && this->lanes[i].type == 0){
            ingressVehicleLanes.append(this->lanes[i]);
        }
    }
    return ingressVehicleLanes;
}

PointWorldCoord Mapem::getFirstPointOfLane(Lane lane, PointWorldCoord refPoint)
{
    qreal f_long = refPoint.longitude() + (qreal)lane.nodes[0].x / 10000000.0;
    qreal f_lat = refPoint.latitude() + (qreal)lane.nodes[0].y / 10000000.0;
    return PointWorldCoord(f_long, f_lat);
}
PointWorldCoord Mapem::getLastPointOfLane(Lane lane, PointWorldCoord refPoint)
{
    qreal f_long = refPoint.longitude() + (float)lane.nodes[1].x / 10000000.0;
    qreal f_lat = refPoint.latitude() + (float)lane.nodes[1].y / 10000000.0;
    return PointWorldCoord(f_long, f_lat);
}
Lane Mapem::getLaneByID(long id, QVector<Lane> lanes)
{
    for (int i = 0; i < lanes.size(); i++){
        if (lanes[i].laneID == id){
            return lanes[i];
        }
    }
    Lane empty;
    empty.laneID = -1;
    return empty;
};

long Mapem::getConnectedLaneID(ConnectingLane lane, QVector <Lane> lanes)
{
    // find lane to connect to
    long foundLaneId = -1;
    for (int l = 0; l < lanes.size(); l++){
        if (lanes[l].laneID == lane.laneNumber){
            //qInfo() << "Connection found";
            foundLaneId = lanes[l].laneID;
            break;
        }
    }
    return foundLaneId;
}

std::vector <PointWorldCoord> Mapem::getLanePoints(Lane lane, PointWorldCoord refPoint)
{
    std::vector<PointWorldCoord> raw_points;

    for (int j = 0; j < lane.nodes.size(); j++)
    {
        qreal f_long = refPoint.longitude() + (qreal)lane.nodes[j].x / 10000000.0;
        qreal f_lat = refPoint.latitude() + (qreal)lane.nodes[j].y / 10000000.0;
        PointWorldCoord node_coor = PointWorldCoord(f_long, f_lat);

        raw_points.emplace_back(node_coor);
    }
    return raw_points;
}

int Mapem::findAdjIngVehLaneByOrientation(float orientation)
{
    int index = -1;
    float minimalDifference = 361.0f;

    for(int i = 0; i < adjacentIngressLanes.size(); i++)
    {
        // take the first lane
        Lane lane = adjacentIngressLanes.at(i).at(0);
        PointWorldCoord ptFrom = getFirstPointOfLane(lane, refPoint);
        PointWorldCoord ptTo = getLastPointOfLane(lane, refPoint);

        const PointWorldPx ptFromPx(projection::get().toPointWorldPx(ptFrom, 17)); // 2
        const PointWorldPx ptToPx(projection::get().toPointWorldPx(ptTo, 17)); // 3

        QVector <qreal> toNorth = {0.0, 1.0};
        QVector <qreal> toEndOfLane = {ptToPx.x() - ptFromPx.x(), ptToPx.y() - ptFromPx.y()}; // x2

        qreal dot = toNorth[0] * toEndOfLane[0] + toNorth[1] * toEndOfLane[1];
        qreal det = toNorth[0] * toEndOfLane[1] - toNorth[1] * toEndOfLane[0];
        qreal angle = atan2(det, dot);
        qreal deg = angle * 180.0 / M_PI;

        if(deg < 0){ deg = 360 - deg; }

        float diff = abs(orientation - deg);
        //qInfo() << "Lane or: " << deg << "orientation: " << orientation << "diff: " << diff;

        if(diff < minimalDifference){
            minimalDifference = diff;
            index = i;
        }
    }
    //qInfo() << "Winner Minimal diff: " << minimalDifference;
    return index;
}

QVector <QVector<Lane>> Mapem::findAdjacentIngressVehicleLanes()
{
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

QVector <QPair<int, QVector<SignalGroupInfo>>> Mapem::getLaneSignalGroupsVector(Lane lane)
{
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
            //qInfo() << &currentPair.first << ", " << &signalGroupPairVector[j].first;

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
}
bool Mapem::haveAdjacentLanesSameSignalGroup(QVector<Lane> adjacentIngressLanes)
{
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
void Mapem::prepareCrossroadSignalGroups()
{
    adjacentIngressLanes = this->findAdjacentIngressVehicleLanes();

    for(int i = 0; i < adjacentIngressLanes.size(); i++){
        for(int j = 0; j < adjacentIngressLanes[i].size(); j++){
            //e.g. [SG3,[{l5,MR}], SG4[{l5,MS}, {l4,MR}]]
            adjacentIngressLanes[i][j].signalGroupInfoPairVector = getLaneSignalGroupsVector(adjacentIngressLanes[i][j]);
        }
    }
}
