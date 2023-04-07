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

    QLabel * lblDirection;
    QLabel * lblTrafficLight;
    QLabel * lblInfo;

    int signalGroup;
    QVector <QString> maneuvers;

    std::shared_ptr<GeometryPointImageScaled> trafficLightMapPoint;

    void setTrafficLightPixmap(int state);
    void setDirectionPixmap(int state);
    void setAllowedDirection();
    void setResidualTime(int timeStamp, int moy, int likelyTime);

private:
    QPixmap trafficLightPixmap;
    QPixmap directionPixmap;
private slots:
};
