#include "processhandler.h"
#include <QProcess>
#include <QDebug>
#include <QDir>

#include "messageparser.h"

ProcessHandler::ProcessHandler(QObject *parent): QObject{parent}
{
    this->fileChanged = false;
    //this->currentFile = "/home/krivmi/QT_projects/QMapControl/Samples/resources/trafficFiles/ukazka_80211p.pcap";

    this->currentFile = "/home/krivmi/QT_projects/QMapControl/Samples/resources/trafficFiles/capture_X1_02.pcap";

}
int ProcessHandler::startReceiving(){
    //QString cmd2 = "/bin/sh -c \"echo krivanek | sudo -S stdbuf -i0 -o0 -e0 tshark -i hwsim0 -T json"; //
    QString cmd2 = "/bin/sh -c \"tshark -r /tmp/tcpdump_data -T json";

    QObject::connect(&receiveDataProcess, &QProcess::started, [=]{
        qDebug() << "Receiving started...";
    });
    QObject::connect(&receiveDataProcess, &QProcess::readyReadStandardOutput, [=]{
        //qDebug() << receiveDataProcess->readAllStandardOutput();
        MessageParser::getInstance().findMessagesInStream((QString)receiveDataProcess.readAllStandardOutput());
    });
    QObject::connect(&receiveDataProcess, &QProcess::readyReadStandardError, [=]{
        //qDebug() << "read error";
        qDebug() << receiveDataProcess.readAllStandardError();
    });

    receiveDataProcess.start(cmd2);

    if(receiveDataProcess.state() != QProcess::NotRunning){
        return 0;
    }
    qInfo() << "Process could not be started.";
    return 1;
}

int ProcessHandler::startLoading(){
    qInfo() << this->currentFile;
    QString cmd = "/bin/sh -c \"tshark -r " + this->currentFile +" -T json";

    loadDataProcess.start(cmd);
    loadDataProcess.waitForFinished(-1); // will wait forever until finished

    QString stdout = loadDataProcess.readAllStandardOutput();
    QString stderr = loadDataProcess.readAllStandardError();
    loadDataProcess.close();

    if(!stderr.isEmpty()){
        qInfo() << "Error with getting pcap.";
        return 1;
    }

    if(MessageParser::getInstance().loadJSONFromString(stdout) == 0){
        qInfo() << "Messages were loaded successfully from the file.";
    }

    return 0;
}

