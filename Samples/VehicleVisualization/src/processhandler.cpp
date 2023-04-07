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
void ProcessHandler::readyReadOutput(){
    //qInfo() << receiveDataProcess->readAllStandardOutput();
    //MessageParser::getInstance().clear();
    MessageParser::getInstance().findMessagesInStream((QString)receiveDataProcess.readAllStandardOutput());
}
void ProcessHandler::readyReadError(){
    qInfo() << "Ajaaaj";
    qInfo() << receiveDataProcess.readAllStandardError();
}
void ProcessHandler::processStarted(){
    qInfo() << "Receiving started...";
}
int ProcessHandler::startReceiving(){
    //QString cmd2 = "/bin/sh -c \"tshark -r /tmp/tcpdump_data -T json";
    QString cmd2 = "/bin/sh -c \"echo krivanek | sudo -S stdbuf -i0 -o0 -e0 tshark -i hwsim0 -T json"; //

    receiveDataProcess.start(cmd2); // this->receivingCommand

    QObject::connect(&receiveDataProcess, &QProcess::started, this, &ProcessHandler::processStarted);
    QObject::connect(&receiveDataProcess, &QProcess::readyReadStandardOutput, this, &ProcessHandler::readyReadOutput);
    QObject::connect(&receiveDataProcess, &QProcess::readyReadStandardError, this, &ProcessHandler::readyReadError);

    if(receiveDataProcess.state() != QProcess::NotRunning){
        return 0;
    }
    qInfo() << "Process could not be started.";
    //emit error(receiveDataProcess.readAllStandardError());
    return 1;
}
void ProcessHandler::stopReceiving(){
    receiveDataProcess.close();
    qInfo() << "Receiving stoped...";
    MessageParser::getInstance().clear();
}

int ProcessHandler::startLoading(){
    qInfo() << "Loading file: " << this->currentFile;

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
        qInfo() << "Messages were loaded successfully from the file...";
    } else {
        qInfo() << "loading was unsuccessfull...";
    }

    return 0;
}

