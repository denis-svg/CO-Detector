#include <map>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Wifi network station credentials
#define WIFI_SSID "hostp"
#define WIFI_PASSWORD "12345678"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6997061084:AAFeIqDFbySFbdmjx4PhvU188GvOah4HUwc"

std::map<String, char> chats; // dictionary for storing the chat ids

const unsigned long BOT_MTBS = 10; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done
unsigned long sent_lasttime; // last time messages' scan has been done

const int ledPin = LED_BUILTIN;
int ledStatus = 0;

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;

    if (from_name == "")
      from_name = "Guest";

    if (text == "/subscribe"){
      if ( !(chats.find(chat_id) != chats.end()) ){
        if (chats.size() > 10){
           bot.sendMessage(chat_id, "Cannot subscribe since I am subscribed to too many chats\n", "Markdown");
        }
        else{
          chats[chat_id] = 'T'; // the values assigned to the chat's id doesnt matter
          bot.sendMessage(chat_id, "Succesfully subscribed to this chat.\n", "Markdown");
        }
      }else{
        bot.sendMessage(chat_id, "Already subscribed.\n", "Markdown");
      }
    }

    if (text == "/unsubscribe"){
      if ( !(chats.find(chat_id) != chats.end()) ) {
        bot.sendMessage(chat_id, "Cannot unsubscribe since I am not subsribed\n", "Markdown");
      }else{
        chats.erase(chat_id);
        bot.sendMessage(chat_id, "Succesfully unsubscribed to this chat.\n", "Markdown");
      }
    }

    if (text == "/start")
    {
      String welcome = "Welcome to MQ Sensor bot, " + from_name + ".\n";
      welcome += "This is MQ Sensor Bot example.\n\n";
      welcome += "/subscribe : to subscribe to a chat in order to receive notification ON\n";
      welcome += "/unsubscribe : to unsubscribe to a chat\n";
      welcome += "/status : to see the ppm value of CO in the room\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.println();

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); // initialize pin as off (active LOW)

  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void sendNotification(){
  for (const auto& pair : chats) {
    String chat_id = pair.first;
    bot.sendMessage(chat_id, "ALLERT THERE IS A HIGH CONCENTRATION OF CO!!!\n", "Markdown");
  }
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
  if (millis() - sent_lasttime > 2000){
    sendNotification();
    sent_lasttime = millis();
  }
}