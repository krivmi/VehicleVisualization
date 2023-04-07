#include "visualizer.h"
//#include "dialogs.h"
#include <math.h>
#include <qrandom.h>

Visualizer::Visualizer(QMapControl * mapWidget)
{
    this->m_map_control = mapWidget;

    // Vytvoření vlastní vrstvy bodů pro zobrazení vozidel
    custom_layer = std::make_shared<LayerGeometry>("Custom Layer");
    navigation_layer = std::make_shared<LayerGeometry>("Navigation Layer");
    m_map_control->addLayer(custom_layer);
    m_map_control->addLayer(navigation_layer);

    // Connect click events of the layer to this object.
    QObject::connect(custom_layer.get(), &LayerGeometry::geometryClicked, this, &Visualizer::geometryClickEvent);

    pointPen = QPen(QColor(255,0,0));
    pointPen.setWidth(3);

    linePen = QPen(QColor(0,255,0));
    linePen.setWidth(3);
}
Lane Visualizer::getLaneByID(long id, QVector<Lane> lanes){

    for (int i = 0; i < lanes.size(); i++){
        if (lanes[i].laneID == id){
            return lanes[i];
        }
    }
    Lane empty;
    empty.laneID = -1;
    return empty;
};

void Visualizer::drawLane(Lane & lane, std::vector <PointWorldCoord> points, QColor color, bool drawPointsOfLane){
    linePen.setColor(color);
    lane.laneMapPtr = std::make_shared<GeometryLineString>(points);
    lane.laneMapPtr->setPen(linePen);
    custom_layer->addGeometry(lane.laneMapPtr);

    if(drawPointsOfLane){
        for(int i = 0; i < points.size(); i++){
            std::shared_ptr<GeometryPoint> nodePoint = std::make_shared<GeometryPointCircle>(points[i], QSize(10.0, 10.0));
            pointPen.setColor(color);
            nodePoint->setMetadata("id", QVariant::fromValue(lane.laneID));
            nodePoint->setMetadata("name", "lane");
            nodePoint->setPen(pointPen);
            custom_layer->addGeometry(nodePoint);
            lane.lanePointsVectorPtr.append(nodePoint);
        }
    }
}
void Visualizer::removeAllGeometries(bool deletePointGPS){
    custom_layer->clearGeometries();

    if(deletePointGPS){
        navigation_layer->clearGeometries();
    }
    // I have to redraw canvas, do not know why
    m_map_control->requestRedraw();
}
void Visualizer::removeGeometry(const std::shared_ptr<Geometry> & geometry){
    custom_layer->removeGeometry(geometry);
}
void Visualizer::addTrafficLight(Mapem * crossroad, std::shared_ptr<GeometryPointImageScaled> & geometryPoint, int adjacentIngressLanesIndex, bool sameTrafficLight){

    // image is set to the first lane if the traffic lights have the same signal group
    Lane lane = crossroad->adjacentIngressLanes[adjacentIngressLanesIndex][0];

    PointWorldCoord ptFrom = Mapem::getFirstPointOfLane(lane, crossroad->refPoint);
    PointWorldCoord ptTo = Mapem::getLastPointOfLane(lane, crossroad->refPoint);

    //visualizer->drawPoint(ptFrom, QColor(230, 180, 230), QSize(15.0, 15.0));
    //visualizer->drawPoint(ptTo, QColor(50, 180, 230), QSize(15.0, 15.0));
    std::vector<PointWorldCoord> points = {ptFrom, ptTo};
    //visualizer->drawLane(points, QColor(50, 180, 230));

    const PointWorldPx ptFromPx(projection::get().toPointWorldPx(ptFrom, 17)); // 2
    const PointWorldPx ptToPx(projection::get().toPointWorldPx(ptTo, 17)); // 3

    QVector <qreal> toNorth = {0.0, 1.0};
    QVector <qreal> toEndOfLane = {ptToPx.x() - ptFromPx.x(), ptToPx.y() - ptFromPx.y()}; // x2

    qreal dot = toNorth[0] * toEndOfLane[0] + toNorth[1] * toEndOfLane[1];
    qreal det = toNorth[0] * toEndOfLane[1] - toNorth[1] * toEndOfLane[0];
    qreal angle = atan2(det, dot);
    qreal deg = angle * 180.0 / M_PI;

    QString infoText;
    for(int j = 0; j < crossroad->adjacentIngressLanes[adjacentIngressLanesIndex].size(); j++){
        lane = crossroad->adjacentIngressLanes[adjacentIngressLanesIndex][j];
        for(int k = 0; k < lane.signalGroupInfoPairVector.size(); k++){
            int signalGroup = lane.signalGroupInfoPairVector[k].first;
            infoText += "Lane: " + QString::number(lane.laneID) + "<br>";
            infoText += "Signal group: " + QString::number(signalGroup) + "<br>";
            infoText += "Connecting Lanes: ";

            for(int l = 0; l < lane.signalGroupInfoPairVector[k].second.size(); l++){
                infoText += " Connecting Lane ID: " + QString::number(lane.signalGroupInfoPairVector[k].second[l].connectingLaneID) + "<br>";
                infoText += " Maneuver: " + lane.signalGroupInfoPairVector[k].second[l].manouver + "<br>";
            }
            Lane l1 = lane;

        }
        infoText += "<br>";
    }

    geometryPoint = std::make_shared<GeometryPointImageScaled>(ptTo, ":/resources/images/trafficLights.png", 19);
    geometryPoint->setRotation(deg, true);
    geometryPoint->setMetadata("text", infoText);
    geometryPoint->setMetadata("name", "trafficLight");
    geometryPoint->setMetadata("crossroadID", QVariant::fromValue(crossroad->crossroadID));
    geometryPoint->setMetadata("adjacentIngressLanesIndex", QVariant::fromValue(adjacentIngressLanesIndex));
    geometryPoint->setMetadata("sameTrafficLight", QVariant::fromValue(sameTrafficLight));
    geometryPoint->setZIndex(2);
    custom_layer->addGeometry(geometryPoint);
}

QColor Visualizer::getColorByLaneType(QString type){
    QColor color;

    if(type == "CrossWalk"){ color = QColor("blue");}
    else if(type == "Vehicle"){ color = QColor("orange");}
    else{color = QColor("yellow");}

    return color;
}
QColor Visualizer::getColorByApproach(bool isIngress){
    QColor color;

    if(isIngress){ color = QColor("blue");}
    else{color = QColor("orange");}

    return color;
}
void Visualizer::drawPoint(PointWorldCoord point, QColor color, QSize size){
    std::shared_ptr<GeometryPoint> node_point = std::make_shared<GeometryPointCircle>(point, size);
    pointPen.setColor(color);
    node_point->setPen(pointPen);
    custom_layer->addGeometry(node_point);
}
/*
void Visualizer::drawPoints(std::vector <PointWorldCoord> points, QColor color){
    for(int i = 0; i < points.size(); i++){
        drawPoint(points[i], color);
    }
}*/
std::vector <PointWorldCoord> Visualizer::getLanePoints(Lane lane, PointWorldCoord refPoint){
    std::vector<PointWorldCoord> raw_points;
    for (int j = 0; j < lane.nodes.size(); j++){
        qreal f_long = refPoint.longitude() + (qreal)lane.nodes[j].x / 10000000.0;
        qreal f_lat = refPoint.latitude() + (qreal)lane.nodes[j].y / 10000000.0;
        PointWorldCoord node_coor = PointWorldCoord(f_long, f_lat);

        raw_points.emplace_back(node_coor);
    }
    return raw_points;
}

void Visualizer::drawLanes(QVector<Lane>& lanes, PointWorldCoord refPoint, int laneType, bool drawPointsOfLane){
    for (int i = 0; i < lanes.size(); i++){
        if(lanes[i].type != laneType){
            continue;
        }
        // get the points of current lane from the crossroad
        std::vector <PointWorldCoord> lanePoints = getLanePoints(lanes[i], refPoint);
        // draw the lane from the points
        //drawLane(lanes[i], lanePoints, getColorByLaneType(lanes[i].strType), drawPointsOfLane);
        drawLane(lanes[i], lanePoints, getColorByApproach(lanes[i].directionIngressPath), drawPointsOfLane);
    }
}
long Visualizer::getConnectedLaneID(ConnectingLane lane, QVector <Lane> lanes){
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

void Visualizer::drawLanesConnections(QVector<Lane> lanes, PointWorldCoord refPoint, QColor color){
    for (int i = 0; i < lanes.size(); i++){
        // get the first point of the crossroad lane (thus the starting point)
        PointWorldCoord ptFrom = Mapem::getFirstPointOfLane(lanes[i], refPoint);

        // find all the connection lines connected to the line[i]
        for (int j = 0; j < lanes[i].connectingLanes.size(); j++){
            // get the line that is connected to the connecting lane (there must be only one)
            long connectedLaneId = getConnectedLaneID(lanes[i].connectingLanes[j], lanes);

            if(connectedLaneId >= 0){
                Lane connectionLane = getLaneByID(connectedLaneId, lanes);
                PointWorldCoord ptTo = Mapem::getFirstPointOfLane(connectionLane, refPoint);

                std::vector<PointWorldCoord> raw_points;
                raw_points.emplace_back(ptFrom);
                raw_points.emplace_back(ptTo);

                drawLane(connectionLane, raw_points, color);

            } else{
                qInfo() << "Something is wrong with the lanes connections";
                throw std::invalid_argument("Something is wrong with the lanes connections");
            }
        }
    }
}
void Visualizer::drawVehiclePath(Cam * cam){

    std::vector<PointWorldCoord> points;
    for(int i = 0; i < cam->path.size(); i++){
        PointWorldCoord pt = PointWorldCoord(cam->refPoint.longitude() + cam->path[i].dLongitude, cam->refPoint.latitude() + cam->path[i].dLatitude);

        cam->path[i].point = std::make_shared<GeometryPointCircle>(pt, QSizeF(10.0, 10.0));
        cam->path[i].point->setPen(pointPen);
        custom_layer->addGeometry(cam->path[i].point);

        points.push_back(pt);
    }
    cam->pathString = std::make_shared<GeometryLineString>(points);
    cam->pathString->setPen(linePen);

    custom_layer->addGeometry(cam->pathString);
}
void Visualizer::removeVehiclePath(Cam * cam){
    for(int i = 0; i < cam->path.size(); i++){
        custom_layer->removeGeometry(cam->path[i].point);
    }
    custom_layer->removeGeometry(cam->pathString);
}
void Visualizer::addCamStationToLayer(Cam * cam){
    cam->geometryPoint = std::make_shared<GeometryPointImageScaled>(cam->refPoint, (cam->isSrcAttention) ? cam->imgSrcAttention : cam->imgSrcDefault, 19);
    cam->geometryPoint->setZIndex(2);
    cam->geometryPoint->setMetadata("id", QVariant::fromValue(cam->stationID));
    cam->geometryPoint->setMetadata("name", "cam");
    custom_layer->addGeometry(cam->geometryPoint);

    //drawBoundingBox(cam->geometryPoint->boundingBox(19), cam);
}
void Visualizer::addDenmHazardToLayer(Denm * denm){
    denm->geometryPoint = std::make_shared<GeometryPointImageScaled>(denm->refPoint, ":/resources/images/DENM_icon.png", 19);
    denm->geometryPoint->setMetadata("originatingStationID", QVariant::fromValue(denm->originatingStationID));
    denm->geometryPoint->setMetadata("sequenceNumber", QVariant::fromValue(denm->sequenceNumber));
    denm->geometryPoint->setMetadata("name", "denm");
    custom_layer->addGeometry(denm->geometryPoint);
}
void Visualizer::geometryClickEvent(const Geometry* geometry)
{
    if(geometry->geometryType() == Geometry::GeometryType::GeometryPoint)
    {
        if(geometry->metadata("name").toString() == "cam")
        {
            long stationID = geometry->metadata("id").toLongLong();

            if(stationID > 0){
                emit unitClick(stationID);
            }
        }
        else if(geometry->metadata("name").toString() == "denm")
        {
            long originatingStationID = geometry->metadata("originatingStationID").toLongLong();
            int sequenceNumber = geometry->metadata("sequenceNumber").toInt();

            emit hazardClick(originatingStationID, sequenceNumber);
        }
        else if(geometry->metadata("name").toString() == "lane")
        {
            long laneID = geometry->metadata("id").toLongLong();

            /*InfoDialog* info_dialog = new InfoDialog(this->m_map_control);
            info_dialog->setWindowTitle("Lane");
            QString infoText = "ID: " + QString::number(laneID);
            info_dialog->setInfotext(infoText);
            //info_dialog->show();*/
        }
        else if(geometry->metadata("name").toString() == "trafficLight")
        {
            QString text = geometry->metadata("text").toString();
            int crossroadID = geometry->metadata("crossroadID").toString().toInt();
            int adjacentIngressLanesIndex = geometry->metadata("adjacentIngressLanesIndex").toString().toInt();
            QString sameTrafficLightStr = geometry->metadata("sameTrafficLight").toString();
            bool sameTrafficLight = (sameTrafficLightStr == "true") ? true : false;

            /*InfoDialog* info_dialog = new InfoDialog(this->m_map_control);
            info_dialog->setWindowTitle("Traffic Light");
            info_dialog->setInfotext(text);
            //info_dialog->show();*/

            emit trafficLightClick(crossroadID, adjacentIngressLanesIndex, sameTrafficLight);
        }
    }
}

void Visualizer::drawBoundingBox(RectWorldCoord rect, Cam * cam)
{
    const std::vector<PointWorldCoord> points = {rect.topLeftCoord(), rect.topRightCoord(), rect.bottomRightCoord(), rect.bottomLeftCoord()};

    //cam->polygon = std::make_shared<GeometryPolygon>(points);
    //custom_layer->addGeometry(cam->polygon);
}

void Visualizer::updateGPSPositionPoint(PointWorldCoord position){
    if(GPSpositionPoint != nullptr){
        // update position
        GPSpositionPoint->setCoord(position);
    } else{
        // draw point
        drawGPSPositionPoint(position);
    }
}
void Visualizer::drawGPSPositionPoint(PointWorldCoord position){
    GPSpositionPoint = std::make_shared<GeometryPointImageScaled>(position, ":/resources/images/navigationBlue.png", 19);
    GPSpositionPoint->setZIndex(3);
    navigation_layer->addGeometry(GPSpositionPoint);
}
void Visualizer::removeGPSPositionPoint(){
    navigation_layer->removeGeometry(GPSpositionPoint);
    GPSpositionPoint = nullptr;
}
