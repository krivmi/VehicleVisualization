#include "eventcounter.h"

EventCounter::EventCounter() : newPlayingCycle(true), m_running(false),
    messagesPlayedFlag(true), messagesIndex(0), messagesSize(0), TPS(10) { }

void EventCounter::start()
{
    // check if we are not already running
    if(!m_running)
    {
        m_running = true;

        if(messagesPlayedFlag)
        {
            messagesPlayedFlag = false;
        }

        // Set a timer to start the 'ticking' (1/TPS of a second).
        QTimer::singleShot(1000 / TPS, this, SLOT(tick()));
    }

}
void EventCounter::stop()
{
    // Set to stop runnings.
    m_running = false;
}
bool EventCounter::isRunning()
{
    return m_running;
}
bool EventCounter::areMessagesPlayed()
{
    return messagesPlayedFlag;
}
void EventCounter::reset(){
    m_running = false;
    messagesPlayedFlag = true;
    messagesIndex = 0;
}
void EventCounter::setMessageSize(int size){
    messagesSize = size;
}
bool EventCounter::messagesNotSet(){
    return messagesSize == 0;
}
void EventCounter::tick()
{
    // check if we have a message to send
    if(messagesIndex < messagesSize)
    {
        if(messagesIndex == 0){
            emit playingStarted();
        }
        emit messageShow(messagesIndex);
        qInfo() << "TICK" << ++messagesIndex;
    }
    else
    {
        messagesIndex = 0;
        m_running = false;
        messagesPlayedFlag = true;
        emit messagesPlayed();
    }

    // Are we still running?
    if(m_running)
    {
        // Schedule the next tick (1000/TPS of a second).
        QTimer::singleShot(1000 / TPS, this, SLOT(tick()));
    }
}
