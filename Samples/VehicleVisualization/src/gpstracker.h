#pragma once

// Qt includes.
#include <QtCore/QObject>
#include <QThread>

class GPSTracker: public QObject
{
    Q_OBJECT

public:
    GPSTracker();
    ~GPSTracker() { }
    void stop();
    void start();

public slots:
    void trackGPS();
signals:
    void resultReady(float longitude, float latitude, float orientation);
    void GPSstopped();
private:
    bool m_running = false;
    const char* host;
    const char* port;
};
