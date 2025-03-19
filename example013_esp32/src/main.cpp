#include <Arduino.h>

static void vPeriodicTask(void *pvParameters);
static void vHandlerTask(void *pvParameters);
static void IRAM_ATTR vExampleInterruptHandler(void);
SemaphoreHandle_t xCountingSemaphore;
const uint8_t interruptPin = 2;

void setup() {
  Serial.begin(9600);
  pinMode(interruptPin, OUTPUT);
  digitalWrite(interruptPin, LOW);
  attachInterrupt(interruptPin, vExampleInterruptHandler, RISING);
  // xBinarySemaphore = xSemaphoreCreateBinary();
  xCountingSemaphore = xSemaphoreCreateCounting(10, 0);
  if( xCountingSemaphore != NULL )
  {
    xTaskCreate( vHandlerTask, "Handler", 1024, NULL, 3, NULL );
    
    xTaskCreate( vPeriodicTask, "Periodic", 1024, NULL, 1, NULL );
  }
  for (;;);
}

static void vHandlerTask(void *pvParameters) {

  xSemaphoreTake(xCountingSemaphore, 0);
  
  for (;;) {
    
    xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
    
    Serial.print( "Handler task - Processing event.\r\n" );
  }
}

static void vPeriodicTask(void *pvParameters) {
  
  for (;;) {
    
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    Serial.print("Periodic task - About to generate an interrupt.\r\n");
    digitalWrite(interruptPin, LOW);
    digitalWrite(interruptPin, HIGH);
    delay(20);
    Serial.print("Periodic task - Interrupt generated.\r\n\r\n\r\n");
  }
}

static void IRAM_ATTR vExampleInterruptHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
  xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
  xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
  xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken == pdTRUE) {
    
    portYIELD_FROM_ISR();
  }
}

void loop() {
    
}
