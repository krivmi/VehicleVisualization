#include "gpstracker.h"

#include "libgpsmm.h"

GPSTracker::GPSTracker(): m_running(false) { }

void GPSTracker::start()
{
    m_running = true;
    trackGPS();
}
void GPSTracker::stop()
{
    m_running = false;
    emit GPSstopped();
}
void GPSTracker::trackGPS()
{
    QSettings settings("krivmi", "VehicleVisualization");
    QString host = settings.value("GPSD/host").toString();
    QString port = settings.value("GPSD/port").toString();

    std::string str_host = host.toStdString();
    std::string str_port = port.toStdString();

    gpsmm gps_rec(str_host.c_str(), str_port.c_str()); // "127.0.0.1", DEFAULT_GPSD_PORT

    if (gps_rec.stream(WATCH_ENABLE|WATCH_JSON) == NULL) {
        qInfo() << "No GPSD running.\n";
        return stop();
    }
    while(m_running) {
        struct gps_data_t* newdata;

        if (!gps_rec.waiting(5000000)){ // in mikro-seconds
              qInfo() << "Waiting...";
              continue;
        }

        if ((newdata = gps_rec.read()) == NULL)
        {
            qInfo() << "Read error.\n";
            return stop();
        }
        else
        {
            // PROCESS newdata
            //qInfo() << "Lat: " << QString::number(newdata->fix.latitude) << "Long: " << QString::number(newdata->fix.longitude);
            float latitude = newdata->fix.latitude;
            float longitude = newdata->fix.longitude;
            float orientation = newdata->fix.track;

            if(!std::isnan(latitude) && !std::isnan(longitude) && !std::isnan(orientation) && m_running)
            {
                emit resultReady(longitude, latitude, orientation);
            }
        }
    }
}
