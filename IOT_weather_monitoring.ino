#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// WiFi credentials
const char* ssid = "Hnji";
const char* password = "12345677";

// ThingSpeak settings
const char* server = "http://api.thingspeak.com/update";
String apiKey = "ZC1HHV87I07TQLID";

#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED pins
#define RED_LED    21
#define GREEN_LED  22
#define BLUE_LED   23

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT read failed!");
    return;
  }

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // LED logic
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  if (humidity > 80) {
    digitalWrite(BLUE_LED, HIGH);
  } else if (temperature > 30) {
    digitalWrite(RED_LED, HIGH);
  } else {
    digitalWrite(GREEN_LED, HIGH);
  }

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity);
    
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("Data sent to ThingSpeak.");
    } else {
      Serial.println("Failed to send data.");
    }
    http.end();
  }

  delay(15000);  // ThingSpeak accepts updates every 15 sec
}
