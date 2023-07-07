#include "processhandler.h"

ProcessHandler::ProcessHandler(QObject *parent): QObject{parent},
    fileChanged(false), currentFile("")
{
    this->changeCommandsFromSettings();
}
ProcessHandler::~ProcessHandler()
{
    loadDataProcess.close();
    receiveDataProcess.close();
}

void ProcessHandler::changeCommandsFromSettings()
{
    QSettings settings("krivmi", "VehicleVisualization");
    this->receivingCommand = settings.value("receivingCommand").toString();
    this->loadingCommand = settings.value("loadingCommand").toString();
}

void ProcessHandler::readyReadOutput(){
    // get MessageParser and parse messages in a ready output stream
    MessageParser::getInstance().findMessagesInStream((QString)receiveDataProcess.readAllStandardOutput());
}
void ProcessHandler::readyReadError(){
    // oftenly it is not actually an error but a warning
    qInfo() << receiveDataProcess.readAllStandardError();
}
void ProcessHandler::processStarted(){
    qInfo() << "Receiving started: " << this->receivingCommand;
}
int ProcessHandler::startReceiving()
{
    //QString cmd2 = "/bin/sh -c \"tshark -r /tmp/tcpdump_data -T json";
    QString cmd2 = "/bin/sh -c \"echo krivanek | sudo -S stdbuf -i0 -o0 -e0 tshark -i hwsim0 -T json"; //

    receiveDataProcess.start(this->receivingCommand); // cmd2
    qInfo() << "Receiving command: " << this->receivingCommand;

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
void ProcessHandler::stopReceiving()
{
    receiveDataProcess.close();
    qInfo() << "Receiving stoped...";
    MessageParser::getInstance().clear();
}

int ProcessHandler::startLoading(){

    if(this->currentFile.isEmpty()){
        qInfo() << "Loading file has not yet been specified...";
    }
    qInfo() << "Loading file: " << this->currentFile;

    //QString cmd = "/bin/sh -c \"tshark -r " + this->currentFile +" -T json";
    QString cmd = this->loadingCommand;
    cmd.replace("%FILE", this->currentFile); // path + filename

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

