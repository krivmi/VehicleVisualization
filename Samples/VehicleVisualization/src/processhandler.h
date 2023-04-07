#pragma once

#include <QObject>
#include <QProcess>
#include "messageparser.h"

class ProcessHandler : public QObject
{
    Q_OBJECT
public:
    ProcessHandler(QObject *parent = nullptr);
    ~ProcessHandler(){
        loadDataProcess.close();
        receiveDataProcess.close();
    }
    int startReceiving();
    int startLoading();

    void stopReceiving();

    bool fileChanged;
    QString currentFile;
public slots:
    void readyReadOutput();
    void readyReadError();
    void processStarted();

signals:
    void error(QString error);
private:
    QProcess receiveDataProcess;
    QProcess loadDataProcess;
    QString receivingCommand;
    QString loadingCommand;
};
