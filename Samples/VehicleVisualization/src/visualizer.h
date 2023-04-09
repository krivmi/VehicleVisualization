#pragma once

#include "message.h"
#include "cam.h"
#include "mapem.h"
#include "spatem.h"
#include "denm.h"

#include <QVector>

#include <QMapControl/GeometryLineString.h>
#include <QMapControl/GeometryPointCircle.h>
//#include <QMapControl/GeometryPolygon.h>
#include <QMapControl/GeometryWidget.h>
#include <QMapControl/Layer.h>
#include <QMapControl/LayerGeometry.h>


class Visualizer : public QObject
{
    Q_OBJECT

public:
    Visualizer(QMapControl * mapWidget);

    void addCamStationToLayer(Cam * cam);
    void addDenmHazardToLayer(Denm * denm);
    void addTrafficLight(Mapem * crossroad, std::shared_ptr<GeometryPointImageScaled> & geometryPoint, int adjacentIngressLanesIndex, bool sameTrafficLight);

    void drawLanes(QVector<Lane> & lanes, PointWorldCoord refPoint, int laneType, bool drawPointsOfLane = false);
    //void drawReferencePoint(PointWorldCoord point, QColor color = QColor(0, 0, 0));
    //void drawPoints(std::vector <PointWorldCoord> points, QColor color = QColor(0, 0, 0));
    void drawVehiclePath(Cam * cam);
    void drawLane(Lane & lane, std::vector <PointWorldCoord> points, QColor color = QColor(0, 0, 0), bool drawPointsOfLane = false);
    void drawPoint(PointWorldCoord point, QColor color = QColor(0, 0, 0), QSize size = QSize(3.0, 3.0));
    void drawLanesConnections(QVector<Lane> lanes, PointWorldCoord refPoint, QColor color = QColor(0, 0, 0));
    void drawGPSPositionPoint(PointWorldCoord position);
    void drawBoundingBox(RectWorldCoord rect, Cam * cam);

    void removeVehiclePath(Cam * cam);
    void removeGeometry(const std::shared_ptr<Geometry> & geometry);
    void removeGPSPositionPoint();
    void removeAllGeometries(bool deletePointGPS);

    void updateGPSPositionPoint(PointWorldCoord position);
    void geometryClickEvent(const Geometry* geometry);

    QColor getColorByLaneType(QString type);
    QColor getColorByApproach(bool isIngress);

    std::shared_ptr<GeometryPointImageScaled> GPSpositionPoint;

signals:
    void unitClick(long stationID);
    void hazardClick(long time, int code);
    void trafficLightClick(int crossroadID, int adjacentIngressLanesIndex, bool sameTrafficLight);

private:
    QMapControl * m_map_control;

    QPen pointPen;
    QPen linePen;

    std::shared_ptr<LayerGeometry> custom_layer;
    std::shared_ptr<LayerGeometry> navigation_layer;
};
