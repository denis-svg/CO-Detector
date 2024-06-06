#include "MQ7Sensor.h"
#include "Telegram.h"
#include "WifiManager.h"
#include <BluetoothSerial.h>

#define BOT_TOKEN "6997061084:AAFeIqDFbySFbdmjx4PhvU188GvOah4HUwc"

int pinIN = 36, pinMOSFET = 33, pinButton = 25, pinBuzzer = 32, pinLED = 26,
    maxVoltage = 5, bitRes = 4096;
void alert(int ppm);
WiFiManager wifi;
MQ7Sensor mq7(pinIN, pinMOSFET, pinButton, pinBuzzer, pinLED, maxVoltage,
              bitRes, alert);
TelegramBot bot(mq7, BOT_TOKEN, wifi.getClient());

BluetoothSerial SerialBT;
void readBluetoothInput(const char *prompt, char *input, int inputSize);
bool restarting = true;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32-BT");
  Serial.println(
      "Setup complete. Waiting for WiFi credentials via Bluetooth...");
}

void loop() {
  if (wifi.connected()) {
    bot.tick();
  } else {
    tryBluetooth();
  }
  mq7.tick();
}

void tryBluetooth() {
  if (restarting) {
    SerialBT.begin("ESP32-BT");
    restarting = false;
  }
  if (SerialBT.hasClient()) {
    wifi.disconnect();

    char ssid[32];
    char password[32];

    Serial.println("Waiting for Bluetooth input...");
    SerialBT.println("Waiting for Bluetooth input...");

    readBluetoothInput("SSID", ssid, 32);
    SerialBT.print("Received SSID: ");
    SerialBT.println(ssid);

    readBluetoothInput("Password", password, 32);
    SerialBT.print("Received Password: ");
    SerialBT.println(password);
    delay(100);
    SerialBT.end();

    Serial.println("Turning on WiFi...");
    wifi.begin();
    wifi.connect(ssid, password);
    restarting = true;
  }
}

void readBluetoothInput(const char *prompt, char *input, int inputSize) {
  SerialBT.print("Enter ");
  SerialBT.println(prompt);

  // Clear any leftover characters in the Bluetooth buffer
  while (SerialBT.available()) {
    SerialBT.read();
  }

  int index = 0;
  while (true) {
    while (SerialBT.available()) {
      char ch = SerialBT.read();
      if (ch == '\n' || ch == '\r' || index >= inputSize - 1) {
        input[index] = '\0';
        return;
      }
      input[index] = ch;
      index += 1;
    }
  }
}

void alert(int ppm)
{
  bot.alert(ppm);
}
