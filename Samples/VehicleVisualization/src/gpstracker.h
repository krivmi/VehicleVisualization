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
    void resultReady(float longitude, float latitude);
private:
    bool m_running = false;
};
