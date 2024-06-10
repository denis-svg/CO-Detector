#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

class WiFiManager
{
public:
  WiFiManager() {}

  void begin()
  {
    WiFi.mode(WIFI_STA);
    disconnect();
  }

  bool connect(char *ssid, char *password) {
    Serial.println();

    // attempt to connect to Wifi network:
    Serial.print("Connecting to Wifi SSID ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    _client.setCACert(
        TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < MAX_WIFI_RETRIES) {
      delay(100);
      retries += 1;
    }

    if (!connected()) {
      Serial.println("WiFi connection failed.");
      return false;
    }
    Serial.print("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
    configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
    return true;
  }

  void disconnect() {
    WiFi.disconnect();
    Serial.println("WiFi disconnected.");
  }

  WiFiClientSecure &getClient()
  {
    return _client;
  }

  bool connected()
  {
    return WiFi.status() == WL_CONNECTED;
  }

private:
  WiFiClientSecure _client;
  static const int MAX_WIFI_RETRIES = 300;
};

#endif
