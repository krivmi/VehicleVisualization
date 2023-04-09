#pragma once

#include "message.h"
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
    // Singleton class
    Q_OBJECT
public:
    static MessageParser& getInstance();

    int loadJSONFromString(QString jsonString);
    void findMessagesInStream(QString messageStream);
    void clear();
signals:
    void messagesParsed(QString message);
    void messageParsed(std::shared_ptr<Message> message);
private:
    MessageParser();

    void processMessage();
    void recognizeMessage(QJsonObject jsonObject);
    void parseCAM(QJsonObject packetObj);
    void parseDENM(QJsonObject packetObj);
    void parseSREM(QJsonObject packetObj);
    void parseMAPEM(QJsonObject packetObj);
    void parseSPATEM(QJsonObject packetObj);
    void parseGEONW(QJsonObject packetObj);

    QStack <QChar> stack;
    QString json_message;
};

