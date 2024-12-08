#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

// Task handlers
TaskHandle_t task1Handle;
TaskHandle_t task2Handle;

// Queue handler
QueueHandle_t sendingQueue;
// Task function for Task 1
void task1(void *parameter) {
  while (true) {

    int randomNumber = random(100);
    Serial.print("Random number in Task 1: ");
    Serial.println(randomNumber);

    if (xQueueSend(sendingQueue, &randomNumber, 0) != pdTRUE) {
      Serial.println("Failed to send data to queue");
    }

    vTaskDelay(500 / portTICK_PERIOD_MS); // Delay for 500 milliseconds
  }
}

// Task function for Task 2s
void task2(void *parameter) {
  int receivedNumber;
  while (true) {
    if (xQueueReceive(sendingQueue, &receivedNumber, portMAX_DELAY) == pdTRUE) {
      Serial.print("Received number in Task 2: ");
      Serial.println(receivedNumber);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Create the queue
  sendingQueue = xQueueCreate(1, sizeof(int));
  
  // Create tasks
  xTaskCreatePinnedToCore(task1, "Task 1", 2048, NULL, 1, &task1Handle, 0);
  xTaskCreatePinnedToCore(task2, "Task 2", 2048, NULL, 1, &task2Handle, 1);
}

void loop() {
  // Empty loop
}
