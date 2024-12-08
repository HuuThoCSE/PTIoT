#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

// SDA 21
// SCL 22

// Thông tin WiFi
const char* ssid = "Class 1KMT21A";
const char* password = "1KMT21A24";

// Thông tin MQTT
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttTopic = "fitvlute2008/message";
const char* mqttID = "esp32_lcd_display";

// Khởi tạo WiFi và MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Khởi tạo LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ I2C có thể khác, kiểm tra bằng I2C scanner

// Hàm kết nối WiFi
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Hàm kết nối MQTT
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(mqttID)) {
      Serial.println("Connected");
      client.subscribe(mqttTopic); // Đăng ký topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Hàm callback nhận thông điệp từ MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  String message = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();

  // Hiển thị lên LCD
  lcd.clear();
  lcd.setCursor(0, 0); // Dòng đầu tiên
  lcd.print("Topic:");
  lcd.setCursor(0, 1); // Dòng thứ hai
  lcd.print(message);
}

void setup() {
  // Khởi động Serial, WiFi, MQTT, và LCD
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  connectWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
}
