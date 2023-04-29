#include "logwidget.h"

#include <QDebug>
#include <QDate>
#include <QDateTime>
#include <QPixmap>
#include <QIcon>

LogWidget::LogWidget(QWidget *parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    this->setStyleSheet("border: 1px solid grey");
    this->setStyleSheet("background-color: white");
    this->setMinimumHeight(40);
    this->setMaximumHeight(40);

    lblProtocol = new QLabel();
    lblProtocol->setFixedWidth(85);
    lblImage = new QLabel();
    lblImage->setFixedWidth(32);
    lblTime = new QLabel();

    lblProtocol->setContentsMargins(5, 0, 0, 0);
    lblTime->setContentsMargins(5, 0, 5, 0);

    layout->addWidget(lblProtocol);
    layout->addWidget(lblImage);
    layout->addWidget(lblTime);
}

void LogWidget::setWidgetInfo(std::shared_ptr <Message> message) {
    if(message != nullptr){
        lblProtocol->setText(message->GetProtocol());

        QPixmap pixmap(this->getPixmap(message));
        lblImage->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio));

        //lblTime->setText(" - " + QString::number(message->stationID));
        lblTime->setText("Update: 0 sec ago");

        QTimer::singleShot(1000, this, SLOT(updateTime()));

        this->message = message;
        lastUpdateTime = QTime::currentTime();
        timerRunning = true;

        if(message->GetProtocol() == "Denm"){
            lifeTimeOfUnit = 10;
        } else if(message->GetProtocol() == "Mapem"){
            lifeTimeOfUnit = 120;
        } else if(message->GetProtocol() == "Srem"){
            lifeTimeOfUnit = 60;
        } else if(message->GetProtocol() == "Spatem"){
            lifeTimeOfUnit = 60;
        } else if(message->GetProtocol() == "Cam"){
            lifeTimeOfUnit = 60;
        } else {
            lifeTimeOfUnit = 60;
        }

    } else {
        timerRunning = false;
        lblProtocol->clear();
        lblImage->clear();
        lblTime->clear();
    }

}
void LogWidget::updateTime() {
    if(timerRunning){
        QTime now = QTime::currentTime();
        int lastUpdate = lastUpdateTime.secsTo(now);

        if(lastUpdate > 60){
            lblTime->setText("Update: " + QString::number((int)lastUpdate / 60) + " min ago");
        } else{
            lblTime->setText("Update: " + QString::number(lastUpdate) + " sec ago");
        }
        if(lastUpdate > lifeTimeOfUnit){
            // life time exceeded
            emit lifeTimeExceeded(this->message);
        } else{
            // else continue updating unit
            QTimer::singleShot(1000, this, SLOT(updateTime()));
        }

    }
}

QPixmap LogWidget::getPixmap(std::shared_ptr <Message> message){
    QString protocol = message->GetProtocol();

    if(protocol == "Cam"){
        return QPixmap(std::static_pointer_cast<Cam>(message)->imgSrcDefault);
    } else if(protocol == "Mapem"){
        return QPixmap(":/resources/images/crossroadIcon.png");
    } else if(protocol == "Spatem"){
        return QPixmap(":/resources/images/SPATEM_icon.png");
    } else if(protocol == "Srem"){
        return QPixmap(":/resources/images/SREM_icon.png");
    } else if(protocol == "Denm"){
        return QPixmap(":/resources/images/DENM_icon.png");
    }
    else{
        qInfo() << "Something went wrong";
        return QPixmap();
    }
}

void LogWidget::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton) {
        emit clicked(this->message);
    }
}
