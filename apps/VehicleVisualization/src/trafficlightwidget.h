#pragma once

#include "stdafx.h"

#include "mapem.h"

class TrafficLightWidget : public QWidget
{
     Q_OBJECT
public:
    TrafficLightWidget(int signalGroup, QVector <QString> maneuvers, QWidget *parent = nullptr);

    void setTrafficLightPixmap(int state);
    void setDirectionPixmap(int state);
    void setAllowedDirection();
    void setResidualTime(int timeStamp, int moy, int likelyTime);
    void setResidualTime(bool noData);

    int signalGroup;
    QVector <QString> maneuvers;
    std::shared_ptr<GeometryPointImageScaled> trafficLightMapPoint;

private:
    QLabel * lblDirection;
    QLabel * lblTrafficLight;
    QLabel * lblInfo;

    QPixmap trafficLightPixmap;
    QPixmap directionPixmap;
};
