#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

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

    bool fileChanged;
    QString currentFile;
private:
    QProcess receiveDataProcess;
    QProcess loadDataProcess;
signals:

};

#endif // PROCESSHANDLER_H
