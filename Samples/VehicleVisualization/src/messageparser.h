#pragma once

#include "cam.h"
#include "mapem.h"
#include "spatem.h"
#include "srem.h"
#include "denm.h"

#include <QStack>
#include <QChar>
#include <QObject>

class MessageParser : public QObject
{
    Q_OBJECT
public:
    static MessageParser& getInstance(){
        static MessageParser instance; // Guaranteed to be destroyed, Instantiated on first use.
        return instance;
    }

    bool findMessagesInStream(QString messageStream);
    void processMessage();
    int loadJSONFromString(QString jsonString);
    QString printMessage();

    void recognizeMessage(QJsonObject jsonObject);
    void parseCAM(QJsonObject packetObj);
    void parseDENM(QJsonObject packetObj);
    void parseSREM(QJsonObject packetObj);
    void parseMAPEM(QJsonObject packetObj);
    void parseSPATEM(QJsonObject packetObj);
    void parseGEONW(QJsonObject packetObj);

signals:
    void messagesParsed(QString message);
    void messageParsed(std::shared_ptr<Message> message);

private:
    MessageParser();
    QStack <QChar> stack;
    QString json_message;
};

