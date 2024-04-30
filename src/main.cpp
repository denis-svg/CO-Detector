#include "WifiManager.h"
#include "Telegram.h"
#include "MQ7Sensor.h"
// Wifi network station credentials
#define WIFI_SSID "hostp"
#define WIFI_PASSWORD "12345678"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6997061084:AAFeIqDFbySFbdmjx4PhvU188GvOah4HUwc"
void alert(int ppm);
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
int pinIN = A0,  pinMOSFET=D5, pinButton=D7, pinBuzzer=D4, pinLED=D6, maxVoltage=5, bitRes=1023;

MQ7Sensor mq7(pinIN, pinMOSFET, pinButton, pinBuzzer, pinLED,
             maxVoltage, bitRes, alert);
WiFiManager wifi(cert);
TelegramBot bot(mq7, BOT_TOKEN, wifi.getClient());

void setup()
{
  Serial.begin(9600);
  Serial.println();

  wifi.connect(WIFI_SSID, WIFI_PASSWORD);
}

void alert(int ppm){
    bot.alert(ppm);
}

void loop()
{
  bot.tick();
  mq7.tick();
}