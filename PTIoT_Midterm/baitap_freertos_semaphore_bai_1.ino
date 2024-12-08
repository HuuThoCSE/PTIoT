#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#define LED1_PIN 19
#define LED2_PIN 18
#define LED3_PIN 17

SemaphoreHandle_t xSemaphore1;
SemaphoreHandle_t xSemaphore2;
SemaphoreHandle_t xSemaphore3;

void setup() {
  // Khởi tạo các chân LED
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  // Tạo Semaphore cho mỗi LED
  xSemaphore1 = xSemaphoreCreateBinary();
  xSemaphore2 = xSemaphoreCreateBinary();
  xSemaphore3 = xSemaphoreCreateBinary();

  if (xSemaphore1 != NULL) {
    xSemaphoreGive(xSemaphore1);
  }
  if (xSemaphore2 != NULL) {
    xSemaphoreGive(xSemaphore2);
  }
  if (xSemaphore3 != NULL) {
    xSemaphoreGive(xSemaphore3);
  }

  // Tạo các task để điều khiển đèn LED
  xTaskCreate(taskLed1, "Task LED 1", 1000, NULL, 1, NULL);
  xTaskCreate(taskLed2, "Task LED 2", 1000, NULL, 1, NULL);
  xTaskCreate(taskLed3, "Task LED 3", 1000, NULL, 1, NULL);
}

void loop() {
  // Không làm gì trong loop
}

void taskLed1(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (xSemaphoreTake(xSemaphore1, portMAX_DELAY) == pdTRUE) {
      digitalWrite(LED1_PIN, HIGH);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(LED1_PIN, LOW);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      xSemaphoreGive(xSemaphore2); // Phát hành Semaphore cho task tiếp theo
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
