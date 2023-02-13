#ifndef LOGWIDGET_H
#define LOGWIDGET_H

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

    QLabel * lblProtocol;
    QLabel * lblImage;
    QLabel * lblTime;

    QTime lastUpdateTime;

    bool widgetClickable;

    std::shared_ptr <Message> message;

    void setWidgetInfo(std::shared_ptr <Message> message);
    QPixmap getPixmap(std::shared_ptr <Message> message);
public slots:
    void updateTime();
protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool timerRunning = false;
    int lifeTimeOfUnit = 5 * 60; // in seconds
signals:
    void clicked(std::shared_ptr <Message> message);
    void lifeTimeExceeded(std::shared_ptr <Message> message);
};

#endif
