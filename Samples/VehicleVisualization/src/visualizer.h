#pragma once

#include "message.h"
#include "cam.h"
#include "mapem.h"
#include "spatem.h"
#include "denm.h"
#include "dialogs.h"

// Qt includes.
#include <QApplication>
#include <QVector>

// QMapControl includes.
#include <QMapControl/QMapControl.h>
#include <QMapControl/GeometryLineString.h>
#include <QMapControl/GeometryPointCircle.h>
#include <QMapControl/GeometryPolygon.h>
#include <QMapControl/GeometryPolygonImage.h>
#include <QMapControl/GeometryWidget.h>
#include <QMapControl/Layer.h>
#include <QMapControl/LayerGeometry.h>

using namespace qmapcontrol;

class Visualizer : public QObject
{
    Q_OBJECT

public:
    Visualizer(QMapControl * mapWidget);


    std::shared_ptr<GeometryPointImageScaled> GPSpositionPoint;

    void visualizeCrossroad();
    void addCamStationToLayer(Cam * cam);
    void addDenmHazardToLayer(Denm * denm);
    void addTrafficLight(Mapem * crossroad, std::shared_ptr<GeometryPointImageScaled> & geometryPoint, int adjacentIngressLanesIndex, bool sameTrafficLight);

    QColor getColorByLaneType(QString type);
    QColor getColorByApproach(bool isIngress);
    long getConnectedLaneID(ConnectingLane lane, QVector <Lane> lanes);
    std::vector <PointWorldCoord> getLanePoints(Lane lane, PointWorldCoord refPoint);

    void drawLanes(QVector<Lane> & lanes, PointWorldCoord refPoint, int laneType, bool drawPointsOfLane = false);
    //void drawReferencePoint(PointWorldCoord point, QColor color = QColor(0, 0, 0));
    void drawVehiclePath(Cam * cam);
    void drawLane(Lane & lane, std::vector <PointWorldCoord> points, QColor color = QColor(0, 0, 0), bool drawPointsOfLane = false);
    void drawPoint(PointWorldCoord point, QColor color = QColor(0, 0, 0), QSize size = QSize(3.0, 3.0));
    //void drawPoints(std::vector <PointWorldCoord> points, QColor color = QColor(0, 0, 0));
    void drawLanesConnections(QVector<Lane> lanes, PointWorldCoord refPoint, QColor color = QColor(0, 0, 0));
    Lane getLaneByID(long id, QVector<Lane> lanes);
    void removeVehiclePath(Cam * cam);
    void removeGeometry(const std::shared_ptr<Geometry> & geometry);
    void geometryClickEvent(const Geometry* geometry);

    void updateGPSPositionPoint(PointWorldCoord position);
    void drawGPSPositionPoint(PointWorldCoord position);
    void removeGPSPositionPoint();
    void removeAllGeometries(bool deletePointGPS);

    void drawBoundingBox(RectWorldCoord rect, Cam * cam);
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
