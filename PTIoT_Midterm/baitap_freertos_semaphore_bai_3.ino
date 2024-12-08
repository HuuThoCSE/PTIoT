#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#define LED1_PIN 19
#define LED2_PIN 18
#define LED3_PIN 17
#define BUTTON1_PIN 34
#define BUTTON2_PIN 35

SemaphoreHandle_t xSemaphore1;
SemaphoreHandle_t xSemaphore2;
SemaphoreHandle_t xSemaphore3;
TaskHandle_t xTaskHandle1;
TaskHandle_t xTaskHandle2;
TaskHandle_t xTaskHandle3;

void setup() {
  // Khởi tạo các chân LED
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  // Khởi tạo các chân nút nhấn
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  // Tạo Semaphore cho mỗi LED
  xSemaphore1 = xSemaphoreCreateBinary();
  xSemaphore2 = xSemaphoreCreateBinary();
  xSemaphore3 = xSemaphoreCreateBinary();

  if (xSemaphore1 != NULL) {
    xSemaphoreGive(xSemaphore1);
  }

  // Tạo các task để điều khiển đèn LED
  xTaskCreate(taskLed1, "Task LED 1", 1000, NULL, 3, &xTaskHandle1);
  xTaskCreate(taskLed2, "Task LED 2", 1000, NULL, 2, &xTaskHandle2);
  xTaskCreate(taskLed3, "Task LED 3", 1000, NULL, 1, &xTaskHandle3);
  xTaskCreate(taskButtonControl, "Task Button Control", 1000, NULL, 4, NULL);
}

void loop() {
  // Không làm gì trong loop
}

void taskLed1(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (xSemaphoreTake(xSemaphore1, portMAX_DELAY) == pdTRUE) {
      // Tăng ưu tiên cho task LED 1
      vTaskPrioritySet(xTaskHandle1, 4);

      digitalWrite(LED1_PIN, HIGH);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(LED1_PIN, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS);

      // Phát hành Semaphore cho task tiếp theo
      xSemaphoreGive(xSemaphore2);

      // Giảm ưu tiên trở lại sau khi hoàn thành
      vTaskPrioritySet(xTaskHandle1, 3);
    }
  }
}

void taskLed2(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (xSemaphoreTake(xSemaphore2, portMAX_DELAY) == pdTRUE) {
      digitalWrite(LED2_PIN, HIGH);
      vTaskDelay(300 / portTICK_PERIOD_MS);
      digitalWrite(LED2_PIN, LOW);
      vTaskDelay(300 / portTICK_PERIOD_MS);
      xSemaphoreGive(xSemaphore3); // Phát hành Semaphore cho task tiếp theo
    }
  }
}

void taskLed3(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (xSemaphoreTake(xSemaphore3, portMAX_DELAY) == pdTRUE) {
      digitalWrite(LED3_PIN, HIGH);
      vTaskDelay(700 / portTICK_PERIOD_MS);
      digitalWrite(LED3_PIN, LOW);
      vTaskDelay(700 / portTICK_PERIOD_MS);
      xSemaphoreGive(xSemaphore1); // Phát hành Semaphore cho task tiếp theo
    }
  }
}

void taskButtonControl(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    // Kiểm tra nút 1
    if (digitalRead(BUTTON1_PIN) == LOW) {
      // Tạm dừng các task LED 1 và LED 2
      vTaskSuspend(xTaskHandle1);
      vTaskSuspend(xTaskHandle2);
    }
    // Kiểm tra nút 2
    if (digitalRead(BUTTON2_PIN) == LOW) {
      // Tiếp tục các task LED 1 và LED 2
      vTaskResume(xTaskHandle1);
      vTaskResume(xTaskHandle2);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Tránh lặp quá nhanh
  }
}
