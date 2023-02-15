#ifndef EVENTCOUNTER_H
#define EVENTCOUNTER_H

#include "cam.h"
#include "datahandler.h"
#include <QtCore/QObject>
#include <QMapControl/Point.h>

using namespace qmapcontrol;

class EventCounter : public QObject
{
    Q_OBJECT
public:
    EventCounter();
    ~EventCounter() { }

    void start();
    void stop();
    void reset();
    void setMessageSize(int size);
    bool isRunning();
    bool messagesNotSet();
    bool areMessagesPlayed();

    bool newPlayingCycle = true;

public slots:
    void tick();

signals:
    void messageShow(int index);
    void messagesPlayed();

private:
    bool m_running = false;
    bool messagesPlayedFlag = true;
    int messagesIndex = 0;
    int messagesSize = 0;
    int TPS = 10;

    //QVector <Message*> messages;
};

#endif // EVENTCOUNTER_H
