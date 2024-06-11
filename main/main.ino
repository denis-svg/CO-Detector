#include "Button.h"
#include "LED.h"
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
const int WARNING_PPM = 3;

void handle_reading(int ppm);
void alert(int ppm);
void silence_alert();

WiFiManager wifi;
Button btn = Button(pinButton);
LED led = LED(pinLED);
MQ7Sensor mq7(pinIN, pinMOSFET, pinBuzzer, maxVoltage, bitRes, handle_reading);
TelegramBot bot(mq7, BOT_TOKEN, wifi.getClient(), alert, silence_alert);
BluetoothSerial SerialBT;
void readBluetoothInput(const char *prompt, char *input, int inputSize);

bool restartBT = true;
bool alert_on = false;
bool silenced = false;

void setup() {
  Serial.begin(9600);
  startupDiagnostic();

  SerialBT.begin("ESP32-BT");
  Serial.println("Bluetooth started. Waiting for WiFi credentials...");

  xTaskCreate(&input_task, "input_task", 4096, NULL, 2, NULL);
  xTaskCreate(&MQ7_task, "MQ7_task", 4096, NULL, 1, NULL);
  xTaskCreate(&Telegram_task, "Telegram_task", 16384, NULL, 1, NULL);

  led.mode = LED::pulsating;
}

void loop() {}

void long_beep() {
  tone(pinBuzzer, 2000);
  delay(1000);
  noTone(pinBuzzer);
}

void short_beep() {
  int d = 80;
  tone(pinBuzzer, 2000);
  delay(d);
  noTone(pinBuzzer);
  delay(d);
  tone(pinBuzzer, 2000);
  delay(d);
  noTone(pinBuzzer);
}

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
      short_beep();
      led.toggle(false);

      Serial.println("Startup diagnostics finished.");
      return;
    }
    if (btn.pressed) {
      led.toggle(true);
    } else {
      led.toggle(false);
    }
  }
}

void input_task(void *pvParameter) {
  while (1) {
    led.tick();

    btn.poll();
    if (alert_on && btn.pressed) {
      silence_alert();
    }
    vTaskDelay(1);
  }
}

void MQ7_task(void *pvParameter) {
  while (1) {
    mq7.tick();
    vTaskDelay(10);
  }
}

void Telegram_task(void *pvParameter) {
  while (1) {
    if (wifi.connected()) {
      bot.tick();
    } else {
      tryBluetooth();
    }
    vTaskDelay(10);
  }
}

void tryBluetooth() {
  if (restartBT) {
    restartBT = false;
    SerialBT.begin("ESP32-BT");

    long_beep();
  }
  if (SerialBT.hasClient()) {
    short_beep();
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
    bool success = wifi.connect(ssid, password);
    if (success) {
      short_beep();
      led.mode = LED::steady;
      led.toggle(true);
    } else {
      long_beep();
    }
    restartBT = true;
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

void stop_alert() {
  alert_on = false;
  noTone(pinBuzzer);
  if (wifi.connected()) {
    led.mode = LED::steady;
    led.toggle(true);
  } else {
    led.mode = LED::pulsating;
  }
}

void silence_alert() {
  noTone(pinBuzzer);
  Serial.println("Alert silenced.");
}

void alert(int ppm) {
  alert_on = true;

  noTone(pinBuzzer);
  tone(pinBuzzer, 2000);
  led.mode = LED::flickering;
  bot.handle_reading(ppm);
}

void handle_reading(int ppm) {
  Serial.print(">ppm:");
  Serial.println(ppm);

  if (ppm > WARNING_PPM) {
    alert(ppm);
  } else {
    stop_alert();
  };
}
