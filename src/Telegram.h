#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <map>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include "MQ7Sensor.h"

class TelegramBot
{
public:
    TelegramBot(MQ7Sensor &mq7, String BOT_TOKEN, WiFiClientSecure &secured_client);
    void tick();
    void alert(int ppm);

private:
    void handleNewMessages(int numNewMessages);
    MQ7Sensor *mq7_sensor;
    std::map<String, char> chats;
    WiFiClientSecure *secured_client;
    UniversalTelegramBot bot;
};
#endif