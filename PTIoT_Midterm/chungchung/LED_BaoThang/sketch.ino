#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "PubSubClient.h"

const char * MQTTServer = "broker.emqx.io";
const char * MQTT_Topic = "IoT/display";

#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

#define led 16
#define BUTTON_PIN 25

// Tạo ID ngẫu nhiên tại: https://www.guidgen.com/
const char * MQTT_ID = "c5afa5ae-d3a0-48b7-b850-92015a281909";
int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void WIFIConnect() {
  Serial.println("Connecting to SSID: Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");
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
      Serial.println(" connected");
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
  String receivedMessage;
  if (String(topic) == MQTT_Topic) {
    for (int i = 0; i < length; i++) {
      Serial.print((char)message[i]);
      receivedMessage += (char)message[i];
    }
    
  }
  Serial.println();
  if (String(topic) == MQTT_Topic) {
      if (receivedMessage == "on") {
        digitalWrite(led, HIGH); // Bật đèn
      } else if (receivedMessage == "off") {
        digitalWrite(led, LOW); // Tắt đèn
      }
    }
}

void setup() {
  // Init
  lcd.init();
  lcd.backlight();

  // Print something
  lcd.setCursor(3, 0);
  lcd.print("Welcome!");

  Serial.begin(115200);

  pinMode(led, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WIFIConnect();
  client.setServer(MQTTServer, Port);
  client.setCallback(callback);
  client.subscribe(MQTT_Topic); // Đăng ký topic để nhận dữ liệu

}

void loop() {
  delay(10);
 if (!client.connected()) {
    MQTT_Reconnect();
    client.subscribe(MQTT_Topic); // Đăng ký lại topic sau khi kết nối lại với MQTT
  }
  client.loop();

    // Xóa dòng cũ trước khi in chuỗi mới
  lcd.setCursor(0, 1); // Đặt con trỏ tại đầu dòng thứ hai của LCD
  lcd.print("                "); // In chuỗi khoảng trắng để xóa dòng

  static bool lastButtonState = HIGH; // Lưu trạng thái cuối cùng của nút bấm
  bool currentButtonState = digitalRead(BUTTON_PIN); // Đọc trạng thái hiện tại của nút bấm

  // Kiểm tra xem nút bấm có được nhấn từ trạng thái không nhấn không
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Đổi trạng thái của LED
    digitalWrite(led, !digitalRead(led)); // Đổi trạng thái LED
  }

  lastButtonState = currentButtonState; // Cập nhật trạng thái cuối cùng của nút bấm

  // Kiểm tra trạng thái của LED và cập nhật màn hình LCD
  lcd.setCursor(0, 1); // Đặt lại con trỏ tại đầu dòng
  if (digitalRead(led) == HIGH) {
    lcd.print("LED is ON ");
  } else {
    lcd.print("LED is OFF");
  }


  delay(1000);
}

/*
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Button</title>
    <script src="https://unpkg.com/mqtt/dist/mqtt.min.js"></script>
</head>
<body>
    <button type="submit" id="onBtn">On</button>
    <button type="submit" id="offBtn">Off</button>

    <script>
        // Kết nối tới MQTT broker
        const client = mqtt.connect('ws://broker.emqx.io:8083/mqtt');

        client.on('connect', function () {
            console.log('Connected to MQTT broker');
        });

        document.getElementById('onBtn').addEventListener('click', function() {
            client.publish('IoT/display', 'on');
            console.log('Sent ON message');
        });

        document.getElementById('offBtn').addEventListener('click', function() {
            client.publish('IoT/display', 'off');
            console.log('Sent OFF message');
        });
    </script>
</body>
</html>
*/