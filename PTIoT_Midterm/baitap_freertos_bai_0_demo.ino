#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void TaskBlink1( void *pvParameters );
void TaskBlink2( void *pvParameters );

void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    TaskBlink1
    ,  "TaskBlink1"
    ,  1024
    ,  NULL
    ,  2
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskBlink2
    ,  "TaskBlink2"
    ,  1024
    ,  NULL
    ,  1 
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
}

void loop(){}

void TaskBlink1(void *pvParameters)
{
  (void) pvParameters;
  pinMode(19, OUTPUT);

  for (;;)
  {
    digitalWrite(19, HIGH);
    vTaskDelay(100);
    digitalWrite(19, LOW);
    vTaskDelay(100);
  }
}

void TaskBlink2(void *pvParameters)
{
  (void) pvParameters;
  pinMode(18, OUTPUT);

  for (;;)
  {
    digitalWrite(18, HIGH);
    vTaskDelay(100);
    digitalWrite(18, LOW);
    vTaskDelay(100);
  }
}