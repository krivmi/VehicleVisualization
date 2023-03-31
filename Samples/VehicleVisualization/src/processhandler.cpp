#include "processhandler.h"
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QSettings>

#include "messageparser.h"

ProcessHandler::ProcessHandler(QObject *parent): QObject{parent}
{
    QSettings settings(":/resources/config/app.ini", QSettings::IniFormat);
    this->receivingCommand = settings.value("receivingCommand").toString();
    this->loadingCommand = settings.value("loadingCommand").toString();
    QString projectPath = settings.value("projectPath").toString();
    //qInfo() << settings.fileName();

    this->fileChanged = false;
    this->currentFile = projectPath + "Samples/resources/trafficFiles/capture_X1_02.pcap";
}
int ProcessHandler::startReceiving(){
    //QString cmd2 = "/bin/sh -c \"tshark -r /tmp/tcpdump_data -T json";

    QObject::connect(&receiveDataProcess, &QProcess::started, [=]{
        qDebug() << "Receiving started...";
    });
    QObject::connect(&receiveDataProcess, &QProcess::readyReadStandardOutput, [=]{
        //qDebug() << receiveDataProcess->readAllStandardOutput();
        MessageParser::getInstance().findMessagesInStream((QString)receiveDataProcess.readAllStandardOutput());
    });
    QObject::connect(&receiveDataProcess, &QProcess::readyReadStandardError, [=]{
        //qDebug() << receiveDataProcess.readAllStandardError();
        emit error(receiveDataProcess.readAllStandardError());
    });

    receiveDataProcess.start(this->receivingCommand);

    if(receiveDataProcess.state() != QProcess::NotRunning){
        return 0;
    }
    qInfo() << "Process could not be started.";
    return 1;
}
void ProcessHandler::stopReceiving(){
    receiveDataProcess.close();
    qDebug() << "Receiving stoped...";
}

int ProcessHandler::startLoading(){
    qInfo() << this->currentFile;

    //QString cmd = "/bin/sh -c \"tshark -r " + this->currentFile +" -T json";
    QString cmd = this->loadingCommand;
    cmd.replace("%FILE", this->currentFile);

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

