#include "WifiManager.h"
#include "MQ7Sensor.h"
#include "Telegram.h"

#define WIFI_SSID "hostp"
#define WIFI_PASSWORD "12345678"
#define BOT_TOKEN "6997061084:AAFeIqDFbySFbdmjx4PhvU188GvOah4HUwc"

int pinIN = 36, pinMOSFET = 19, pinButton = 34, pinBuzzer = 5, pinLED = 18, maxVoltage = 5, bitRes = 1023;
void alert(int ppm);
WiFiManager wifi;
MQ7Sensor mq7(pinIN, pinMOSFET, pinButton, pinBuzzer, pinLED,
              maxVoltage, bitRes, alert);
TelegramBot bot(mq7, BOT_TOKEN, wifi.getClient());

void setup()
{
  Serial.begin(9600);
  wifi.begin();
  wifi.connect(WIFI_SSID, WIFI_PASSWORD);
}

void loop()
{
  mq7.tick();
  bot.tick();
}

void alert(int ppm)
{
  bot.alert(ppm);
}
