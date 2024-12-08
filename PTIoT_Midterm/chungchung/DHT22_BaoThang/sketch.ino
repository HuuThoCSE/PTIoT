#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include "DHTesp.h"

const char * MQTTServer = "broker.emqx.io";
const char * MQTT_Topic = "IoT/display";
const char * MQTT_Topic1 = "IoT/data";
const char * MQTT_Topic2 = "IoT/avgTemp";

#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4
#define led 4

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

WiFiClient espClient;
PubSubClient client(espClient);

// Tạo ID ngẫu nhiên tại: https://www.guidgen.com/
const char * MQTT_ID = "c5afa5ae-d3a0-48b7-b850-92015a281909";
int Port = 1883;

const int DHT_PIN = 15;
DHTesp dhtSensor;

// Khai báo để tính nhiệt độ trung bình
const int numReadings = 3;
float temperatures[numReadings];
int readIndex = 0;
float total = 0;
float average = 0;
int numValidReadings = 0;

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
      Serial.print("MQTT Topic: ");
      Serial.print(MQTT_Topic1);
      Serial.println(" connected");
      Serial.println("");
      Serial.print("MQTT Topic: ");
      Serial.print(MQTT_Topic2);
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
  String stMessage;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  WIFIConnect();
  client.setServer(MQTTServer, Port);
  client.setCallback(callback);

  lcd.init();
  lcd.backlight();

  // Print something
  lcd.setCursor(3, 0);
  lcd.print("Welcome!");

  Serial.begin(115200);

  pinMode(led, OUTPUT);

  //Khởi tạo mảng lưu nhiệt độ trung bình
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    temperatures[thisReading] = NAN;
  }

}
void loop() {
  delay(10);
  if (!client.connected()) {
    MQTT_Reconnect();
  }
   client.loop();

    // Đọc dữ liệu từ cảm biến DHT22
  float humidity = dhtSensor.getHumidity();
  float temperature = dhtSensor.getTemperature();

  // Kiểm tra dữ liệu có hợp lệ không
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  //Thực hiện tính trung bình
  // Cập nhật mảng và số lượng đọc hợp lệ
  if (!isnan(temperatures[readIndex])) {
    // Nếu giá trị cũ là hợp lệ, giảm số lượng đọc hợp lệ
    numValidReadings--;
  }

  temperatures[readIndex] = temperature;
  if (!isnan(temperature)) {
    // Nếu giá trị mới là hợp lệ, tăng số lượng đọc hợp lệ
    numValidReadings++;
  }

  readIndex = (readIndex + 1) % numReadings;

  // Tính trung bình
  float total = 0;
  for (int i = 0; i < numReadings; i++) {
    if (!isnan(temperatures[i])) {
      total += temperatures[i];
    }
  }

  float average = numValidReadings > 0 ? total / numValidReadings : NAN;

  // Gửi dữ liệu nếu có đủ giá trị hợp lệ
  if (!isnan(average)) {
    String message = String(average);
    client.publish(MQTT_Topic2, message.c_str());
  }
  //gửi dữ liệu lên mqtt
  if (!isnan(humidity) && !isnan(temperature)) {
    // Tạo chuỗi tin nhắn
    String message = String(temperature) + "|" + String(humidity);

    // Gửi tin nhắn
    client.publish(MQTT_Topic1, message.c_str());
  }

  if (temperature > 35 && humidity < 40) {
    client.publish(MQTT_Topic,"Báo động");
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
  }
  else {
    client.publish(MQTT_Topic,"Bình thường");
    digitalWrite(led,LOW);
  }
  // Hiển thị dữ liệu lên LCD
  lcd.clear(); // Xóa nội dung cũ trên LCD
  lcd.setCursor(0, 0); // Đặt con trỏ tại vị trí đầu tiên của LCD
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1); // Chuyển con trỏ xuống dòng tiếp theo
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");

  // Thêm delay để giữ dữ liệu trên màn hình một thời gian
  delay(1000); // Cập nhật dữ liệu mỗi 2 giây
}
/*
<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8" />
        <meta name="viewport" content="width=device-width, initial-scale=1.0" />
        <title>Giám sát</title>
        <script src="https://unpkg.com/mqtt/dist/mqtt.min.js"></script>
    </head>
    <body>
        <h1>Theo dõi nhiệt độ và độ ẩm</h1>
        <div id="dataDisplay">Đang chờ dữ liệu .....</div>
        <div id="averageTempDisplay">Đang chờ dữ liệu trung bình ....</div>
        <div id="alertDisplay">Đang chờ cảnh báo .....</div>


        <script>
            // Kết nối tới MQTT broker
            const client = mqtt.connect("ws://broker.emqx.io:8083/mqtt");

            client.on("connect", function () {
                console.log("Connected to MQTT broker");
                client.subscribe("IoT/display");
                client.subscribe("IoT/data");
                client.subscribe("IoT/avgTemp");
            });

            client.on("message", function (topic, message) {
                console.log("Received message:", message.toString());
                if (topic === "IoT/display") {
                    document.getElementById("alertDisplay").innerHTML =
                        "Alert: " + message.toString();
                } else if (topic === "IoT/data") {
                    const [temperature, humidity] = message
                        .toString()
                        .split("|");
                    document.getElementById("dataDisplay").innerHTML =
                        "Temperature: " +
                        temperature +
                        "°C, Humidity: " +
                        humidity +
                        "%";
                }
                else if (topic == "IoT/avgTemp"){
                    document.getElementById("averageTempDisplay").innerHTML = "Average Temp: " + message.toString() + "°C";
                }
            });
        </script>
    </body>
</html>

*/

/* Vẽ biểu đồ 
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Giám sát</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://unpkg.com/mqtt/dist/mqtt.min.js"></script>
</head>
<body>
    <h1>Theo dõi nhiệt độ và độ ẩm</h1>
    <div id="dataDisplay">Đang chờ dữ liệu...</div>
    <div id="averageTempDisplay">Đang chờ dữ liệu trung bình...</div>
    <div id="alertDisplay">Đang chờ cảnh báo...</div>
    <div style="width: 1000px; height: 200px;">
        <canvas id="temperatureChart"></canvas>
    </div>

    <script>
        const client = mqtt.connect("ws://broker.emqx.io:8083/mqtt");

        client.on("connect", function () {
            console.log("Connected to MQTT broker");
            client.subscribe("IoT/display");
            client.subscribe("IoT/data");
            client.subscribe("IoT/avgTemp");
        });

        let temperatureData = [];
        let humidityData = [];
        let chartLabels = [];
        const maxDataPoints = 50; // Giới hạn số điểm dữ liệu

        const ctx = document.getElementById('temperatureChart').getContext('2d');
        const temperatureChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: chartLabels,
                datasets: [{
                    label: 'Temperature',
                    data: temperatureData,
                    borderColor: 'red',
                    borderWidth: 1
                }, {
                    label: 'Humidity',
                    data: humidityData,
                    borderColor: 'blue',
                    borderWidth: 1
                }]
            },
            options: {
                scales: {
                    y: {
                        beginAtZero: true
                    }
                }
            }
        });

        client.on("message", function (topic, message) {
            console.log("Received message:", message.toString());
            if (topic === "IoT/display") {
                document.getElementById("alertDisplay").innerHTML =
                    "Alert: " + message.toString();
            } else if (topic === "IoT/data") {
                const [temperature, humidity] = message.toString().split("|");

                if (chartLabels.length >= maxDataPoints) {
                    chartLabels.shift();
                    temperatureData.shift();
                    humidityData.shift();
                }

                chartLabels.push(new Date().toLocaleTimeString());
                temperatureData.push(parseFloat(temperature));
                humidityData.push(parseFloat(humidity));

                temperatureChart.update();
            }
            else if (topic == "IoT/avgTemp"){
                document.getElementById("averageTempDisplay").innerHTML = "Average Temp: " + message.toString() + "°C";
            }
        });
    </script>
</body>
</html>
*/
