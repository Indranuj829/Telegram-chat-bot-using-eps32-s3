#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// 1. Replace with your network credentials
const char* ssid = "your_wifi_ssid";
const char* password = "yourwifipassword";

// 2. Replace with your Telegram Bot Token from BotFather
#define BOT_TOKEN "enter_the_bot_token_you_got"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Checks for new messages every 1 second
unsigned long botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// Function to process incoming messages and reply
#define LED_PIN 2 // Adjust this to your specific ESP32-S3 board's built-in LED or a custom GPIO

// Replace with your actual 9 or 10-digit Telegram Chat ID
const String AUTHORIZED_CHAT_ID = "enterthatnumber"; 

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    
    // Security Check: Ignore anyone who isn't you
    if (chat_id != AUTHORIZED_CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized access denied.", "");
      continue; 
    }

    String text = bot.messages[i].text;
    
    if (text == "/led_on") {
      digitalWrite(LED_BUILTIN, HIGH);
      bot.sendMessage(chat_id, "GPIO Pin is now HIGH (ON).", "");
    } 
    else if (text == "/led_off") {
      digitalWrite(LED_BUILTIN, LOW);
      bot.sendMessage(chat_id, "GPIO Pin is now LOW (OFF).", "");
    } 
    else if (text == "/status") {
      int pinState = digitalRead(LED_PIN);
      String statusMsg = "System operational. LED is currently " + String(pinState == HIGH ? "ON" : "OFF");
      bot.sendMessage(chat_id, statusMsg, "");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Skip SSL certificate validation for long-term reliability
  client.setInsecure();

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Check for Telegram messages at the defined interval
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
