#include "Telegram.h"

TelegramBot::TelegramBot(MQ7Sensor &mq7, String BOT_TOKEN,
                         WiFiClientSecure &secured_client, void (*alert)(int),
                         void (*silence_alert)())
    : mq7_sensor(&mq7), secured_client(&secured_client),
      bot(BOT_TOKEN, secured_client), alert(alert),
      silence_alert(silence_alert) {}

void TelegramBot::tick() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    Serial.println("got response");
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

void TelegramBot::handle_message(String chat_id, String text,
                                 String from_name) {

  if (text == "/subscribe") {
    if (!(chats.find(chat_id) != chats.end())) {
      if (chats.size() > 10) {
        bot.sendMessage(
            chat_id,
            "Cannot subscribe since I am subscribed to too many chats\n",
            "Markdown");
      } else {
        chats[chat_id] =
            'T'; // the values assigned to the chat's id doesnt matter
        bot.sendMessage(
            chat_id,
            "You will now be receiving alerts on dangerous levels of CO.\n",
            "Markdown");
      }
    } else {
      bot.sendMessage(chat_id, "Already subscribed.\n", "Markdown");
    }
  }

  else if (text == "/unsubscribe") {
    if (chats.find(chat_id) != chats.end()) {
      chats.erase(chat_id);
    }
    bot.sendMessage(chat_id, "You won't be receiving alerts anymore.\n",
                    "Markdown");
  }

  else if (text == "/status") {
    String numberAsString = String(mq7_sensor->ppm);
    String message = "The ppm of CO in the room is ";
    message.concat(numberAsString);
    bot.sendMessage(chat_id, message, "Markdown");
  }

  else if (text == "/start") {
    String welcome = "Hi, " + from_name +
                     ". Here is the list of commands for the CO sensor:\n";
    welcome += "/subscribe : allow sending alerts\n";
    welcome += "/unsubscribe : stop receiving alerts\n";
    welcome += "/status : print curren CO concentration\n";
    welcome += "/alert : trigger an alert\n";
    welcome += "/silence : silence the alert sound\n";
    bot.sendMessage(chat_id, welcome, "Markdown");
  }

  else if (text == "/alert") {
    alert(100);
    bot.sendMessage(chat_id, "Demo alert triggered.\n", "Markdown");
  }

  else if (text == "/silence") {
    silence_alert();
    bot.sendMessage(chat_id, "Alert silenced.\n", "Markdown");
  }
}

void TelegramBot::handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (from_name == "")
      from_name = "Guest";
    handle_message(chat_id, text, from_name);
  }
}

void TelegramBot::handle_reading(int ppm) {
  for (const auto &pair : chats) {
    String chat_id = pair.first;
    bot.sendMessage(chat_id,
                    String("ALERT, HIGH CONCENTRATION OF CO: ") + String(ppm) +
                        String(" PPM\n"),
                    "Markdown");
  }
}
