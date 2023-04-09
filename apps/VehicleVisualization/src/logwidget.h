#pragma once

#include "message.h"
#include "cam.h"

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>

class LogWidget : public QWidget
{
     Q_OBJECT
public:
    LogWidget(QWidget *parent = nullptr);

    void setWidgetInfo(std::shared_ptr <Message> message);
    QPixmap getPixmap(std::shared_ptr <Message> message);

    QTime lastUpdateTime;

    bool widgetClickable;
    bool timerRunning = false;

    std::shared_ptr <Message> message;

public slots:
    void updateTime();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int lifeTimeOfUnit = 5 * 60; // in seconds

    QLabel * lblProtocol;
    QLabel * lblImage;
    QLabel * lblTime;

signals:
    void clicked(std::shared_ptr <Message> message);
    void lifeTimeExceeded(std::shared_ptr <Message> message);
};
