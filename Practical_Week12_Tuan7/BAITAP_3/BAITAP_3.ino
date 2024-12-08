#include <WiFi.h>
#include "PubSubClient.h"

// Thông tin WiFi
const char* ssid = "Class 1KMT21A";
const char* password = "1KMT21A24";

const char * MQTTServer = "broker.emqx.io";
const char * MQTT_Topic = "fitvlute2008/led";

// Tạo ID ngẫu nhiên tại: https://www.guidgen.com/
const char * MQTT_ID = "eecf5f88-5a58-4c56-9277-74d5510d11fc";
int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
const int ledPin = 2;

void WIFIConnect() {
  Serial.println("Connecting to SSID:");
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected");
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

void MQTT_Reconnect() {
  while (!client.connected()) {
    if (client.connect(MQTT_ID)) {
      Serial.print("MQTT Topic: ");
      Serial.print(MQTT_Topic);
      Serial.print(" connected");
      client.subscribe(MQTT_Topic);
      Serial.println("");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  String stMessage;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();

  if (stMessage == "on") {
    digitalWrite(ledPin, HIGH);
  } else if (stMessage == "off") {
    digitalWrite(ledPin, LOW);
  } else {
    Serial.println("Unknown command");
  }
}

void setup() {
  Serial.begin(115200);

  WIFIConnect();
  client.setServer(MQTTServer, Port);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  delay(10);
  if (!client.connected()) {
    MQTT_Reconnect();
  }
  client.loop();
}