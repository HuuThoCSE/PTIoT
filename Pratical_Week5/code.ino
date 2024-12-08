#include "WiFi.h"
#include "PubSubClient.h"
#include "ArduinoJson.h"

int led_green = 2;
int led_red = 4;
int led_yellow = 5;

// WiFi
const char *ssid = "Wokwi-GUEST";
const char *password = ""; 

// MQTT Broker
const char *MQTTServer = "broker.emqx.io";
const char *MQTT_Topic = "VLUTE/WEEK5";
const char *MQTT_ID = "91bdc853-79bb-433a-8121-98b2460e183a";
int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Biến lưu trữ thời gian bật đèn LED
int timeGreen = 1;
int timeYellow = 2;
int timeRed = 3;

// Hàm kết nối lại với MQTT Broker
void MQTT_Reconnect() {
  while (!client.connected()) {
    if (client.connect(MQTT_ID)) {
      Serial.println("Connected to MQTT Broker!");
      client.subscribe(MQTT_Topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  }
}

// Hàm kết nối WiFi
void WIFIConnect() {
  Serial.println("Connecting to SSID: Wokwi-GUEST");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected");
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback MQTT
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Tạo một chuỗi từ tin nhắn nhận được
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  Serial.print("Message: ");
  Serial.println(messageTemp);

  // Phân tích chuỗi JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, messageTemp);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  // Lấy giá trị thời gian từ JSON
  timeGreen = doc["timeGreen"];
  timeYellow = doc["timeYellow"];
  timeRed = doc["timeRed"];

  Serial.printf("TimeGreen: %d, TimeYellow: %d, TimeRed: %d\n", timeGreen, timeYellow, timeRed);
}

// Task xử lý MQTT
void MQTTTask(void *pvParameters) {
  while (true) {
    if (!client.connected()) {
      MQTT_Reconnect();
    }
    client.loop();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Giảm tải CPU
  }
}

// Task điều khiển LED
void LEDTask(void *pvParameters) {
  while (true) {
    // Bật tắt đèn LED theo thời gian nhận được
    digitalWrite(led_green, HIGH);
    vTaskDelay(timeGreen * 1000 / portTICK_PERIOD_MS);
    digitalWrite(led_green, LOW);

    digitalWrite(led_yellow, HIGH);
    vTaskDelay(timeYellow * 1000 / portTICK_PERIOD_MS);
    digitalWrite(led_yellow, LOW);

    digitalWrite(led_red, HIGH);
    vTaskDelay(timeRed * 1000 / portTICK_PERIOD_MS);
    digitalWrite(led_red, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_yellow, OUTPUT);

  // Khởi động WiFi và MQTT
  WIFIConnect();
  client.setServer(MQTTServer, Port);
  client.setCallback(callback);

  // Tạo các Task
  xTaskCreate(MQTTTask, "MQTT Task", 4096, NULL, 1, NULL);
  xTaskCreate(LEDTask, "LED Task", 2048, NULL, 1, NULL);
}

void loop() {
  // Loop không cần làm gì vì đã sử dụng RTOS cho các task.
}
