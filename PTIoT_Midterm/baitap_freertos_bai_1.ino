#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

void TaskBlink1(void *pvParameters);
void TaskBlink2(void *pvParameters);

void setup() {
  lcd.init();  // Khởi tạo LCD
  lcd.backlight();

  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    TaskBlink1,          // Hàm thực thi Task
    "TaskBlink1",        // Tên Task
    4096,                // Kích thước stack
    NULL,                // Tham số truyền vào (không có)
    2,                   // Độ ưu tiên của Task
    NULL,                // Handle của Task (không cần)
    ARDUINO_RUNNING_CORE // Chạy trên core chỉ định
  );

  xTaskCreatePinnedToCore(
    TaskBlink2,
    "TaskBlink2",
    4096,
    NULL,
    1,
    NULL,
    ARDUINO_RUNNING_CORE
  );
}

void loop() {
  // Không sử dụng loop() vì các Task đã được khởi tạo ở setup()
}

void TaskBlink1(void *pvParameters) {
  (void) pvParameters;
  pinMode(19, OUTPUT);

  bool led1State = false;  // Thêm biến để lưu trạng thái LED1

  for (;;) {
    digitalWrite(19, HIGH);
    if (!led1State) {
      lcd.setCursor(0, 0);
      lcd.print("LED1: ON ");
      led1State = true;
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    digitalWrite(19, LOW);
    if (led1State) {
      lcd.setCursor(0, 0);
      lcd.print("LED1: OFF");
      led1State = false;
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }

}

void TaskBlink2(void *pvParameters) {
  (void) pvParameters;
  pinMode(18, OUTPUT);

  bool led2State = false;  // Thêm biến để lưu trạng thái LED2

  for (;;) {
    digitalWrite(18, HIGH);
    if (!led2State) {
      lcd.setCursor(0, 1);
      lcd.print("LED1: ON ");
      led2State = true;
    }
    vTaskDelay(2500 / portTICK_PERIOD_MS);

    digitalWrite(18, LOW);
    if (led2State) {
      lcd.setCursor(0, 1);
      lcd.print("LED1: OFF");
      led2State = false;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

}
