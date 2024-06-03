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

  bool connect(const char *ssid, const char *password)
  {
    Serial.println();

    // attempt to connect to Wifi network:
    Serial.print("Connecting to Wifi SSID ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
    _client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < MAX_WIFI_RETRIES)
    {
      Serial.print(".");
      delay(100);
      retries += 1;
    }

    if (connected())
    {
      Serial.print("\nWiFi connected. IP address: ");
      Serial.println(WiFi.localIP());
      configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
      return true;
    }
    return false;
  }

  void disconnect()
  {
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
  static const int MAX_WIFI_RETRIES = 100;
};

#endif
