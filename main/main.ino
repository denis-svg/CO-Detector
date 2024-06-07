#include "MQ7Sensor.h"
#include "Telegram.h"
#include "WifiManager.h"
#include <BluetoothSerial.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BLINK_GPIO 2

#define BOT_TOKEN "6997061084:AAFeIqDFbySFbdmjx4PhvU188GvOah4HUwc"

int pinIN = 36, pinMOSFET = 33, pinButton = 25, pinBuzzer = 32, pinLED = 26,
    maxVoltage = 5, bitRes = 4096;

void alert(int ppm);

WiFiManager wifi;
Button btn = Button(pinButton);
MQ7Sensor mq7(pinIN, pinMOSFET, pinButton, pinBuzzer, pinLED, maxVoltage,
              bitRes, alert);
TelegramBot bot(mq7, BOT_TOKEN, wifi.getClient());
BluetoothSerial SerialBT;
void readBluetoothInput(const char *prompt, char *input, int inputSize);
bool restarting = true;

void startupDiagnostic() {
  unsigned long btn_down_t = 0;
  Serial.println("Hold button for two seconds to test components.");

  while (1) {
    btn.poll();
    if (btn.stateJustChanged && btn.pressed) {
      btn_down_t = millis();
    }
    unsigned long hold_t = millis() - btn_down_t;
    if (hold_t > 2000 && btn.pressed) {
      tone(pinBuzzer, 2000);
      delay(100);
      noTone(pinBuzzer);
      delay(100);
      tone(pinBuzzer, 2000);
      delay(100);
      noTone(pinBuzzer);
      digitalWrite(pinLED, LOW);

      Serial.println("Startup diagnostics finished.");
      return;
    }
    if (btn.pressed) {
      digitalWrite(pinLED, HIGH);
    } else {
      digitalWrite(pinLED, LOW);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(pinIN, INPUT);
  pinMode(pinMOSFET, OUTPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  startupDiagnostic();

  SerialBT.begin("ESP32-BT");
  Serial.println("Bluetooth started. Waiting for WiFi credentials...");

  xTaskCreate(&MQ7_task, "MQ7_task", 2048, NULL, 1, NULL);
  xTaskCreate(&Telegram_task, "Telegram_task", 8192, NULL, 1, NULL);
}

void loop() {}

void MQ7_task(void *pvParameter) {
  while (1) {
    mq7.tick();
    vTaskDelay(100 / portTICK_RATE_MS);
  }
}

void Telegram_task(void *pvParameter) {
  while (1) {
    if (wifi.connected()) {
      bot.tick();
    } else {
      tryBluetooth();
    }
    vTaskDelay(100 / portTICK_RATE_MS);
  }
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
