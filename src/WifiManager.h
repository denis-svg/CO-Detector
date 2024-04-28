#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

class WiFiManager {
public:
  WiFiManager(const X509List& cert) :
    _cert(cert) {
        // Set WiFi to station mode and disconnect from an AP if it was Previously
        // connected
        WiFi.mode(WIFI_STA);
        disconnect();
    }

  bool connect(const char* ssid, const char* password) {
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    _client.setTrustAnchors(&_cert); // Add root certificate for api.telegram.org
    Serial.print("Connecting to Wifi SSID ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < MAX_WIFI_RETRIES)
    {
        Serial.print(".");
        delay(500);
    }

    if (connected()){
        Serial.print("\nWiFi connected. IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    return false;
  }

  WiFiClientSecure& getClient() {
    return _client;
  }

  void disconnect() {
    WiFi.disconnect();
    Serial.println("WiFi disconnected.");
  }

  bool connected(){
    return WiFi.status() == WL_CONNECTED;
  }
  
  private:
    WiFiClientSecure _client;
    const X509List& _cert;
    static const int MAX_WIFI_RETRIES = 20;
};

#endif