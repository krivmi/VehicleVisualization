include "trafficlightwidget.h"


#include <QDebug>
#include <QDate>
#include <QDateTime>
#include <QPixmap>

TrafficLightWidget::TrafficLightWidget(int signalGroup, QVector <QString> maneuvers, QWidget *parent)
{
    this->signalGroup = signalGroup;
    this->maneuvers = maneuvers;

    QVBoxLayout *layout = new QVBoxLayout(this); // this keyword is iimportant, we tell the widget the parent withn be showed
    layout->setMargin(0);
    layout->setSpacing(0);

    lblDirection = new QLabel();
    lblDirection->setAlignment(Qt::AlignCenter);
    lblDirection->setMargin(0); // to stretch the widget
    //lblDirection->setMaximumSize(QSize(parent->width(), 150));
    lblDirection->setMinimumHeight(100);
    //lblDirection->setScaledContents(true);
    //lblDirection->setFixedSize(128, 128);

    lblTrafficLight = new QLabel();
    lblTrafficLight->setAlignment(Qt::AlignCenter);
    lblTrafficLight->setMargin(0); // to stretch the widget
    lblTrafficLight->setMinimumHeight(100);
    //lblTrafficLight->setMaximumSize(QSize(parent->width(), 128));
    //lblTrafficLight->setScaledContents(true);
    //lblTrafficLight->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    setTrafficLightPixmap(0);

    lblInfo = new QLabel(QString("Next state: 0 s"));
    lblInfo->setAlignment(Qt::AlignCenter);
    lblInfo->setMinimumHeight(20);
    //lblInfo->setMaximumSize(QSize(parent->width(), 30));
    lblInfo->setMargin(0); // to stretch the widget

    layout->addWidget(lblDirection);
    layout->addWidget(lblTrafficLight);
    layout->addWidget(lblInfo, Qt::AlignTop);
    layout->addStretch();

    setAllowedDirection();

}
void TrafficLightWidget::setResidualTime(int timeStamp, int moy, int likelyTime){
    // Data elements of type TimeMark (i.e. ‘startTime’, ‘minEndTime’, ‘maxEndTime’, ‘likelyTime’, ‘nextTime’)
    // shall represent 1/10 s in the hour in which the state change may occur

    QTime pseudoCurrentTime = QTime(0,0).addSecs(moy * 60).addMSecs(timeStamp);
    QTime likelyTimeTime = QTime(pseudoCurrentTime.hour(),0).addMSecs(likelyTime * 100); // * 100 because I have to divide it by 10 in the first place (converting likely timemark to seconds)

    if((likelyTime / 10) < ((moy % 60) * 60)){
        // the TimeMark corresponds to the hour following the hour represented by ‘moy’.
        likelyTimeTime = likelyTimeTime.addSecs(3600);
        qInfo() << "Added the following hour";
    }

    //qInfo() << pseudoCurrentTime.toString("hh:mm:ss_zzz");
    //qInfo() << likelyTimeTime.toString("hh:mm:ss_zzz");

    QString text;
    if(pseudoCurrentTime > likelyTimeTime){
        //qInfo() << "This crossroad does not send residual time or the data are too old";
        text = "No info";
    } else{
        int residualTime = pseudoCurrentTime.msecsTo(likelyTimeTime);
        //qInfo() << residualTime;
        text = "Next state: " + QString::number(residualTime) + "ms";
    }
    lblInfo->setText(text);
}

void TrafficLightWidget::setAllowedDirection(){

    if(maneuvers.size() == 0)
    {
        qInfo() << "Something went wrong";
    }
    else if(maneuvers.size() == 1)
    {
        if(maneuvers.at(0) == "straight")
        {
            setDirectionPixmap(0);
        }
        else if(maneuvers.at(0) == "right")
        {
            setDirectionPixmap(1);
        }
        else if(maneuvers.at(0) == "left"){
            setDirectionPixmap(2);
        }
        else{ // left
            qInfo() << "Something went wrong";
        }
    }
    else if(maneuvers.size() == 2)
    {
        if(maneuvers.contains("straight") && maneuvers.contains("left"))
        {
            setDirectionPixmap(3);
        }
        else if(maneuvers.contains("straight") && maneuvers.contains("right"))
        {
            setDirectionPixmap(4);
        }
    }
    else{
        setDirectionPixmap(-1);
    }
}
void TrafficLightWidget::setTrafficLightPixmap(int state){
    int w = 100;
    int h = 100;

    QString imgSrc;

    switch(state){
        case 0: // unavailable - unknown or error
            imgSrc = ":/resources/images/trafficLights.png";
            break;
        case 1: // dark - traffic signal is unlit
            imgSrc = ":/resources/images/trafficLights.png";
            break;
        case 2: // red (Left turn on red (NA)) - stop-Then-Proceed
        case 3: // red - stop-And-Remain
            imgSrc = ":/resources/images/redPermission.png";
            break;
        case 4: // red+yellow - pre-Movement
            imgSrc = ":/resources/images/redYellowPermission.png";
            break;
        case 5: // green - permissive-Movement-Allowed
        case 6:
            imgSrc = ":/resources/images/greenPermission.png";
            break;
        case 7: // yellow (filtered) - permissive-clearance
        case 8: // yellow (Controlled)) - protected-clearance
            imgSrc = ":/resources/images/yellowPermission.png";
            break;
        default:
            imgSrc = ":/resources/images/trafficLights.png";
    }


    QPixmap pixmap(imgSrc);
    lblTrafficLight->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));

    QPixmap pixmap2(imgSrc);

    if(this->trafficLightMapPoint != nullptr){
        this->trafficLightMapPoint->setImage(pixmap2, true);
        //qInfo() << "changed";
    }

}
void TrafficLightWidget::setDirectionPixmap(int state){
    int w = 100;
    int h = 100;

    QString imgSrc;

    switch(state){
        case 0: // straight
            imgSrc = ":/resources/images/straight.png";
            break;
        case 1: // right
            imgSrc = ":/resources/images/right.png";
            break;
        case 2: // left
            imgSrc = ":/resources/images/left.png";
            break;
        case 3: // straight-left
            imgSrc = ":/resources/images/straight_left.png";
            break;
        case 4: // straight-right
            imgSrc = ":/resources/images/straight_right.png";
            break;
        default: // straight-left-right
            imgSrc = ":/resources/images/straight_left_right.png";
    }
    QPixmap pixmap(imgSrc);
    lblDirection->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
}
