#ifndef TELEGRAM_H
#define TELEGRAM_H

#include "MQ7Sensor.h"
#include <Arduino.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <map>

class TelegramBot {
public:
  TelegramBot(MQ7Sensor &mq7, String BOT_TOKEN,
              WiFiClientSecure &secured_client, void (*alert)(int),
              void (*silence_alert)());

  void handle_message(String chat_id, String text, String from_name);
  void tick();
  void handle_reading(int ppm);
  void (*alert)(int);
  void (*silence_alert)();

private:
  void handleNewMessages(int numNewMessages);
  MQ7Sensor *mq7_sensor;
  std::map<String, char> chats;
  WiFiClientSecure *secured_client;
  UniversalTelegramBot bot;
};
#endif
