#pragma once

#include "messageparser.h"

#include <QObject>
#include <QProcess>

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
