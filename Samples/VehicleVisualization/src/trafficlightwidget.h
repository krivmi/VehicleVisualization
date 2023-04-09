#pragma once

#include "mapem.h"

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

#include <QMapControl/GeometryPointImageScaled.h>

class TrafficLightWidget : public QWidget
{
     Q_OBJECT
public:
    TrafficLightWidget(int signalGroup, QVector <QString> maneuvers, QWidget *parent = nullptr);

    void setTrafficLightPixmap(int state);
    void setDirectionPixmap(int state);
    void setAllowedDirection();
    void setResidualTime(int timeStamp, int moy, int likelyTime);

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
