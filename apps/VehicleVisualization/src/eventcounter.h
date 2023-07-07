#pragma once

#include "stdafx.h"

#include "cam.h"
#include "datahandler.h"

class EventCounter : public QObject
{
    Q_OBJECT
public:
    EventCounter();

    void start();
    void stop();
    void reset();
    void setMessageSize(int size);
    bool isRunning();
    bool messagesNotSet();
    bool areMessagesPlayed();

    bool newPlayingCycle;

public slots:
    void tick();

signals:
    void messageShow(int index);
    void messagesPlayed();
    void playingStarted();

private:
    bool m_running;
    bool messagesPlayedFlag;
    int messagesIndex;
    int messagesSize;
    int TPS;
};
