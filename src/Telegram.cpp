#include "Telegram.h"

TelegramBot::TelegramBot(MQ7Sensor& mq7, String BOT_TOKEN, WiFiClientSecure &secured_client)
    : mq7_sensor(&mq7),
      secured_client(&secured_client),
      bot(BOT_TOKEN, secured_client)
{
    // Additional initialization can be done here if needed
}

void TelegramBot::tick(){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
}

void TelegramBot::handleNewMessages(int numNewMessages){
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));
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
        if (text == "/status"){
            String numberAsString = String(mq7_sensor->ppm);
            String message = "The ppm of CO in the room is ";
            message.concat(numberAsString);
            bot.sendMessage(chat_id, message, "Markdown");
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

void TelegramBot::alert(int ppm){
    for (const auto& pair : chats) {
        String chat_id = pair.first;
        bot.sendMessage(chat_id, "ALLERT THERE IS A HIGH CONCENTRATION OF CO!!!\n", "Markdown");
    }
}