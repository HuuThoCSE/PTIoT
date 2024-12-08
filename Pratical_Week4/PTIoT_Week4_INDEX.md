# Bài 1: Xây dựng ứng dụng giám sát nhiệt độ/độ ẩm thông minh thông qua WIFI theo các yêu cầu sau:
✅a. ESP32 kết nối với DHT22 truy xuất thông tin nhiệt độ và độ ẩm ?
✅b. ESP32 kết nối với MQTT để gửi thông tin nhiệt độ và độ ẩm về ứng dụng WEB ?
✅c. Thiết kết giao diện WEB tĩnh để hiển thị nhiệt độ/độ ẩm theo thời gian thực ?
Tham khảo JSON .parse()  : https://www.w3schools.com/js/js_json_parse.asp
✅d. Hiển thị nhiệt độ trung bình đo 3 lần liên tiếp lên web?
✅e. Nếu độ ẩm >85% thì hiển thị thông báo độ ẩm cao?
f. Mô tả lại đường truyền dữ liệu cho ứng dụng đã xây dựng?


[https://wokwi.com/projects/414156535574217729](https://wokwi.com/projects/414156535574217729)

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN 21
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// WiFi
const char *ssid = "Wokwi-GUEST";
const char *password = ""; 
int Port = 1883;

// MQTT Broker
const char *MQTTServer = "broker.emqx.io";
const char *MQTT_Topic = "VLUTE/DHT22";

// Tạo ID ngẫu nhiên tại: https://www.guidgen.com/
const char *MQTT_ID = "91bdc853-79bb-433a-8121-98b2460e183a";

void MQTT_Reconnect() {
  while (!client.connected()) {
    if (client.connect(MQTT_ID)) {
      Serial.println("Connected to MQTT Broker!");
      client.subscribe(MQTT_Topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void WIFIConnect() {
  Serial.println("Connecting to SSID: Wokwi-GUEST");
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

void setup() {
  Serial.begin(115200);
  dht.begin();

  WIFIConnect();
  client.setServer(MQTTServer, Port);
}

void loop() {
  delay(2000);  // Cập nhật dữ liệu mỗi 2 giây

  if (!client.connected()) {
    MQTT_Reconnect();
  }
  client.loop();

  // Đọc nhiệt độ và độ ẩm từ cảm biến
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Tạo thông điệp JSON
  char message[50];
  snprintf(message, 50, "{\"temperature\": %.2f, \"humidity\": %.2f}", temperature, humidity);

  // Gửi thông điệp lên broker
  client.publish(MQTT_Topic, message);
  Serial.print("Message sent: ");
  Serial.println(message);
}
```
 
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js" type="text/javascript"></script>
    <script src="https://code.jquery.com/jquery-3.6.1.min.js" type="text/javascript"></script>
    <title>Document</title>
</head>
<body>
    <p id="nhietdo"></p>
    <p id="doam"></p>
</body>
<script>
    var client = new Paho.MQTT.Client("broker.emqx.io", Number(8083), "client2");
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    client.connect({
        onSuccess: onConnect,
        onFailure: function(e) {
            console.log("Connection failed: " + e.errorMessage);
        }
    });

    function onConnect() {
        console.log("onConnect");
        client.subscribe("VLUTE/DHT22");
    }

    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
            console.log("Connection lost: " + responseObject.errorMessage);
        }
    }

    function onMessageArrived(message) {
        console.log("onMessageArrived:" + message.payloadString);
        try {
            var data = JSON.parse(message.payloadString);
            document.getElementById("nhietdo").innerHTML = "Nhiệt độ: " + data.temperature + "°C";
            document.getElementById("doam").innerHTML = "Độ ẩm: " + data.humidity + "%";
        } catch (e) {
            console.error("Error parsing JSON:", e);
        }
    }
</script>
</html>
 ```


# NHIỆT ĐỘ TRUNG BÌNH
 
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js" type="text/javascript"></script>
    <script src="https://code.jquery.com/jquery-3.6.1.min.js" type="text/javascript"></script>
    <title>Document</title>
</head>
<body>
    <p id="nhietdo"></p>
    <p id="doam"></p>
    <p id="nhietdo_tb"></p> <!-- Thêm mục hiển thị nhiệt độ trung bình -->
</body>
<script>
    var client = new Paho.MQTT.Client("broker.emqx.io", Number(8083), "client2");
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    client.connect({
        onSuccess: onConnect,
        onFailure: function(e) {
            console.log("Connection failed: " + e.errorMessage);
        }
    });

    var temperatureData = []; // Mảng để lưu 3 lần đo nhiệt độ gần nhất

    function onConnect() {
        console.log("onConnect");
        client.subscribe("VLUTE/DHT22", {
            onSuccess: function() {
                console.log("Subscribed to topic VLUTE/DHT22 successfully.");
            },
            onFailure: function(e) {
                console.log("Subscription failed: " + e.errorMessage);
            }
        });
    }

    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
            console.log("Connection lost: " + responseObject.errorMessage);
        }
    }

    function onMessageArrived(message) {
        console.log("onMessageArrived:" + message.payloadString);
        try {
            var data = JSON.parse(message.payloadString);
            var temperature = data.temperature;
            var humidity = data.humidity;

            // Hiển thị nhiệt độ và độ ẩm hiện tại
            document.getElementById("nhietdo").innerHTML = "Nhiệt độ: " + temperature + "°C";
            document.getElementById("doam").innerHTML = "Độ ẩm: " + humidity + "%";

            // Thêm giá trị nhiệt độ mới vào mảng, giới hạn 3 phần tử
            if (temperatureData.length >= 3) {
                temperatureData.shift(); // Xóa phần tử đầu tiên nếu đã có 3 phần tử
            }
            temperatureData.push(temperature);

            // Tính nhiệt độ trung bình của 3 lần đo gần nhất nếu có đủ dữ liệu
            if (temperatureData.length === 3) {
                var avgTemperature = (temperatureData[0] + temperatureData[1] + temperatureData[2]) / 3;
                document.getElementById("nhietdo_tb").innerHTML = "Nhiệt độ trung bình: " + avgTemperature.toFixed(2) + "°C";
            }
        } catch (e) {
            console.error("Error parsing JSON:", e);
        }
    }
</script>
</html>
```
 
```cpp
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js" type="text/javascript"></script>
    <script src="https://code.jquery.com/jquery-3.6.1.min.js" type="text/javascript"></script>
    <title>Document</title>
</head>
<body>
    <p id="nhietdo"></p>
    <p id="doam"></p>
    <p id="nhietdo_tb"></p> <!-- Thêm mục hiển thị nhiệt độ trung bình -->
    <p id="thongbao"></p> <!-- Thêm mục hiển thị thông báo độ ẩm cao -->
</body>
<script>
    var client = new Paho.MQTT.Client("broker.emqx.io", Number(8083), "client2");
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    client.connect({
        onSuccess: onConnect,
        onFailure: function(e) {
            console.log("Connection failed: " + e.errorMessage);
        }
    });

    var temperatureData = []; // Mảng để lưu 3 lần đo nhiệt độ gần nhất

    function onConnect() {
        console.log("onConnect");
        client.subscribe("VLUTE/DHT22", {
            onSuccess: function() {
                console.log("Subscribed to topic VLUTE/DHT22 successfully.");
            },
            onFailure: function(e) {
                console.log("Subscription failed: " + e.errorMessage);
            }
        });
    }

    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
            console.log("Connection lost: " + responseObject.errorMessage);
        }
    }

    function onMessageArrived(message) {
        console.log("onMessageArrived:" + message.payloadString);
        try {
            var data = JSON.parse(message.payloadString);
            var temperature = data.temperature;
            var humidity = data.humidity;

            // Hiển thị nhiệt độ và độ ẩm hiện tại
            document.getElementById("nhietdo").innerHTML = "Nhiệt độ: " + temperature + "°C";
            document.getElementById("doam").innerHTML = "Độ ẩm: " + humidity + "%";

            // Thêm giá trị nhiệt độ mới vào mảng, giới hạn 3 phần tử
            if (temperatureData.length >= 3) {
                temperatureData.shift(); // Xóa phần tử đầu tiên nếu đã có 3 phần tử
            }
            temperatureData.push(temperature);

            // Tính nhiệt độ trung bình của 3 lần đo gần nhất nếu có đủ dữ liệu
            if (temperatureData.length === 3) {
                var avgTemperature = (temperatureData[0] + temperatureData[1] + temperatureData[2]) / 3;
                document.getElementById("nhietdo_tb").innerHTML = "Nhiệt độ trung bình: " + avgTemperature.toFixed(2) + "°C";
            }

            // Kiểm tra độ ẩm và hiển thị thông báo nếu độ ẩm cao
            if (humidity > 85) {
                document.getElementById("thongbao").innerHTML = "Cảnh báo: Độ ẩm cao!";
                document.getElementById("thongbao").style.color = "red";
                document.getElementById("thongbao").style.fontWeight = "bold";
            } else {
                document.getElementById("thongbao").innerHTML = ""; // Xóa thông báo nếu độ ẩm không cao
            }
        } catch (e) {
            console.error("Error parsing JSON:", e);
        }
    }
</script>
</html>
```


# Bài 2: Xây dựng ứng dụng "Bảng thông báo thông minh" theo các yêu cầu sau:
- a. ESP32 kết nối với màn hình LCD 20x4?
- b. ESP32 kết nối với MQTT nhận dữ liệu từ ứng dụng WEB chứa nội dung cần hiển thị ?
- c. ESP32 hiển thị dữ liệu nhận từ MQTT và hiển thị lên màn hình LCD, nếu dữ liệu quá dài thì tự động scroll ?
- d. Mô tả lại đường truyền dữ liệu cho ứng dụng đã xây dựng?
- [https://wokwi.com/projects/412881553471678465](https://wokwi.com/projects/412881553471678465)
 
```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

// WiFi
const char *ssid = "Wokwi-GUEST";
const char *password = ""; 

// LCD setup
LiquidCrystal_I2C lcd(0x27, 20, 4); // Địa chỉ LCD I2C là 0x27, với kích thước màn hình 20x4

// MQTT Broker
const char *MQTTServer = "broker.emqx.io";
const char *MQTT_Topic = "VLUTE/LCD";
const char *MQTT_ID = "91bdc853-79bb-433a-8121-98b2460e183a";
int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

String currentMessage = ""; // Chuỗi lưu trữ tin nhắn hiện tại để hiển thị
int scrollPosition = 0;     // Vị trí cuộn của chuỗi

void MQTT_Reconnect() {
  while (!client.connected()) {
    if (client.connect(MQTT_ID)) {
      Serial.println("Connected to MQTT Broker!");
      client.subscribe(MQTT_Topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void WIFIConnect() {
  Serial.println("Connecting to SSID: Wokwi-GUEST");
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  currentMessage = ""; // Reset current message
  for (int i = 0; i < length; i++) {
    currentMessage += (char)message[i]; // Tạo chuỗi từ tin nhắn nhận được
  }
  Serial.println(currentMessage);
  scrollPosition = 0; // Đặt lại vị trí cuộn
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // Khởi động WiFi và MQTT
  WIFIConnect();
  client.setServer(MQTTServer, Port);
  client.setCallback(callback);

  // Khởi động LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for data...");
}

void loop() {
  if (!client.connected()) {
    MQTT_Reconnect();
  }
  client.loop();

  // Hiển thị tin nhắn nhận được và cuộn nếu dài hơn 20 ký tự
  if (currentMessage.length() > 20) {
    // Cuộn từ phải sang trái
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(currentMessage.substring(scrollPosition, scrollPosition + 20));
    
    scrollPosition++;
    if (scrollPosition + 20 > currentMessage.length()) {
      scrollPosition = 0; // Reset vị trí cuộn khi đã đến cuối chuỗi
    }
    delay(300); // Độ trễ để điều chỉnh tốc độ cuộn
  } else {
    // Hiển thị trực tiếp nếu không cần cuộn
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(currentMessage);
  }
}

```

 
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ỨNG DỤNG QUẢN LÝ MÀN HÌNH THÔNG BÁO THÔNG MINH</title>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js" type="text/javascript"></script>
</head>
<body>
    <form onsubmit="sendMessage(); return false;" align="center">
        <h1 align="center">ỨNG DỤNG QUẢN LÝ MÀN HÌNH THÔNG BÁO THÔNG MINH</h1>
        <label for="message">Nhập nội dung cần hiển thị (Tiếng Việt không dấu)</label> <br>
        <textarea id="message" rows="3" style="width: 100%;"></textarea> <br>
        <input type="submit" value="Gửi">
    </form>

    <script>
        // Thiết lập kết nối MQTT
        var client = new Paho.MQTT.Client("broker.emqx.io", Number(8083), "client-web-" + Math.random());
        
        client.onConnectionLost = function(responseObject) {
            if (responseObject.errorCode !== 0) {
                console.log("Connection lost: " + responseObject.errorMessage);
            }
        };
        
        client.connect({
            onSuccess: function() {
                console.log("Connected to MQTT broker");
            },
            onFailure: function(e) {
                console.log("Connection failed: " + e.errorMessage);
            }
        });

        function sendMessage() {
            // Lấy nội dung từ textarea
            var messageContent = document.getElementById("message").value;
            if (messageContent.trim() === "") {
                alert("Vui lòng nhập nội dung cần hiển thị!");
                return;
            }

            // Tạo tin nhắn MQTT
            var message = new Paho.MQTT.Message(messageContent);
            message.destinationName = "VLUTE/LCD";

            // Gửi tin nhắn
            client.send(message);
            console.log("Message sent: " + messageContent);
            alert("Nội dung đã được gửi!");

            // Xóa nội dung trong textarea sau khi gửi
            document.getElementById("message").value = "";
        }
    </script>
</body>
</html>
```
