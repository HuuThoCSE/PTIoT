#include <WiFi.h>
#include "PubSubClient.h"

const char * MQTTServer = "broker.emqx.io";
const char* leds = "IoT/leds";

// Tạo ID ngẫu nhiên tại: https://www.guidgen.com/
const char * MQTT_ID = "8748c87e-736d-4327-9e75-a52a0e497dd8";
int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPins[] = {5,17,16};
unsigned long timeLed[] = {0, 0, 0};
unsigned long nextLedChange = 0; // Thời điểm tiếp theo cần thay đổi đèn
int ledIndex = 0; // Chỉ số của đèn hiện tại


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
      Serial.print(leds);
      Serial.println(" connected");
      client.subscribe(leds); 
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

  char strMessage[length + 1];
  for (int i = 0; i < length; i++) {
    strMessage[i] = (char)message[i];
  }
  strMessage[length] = '\0'; 


  String strPayload = String(strMessage);
  Serial.println(strPayload);


  int firstSemiColon = strPayload.indexOf(';');
  int secondSemiColon = strPayload.indexOf(';', firstSemiColon + 1);

 
  // Cập nhật thời gian hoạt động cho mỗi đèn
  timeLed[0] = strPayload.substring(0, firstSemiColon).toInt();
  timeLed[1] = strPayload.substring(firstSemiColon + 1, secondSemiColon).toInt();
  timeLed[2] = strPayload.substring(secondSemiColon + 1).toInt();


  // Send back a confirmation message to the web application
  String confirmation = String("LED1: ") + timeLed[0] + "s, LED2: " + timeLed[1] + "s, LED3: " + timeLed[2] + "s";
  client.publish(leds, confirmation.c_str());
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WIFIConnect();
  client.setServer(MQTTServer, Port);
  client.setCallback(callback);

  for (int i = 0; i < 3; i++) {
      pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  if (!client.connected()) {
    MQTT_Reconnect();
  }
  client.loop();

  // Kiểm tra nếu có thời gian cho đèn được cập nhật
  if (timeLed[0] > 0 || timeLed[1] > 0 || timeLed[2] > 0) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(ledPins[i], HIGH); // Bật đèn hiện tại
      delay(timeLed[i] * 1000);       // Chờ thời gian cài đặt cho đèn hiện tại
      digitalWrite(ledPins[i], LOW);  // Tắt đèn hiện tại

      // Reset thời gian cho đèn sau khi đã sử dụng
      timeLed[i] = 0;
    }
  }
}

/*
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Led</title>
</head>
<body>
    <div class="container">
        <div class="group">
            <h2>Thời gian đèn xanh bật (đơn vị giây)</h2>
            <input type="number" id="led1">
        </div>
    
        <div class="group">
            <h2>Thời gian đèn vàng bật (đơn vị giây)</h2>
            <input type="number" id="led2">
        </div>
    
        <div class="group">
            <h2>Thời gian đèn đỏ bật (đơn vị giây)</h2>
            <input type="number" id="led3">
        </div>

        <button type="submit" id="btnLed" style="background-color: red; color: white; width:100px; margin-top:50px;">Lưu</button>

        <div id="statusMessage" style="margin-top: 50px ;"></div>
    </div>
    <script src="https://unpkg.com/mqtt/dist/mqtt.min.js"></script>
    <script>
        const client = mqtt.connect('ws://broker.emqx.io:8083/mqtt');

        client.on('connect', function() {
            console.log('Connected to MQTT broker');
        });

        document.getElementById('btnLed').addEventListener('click', function() {
            let led1 = document.getElementById('led1').value || 0;
            let led2 = document.getElementById('led2').value || 0;
            let led3 = document.getElementById('led3').value || 0;

            let mess = `${led1};${led2};${led3}`;
            client.publish('IoT/leds',mess);

            document.getElementById('statusMessage').innerHTML = 'Thao tác thành công';
        });
    </script>
</body>
</html>
*/