#include <Arduino.h>

static void vPeriodicTask(void *pvParameters);
static void vHandlerTask(void *pvParameters);
static void IRAM_ATTR vExampleInterruptHandler(void);
SemaphoreHandle_t xBinarySemaphore;

const uint8_t interruptPin = 2;  // Pin for triggering the interrupt
const uint8_t ledPin = 10;       // LED connected to GPIO 10

int led= 0;
void setup() {
  Serial.begin(9600);

  // Setup pins
  pinMode(interruptPin, OUTPUT); // Interrupt pin
  digitalWrite(interruptPin, LOW);

  pinMode(ledPin, OUTPUT);       // LED pin
  digitalWrite(ledPin, led);     // Ensure LED is off at the start

  // Setup interrupt
  attachInterrupt(digitalPinToInterrupt(interruptPin), vExampleInterruptHandler, RISING);

  // Create a binary semaphore
  xBinarySemaphore = xSemaphoreCreateBinary();

  if (xBinarySemaphore != NULL) {
    // Create the handler task (priority 3)
    xTaskCreate(vHandlerTask, "Handler", 1024, NULL, 3, NULL);

    // Create the periodic task (priority 1)
    xTaskCreate(vPeriodicTask, "Periodic", 1024, NULL, 1, NULL);
  }

  // Start FreeRTOS scheduler (loop is empty since tasks handle execution)
}

static void vHandlerTask(void *pvParameters) {
  // Ensure semaphore is initially taken to block the handler task
  xSemaphoreTake(xBinarySemaphore, 0);

  for (;;) {
    // Wait for the semaphore (interrupt event)
    xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

    Serial.print("Handler task - Processing event.\r\n");

    // Turn on LED
    digitalWrite(ledPin, led);
    led = !led;
    
  }
}

static void vPeriodicTask(void *pvParameters) {
  for (;;) {
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    Serial.print("Periodic task - About to generate an interrupt.\r\n");
    digitalWrite(interruptPin, HIGH);  
    digitalWrite(interruptPin, LOW);
    Serial.print("Periodic task - Interrupt generated.\r\n\r\n");
  }
}

static void IRAM_ATTR vExampleInterruptHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Give the semaphore from ISR
  xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);

  if (xHigherPriorityTaskWoken == pdTRUE) {
    // Context switch if needed
    portYIELD_FROM_ISR();
  }
}

void loop() {
  // Empty - FreeRTOS tasks handle execution
}
