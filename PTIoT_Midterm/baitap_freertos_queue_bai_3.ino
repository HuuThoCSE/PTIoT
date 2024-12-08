#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define LED_PIN 19
#define DHTPIN 25
#define DHTTYPE DHT22
#define LIGHT_SENSOR_PIN 34 // Chân đọc giá trị cảm biến ánh sáng
#define LIGHT_THRESHOLD 1000 // Ngưỡng ánh sáng dưới mức này sẽ bật đèn

void TaskButton(void *pvParameters);
void TaskReadTemperature(void *pvParameters);
void TaskDisplayTemperature(void *pvParameters);
void TaskLightSensor(void *pvParameters);
void TaskDisplayLightLevel(void *pvParameters);

volatile bool ledToggle = false;
QueueHandle_t temperatureQueue;
QueueHandle_t lightLevelQueue;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  lcd.init();  // Khởi tạo LCD
  lcd.backlight();

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  dht.begin();

  temperatureQueue = xQueueCreate(5, sizeof(float));
  lightLevelQueue = xQueueCreate(5, sizeof(int));

  xTaskCreatePinnedToCore(
    TaskReadTemperature, // Hàm thực thi Task
    "TaskReadTemperature", // Tên Task
    2048,                // Kích thước stack
    NULL,                // Tham số truyền vào (không có)
    1,                   // Độ ưu tiên của Task
    NULL,                // Handle của Task (không cần)
    ARDUINO_RUNNING_CORE // Chạy trên core chỉ định
  );

  xTaskCreatePinnedToCore(
    TaskDisplayTemperature, // Hàm thực thi Task
    "TaskDisplayTemperature", // Tên Task
    2048,                // Kích thước stack
    NULL,                // Tham số truyền vào (không có)
    1,                   // Độ ưu tiên của Task
    NULL,                // Handle của Task (không cần)
    ARDUINO_RUNNING_CORE // Chạy trên core chỉ định
  );

  xTaskCreatePinnedToCore(
    TaskLightSensor,     // Hàm thực thi Task
    "TaskLightSensor",  // Tên Task
    2048,                // Kích thước stack
    NULL,                // Tham số truyền vào (không có)
    1,                   // Độ ưu tiên của Task
    NULL,                // Handle của Task (không cần)
    ARDUINO_RUNNING_CORE // Chạy trên core chỉ định
  );

  xTaskCreatePinnedToCore(
    TaskDisplayLightLevel, // Hàm thực thi Task
    "TaskDisplayLightLevel", // Tên Task
    2048,                // Kích thước stack
    NULL,                // Tham số truyền vào (không có)
    1,                   // Độ ưu tiên của Task
    NULL,                // Handle của Task (không cần)
    ARDUINO_RUNNING_CORE // Chạy trên core chỉ định
  );
}

void loop() {
  // Không sử dụng loop() vì các Task đã được khởi tạo ở setup()
}

void TaskReadTemperature(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      if (xQueueSend(temperatureQueue, &temperature, 0) != pdTRUE) {
        Serial.println("Failed to send temperature to queue");
      } else {
        Serial.print("Temperature sent to queue: ");
        Serial.println(temperature);
      }
    } else {
      Serial.println("Failed to read from DHT sensor");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Đọc nhiệt độ mỗi 2 giây
  }
}

void TaskDisplayTemperature(void *pvParameters) {
  (void) pvParameters;
  float receivedTemperature;

  for (;;) {
    if (xQueueReceive(temperatureQueue, &receivedTemperature, portMAX_DELAY) == pdTRUE) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(receivedTemperature);
      lcd.print(" C");
    }
  }
}

void TaskLightSensor(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    int lightLevel = analogRead(LIGHT_SENSOR_PIN);
    Serial.print("Light level: ");
    Serial.println(lightLevel);

    if (lightLevel < LIGHT_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH);  // Bật đèn khi ánh sáng dưới ngưỡng
    } else {
      digitalWrite(LED_PIN, LOW);   // Tắt đèn khi ánh sáng trên ngưỡng
    }

    if (xQueueSend(lightLevelQueue, &lightLevel, 0) != pdTRUE) {
      Serial.println("Failed to send light level to queue");
    } else {
      Serial.print("LightLevel sent to queue: ");
      Serial.println(lightLevel);
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Kiểm tra mức ánh sáng mỗi 1 giây
  }
}

void TaskDisplayLightLevel(void *pvParameters) {
  (void) pvParameters;
  int receivedLightLevel;

  for (;;) {
    if (xQueueReceive(lightLevelQueue, &receivedLightLevel, portMAX_DELAY) == pdTRUE) {
      lcd.setCursor(0, 1);
      lcd.print("Light: ");
      lcd.print(receivedLightLevel);
    }
  }
}
