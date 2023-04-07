#pragma once

#include <QObject>
#include <QProcess>
#include "messageparser.h"

class ProcessHandler : public QObject
{
    Q_OBJECT
public:
    ProcessHandler(QObject *parent = nullptr);
    ~ProcessHandler();

    int startReceiving();
    void stopReceiving();

    int startLoading();

    bool fileChanged;
    QString currentFile;

public slots:
    void readyReadOutput();
    void readyReadError();
    void processStarted();

    void changeCommandsFromSettings();
signals:
    void error(QString error);
private:
    QProcess receiveDataProcess;
    QProcess loadDataProcess;

    QString receivingCommand;
    QString loadingCommand;
};
