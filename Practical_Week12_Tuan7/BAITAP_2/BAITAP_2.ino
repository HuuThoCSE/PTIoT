#include "DHT.h"

// Khai báo kiểu cảm biến
#define DHTPIN 5       // Chân DATA của DHT11 được nối với GPIO 4
#define DHTTYPE DHT11  // Loại cảm biến là DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200); // Bắt đầu giao tiếp Serial
  dht.begin();          // Khởi động cảm biến DHT
  Serial.println("DHT11 Sensor Example");
}

void loop() {
  // Đọc độ ẩm
  float humidity = dht.readHumidity();
  // Đọc nhiệt độ theo Celsius
  float temperature = dht.readTemperature();

  // Kiểm tra lỗi
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Lỗi khi đọc dữ liệu từ cảm biến!");
    return;
  }

  // In dữ liệu ra Serial Monitor
  Serial.print("Độ ẩm: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  Serial.print("Nhiệt độ: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Dừng 2 giây trước lần đọc tiếp theo
  delay(2000);
}
