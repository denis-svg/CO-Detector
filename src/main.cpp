#include "WifiManager.h"
#include "Telegram.h"
#include "MQ7Sensor.h"
// Wifi network station credentials
#define WIFI_SSID "hostp"
#define WIFI_PASSWORD "12345678"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6997061084:AAFeIqDFbySFbdmjx4PhvU188GvOah4HUwc"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
MQ7Sensor mq7;
WiFiManager wifi(cert);
TelegramBot bot(mq7, BOT_TOKEN, wifi.getClient());

void setup()
{
  Serial.begin(9600);
  Serial.println();

  wifi.connect(WIFI_SSID, WIFI_PASSWORD);
}

void loop()
{
  bot.tick();
  delay(60);
}