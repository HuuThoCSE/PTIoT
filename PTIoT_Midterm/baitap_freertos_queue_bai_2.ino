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

#define LED1_PIN 19
#define LED2_PIN 18
#define DHTPIN 25
#define DHTTYPE DHT22

void TaskButton(void *pvParameters);
void TaskReadTemperature(void *pvParameters);
void TaskDisplayTemperature(void *pvParameters);

volatile bool ledToggle = false;
QueueHandle_t temperatureQueue;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  lcd.init();  // Khởi tạo LCD
  lcd.backlight();

  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  dht.begin();

  temperatureQueue = xQueueCreate(5, sizeof(float));

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
      lcd.print(" C   ");
    }
  }
}
