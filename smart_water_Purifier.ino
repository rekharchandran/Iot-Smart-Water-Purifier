#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h> // For storing data persistently

// WiFi Credentials
const char* ssid = "******";
const char* password = "*****";

// Telegram Bot Credentials
#define BOTtoken "***************"
#define CHAT_ID "***********"
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Google Spreadsheet Web App URL
const char* serverName = "***************";

// Pins
const int touchSensorPin = 4; // Touch sensor pin
const int ledPin = 21;        // LED pin

// Variables
unsigned long lastTimeBotRan;
int botRequestDelay = 1000;
bool ledState = LOW;

float totalFilterLife;
int transactionCount;

// Preferences for persistent storage
Preferences preferences;

// Function to send Telegram Notification
void sendTelegramMessage(float dispensedVolume, float filterHealth) {
  String message = "Water transaction number: " + String(transactionCount) + "\n";
  message += "Water dispensed Now: " + String(dispensedVolume) + " ml\n";
  message += "Filter Health (%): " + String(filterHealth, 2) + "%";
  bot.sendMessage(CHAT_ID, message, "");
}

// Function to log data in Google Spreadsheet
void logToGoogleSheet(float dispensedVolume, float filterHealth) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"transaction\":" + String(transactionCount) +
                         ",\"dispensedVolume\":" + String(dispensedVolume) +
                         ",\"filterHealth\":" + String(filterHealth, 2) + "}";

    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.println("Data logged to Google Sheets");
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected. Unable to log data.");
  }
}

void dispenseWater() {
  // Start dispensing water
  digitalWrite(ledPin, HIGH);
  Serial.println("Dispensing water...");
  delay(5000); // Dispense 250 ml at 50 ml/sec
  digitalWrite(ledPin, LOW);
  Serial.println("Dispensing stopped.");

  // Update stats
  transactionCount++;
  totalFilterLife -= 250;
  float filterHealth = (totalFilterLife / 100000) * 100;

  // Save data persistently
  preferences.putInt("transactionCount", transactionCount);
  preferences.putFloat("totalFilterLife", totalFilterLife);

  // Notify and Log
  sendTelegramMessage(250, filterHealth);
  logToGoogleSheet(250, filterHealth);
}

void setup() {
  Serial.begin(115200);
  pinMode(touchSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  // Load preferences
  preferences.begin("waterData", false);
  transactionCount = preferences.getInt("transactionCount", 0);
  totalFilterLife = preferences.getFloat("totalFilterLife", 100000.0);
  Serial.println("Persistent data loaded.");
}

void loop() {
  // Handle touch input
  if (digitalRead(touchSensorPin) == HIGH) {
    dispenseWater();
    delay(1000); // Debounce delay
  }

  // Check for Telegram updates
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int NewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (NewMessages) {
      for (int i = 0; i < NewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id == CHAT_ID) {
          String text = bot.messages[i].text;
          if (text == "/status") {
            String statusMessage = "Filter Health: " + String((totalFilterLife / 100000) * 100, 2) + "%";
            bot.sendMessage(chat_id, statusMessage, "");
          }
        }
      }
    }
    lastTimeBotRan = millis();
  }
}

