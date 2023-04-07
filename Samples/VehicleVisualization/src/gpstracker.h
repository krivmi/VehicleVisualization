#pragma once

#include <QtCore/QObject>
#include <QThread>

class GPSTracker: public QObject
{
    Q_OBJECT

public:
    GPSTracker();
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
