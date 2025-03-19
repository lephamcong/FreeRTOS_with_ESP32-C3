#include <Arduino.h>

static void vPeriodicTask(void *pvParameters);
static void vHandlerTask(void *pvParameters);
static void IRAM_ATTR vExampleInterruptHandler(void);
SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t xCountingSemaphore;
const uint8_t interruptPin = 2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(interruptPin, OUTPUT);
  digitalWrite(interruptPin, LOW);
  attachInterrupt(interruptPin, vExampleInterruptHandler, RISING);
  // xBinarySemaphore = xSemaphoreCreateBinary();
  xCountingSemaphore = xSemaphoreCreateCounting(3, 0);
  if( xBinarySemaphore != NULL )
  {
    /* Create the 'handler' task. This is the task that will be synchronized
    with the interrupt. The handler task is created with a high priority to
    ensure it runs immediately after the interrupt exits. In this case a
    priority of 3 is chosen. */
    xTaskCreate( vHandlerTask, "Handler", 1024, NULL, 3, NULL );
    /* Create the task that will periodically generate a software interrupt.
    This is created with a priority below the handler task to ensure it will
    get preempted each time the handler task exist the Blocked state. */
    xTaskCreate( vPeriodicTask, "Periodic", 1024, NULL, 1, NULL );
  }
  for (;;);
}

static void vHandlerTask(void *pvParameters) {
  /* Note that when you create a binary semaphore in FreeRTOS, it is ready
  to be taken, so you may want to take the semaphore after you create it
  so that the task waiting on this semaphore will block until given by
  another task. */
  xSemaphoreTake(xBinarySemaphore, 0);
  /* As per most tasks, this task is implemented within an infinite loop. */
  for (;;) {
    /* Use the semaphore to wait for the event. The semaphore was created
    before the scheduler was started so before this task ran for the first
    time. The task blocks indefinitely meaning this function call will only
    return once the semaphore has been successfully obtained - so there is no
    need to check the returned value. */
    xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
    /* To get here the event must have occurred. Process the event (in this
    case we just print out a message). */
    Serial.print( "Handler task - Processing event.\r\n" );
  }
}

static void vPeriodicTask(void *pvParameters) {
  //const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
  /* As per most tasks, this task is implemented within an infinite loop. */
  for (;;) {
    /* This task is just used to 'simulate' an interrupt. This is done by
    periodically generating a software interrupt. */
    vTaskDelay(500 / portTICK_PERIOD_MS);
    /* Generate the interrupt, printing a message both before hand and
    afterwards so the sequence of execution is evident from the output. */
    Serial.print("Periodic task - About to generate an interrupt.\r\n");
    digitalWrite(interruptPin, LOW);
    digitalWrite(interruptPin, HIGH);
    delay(20);
    Serial.print("Periodic task - Interrupt generated.\r\n\r\n\r\n");
  }
}

static void IRAM_ATTR vExampleInterruptHandler( void )
{
  static BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  /* 'Give' the semaphore multiple times. The first will unblock the handler
  task, the following 'gives' are to demonstrate that the semaphore latches
  the events to allow the handler task to process them in turn without any
  events getting lost. This simulates multiple interrupts being taken by the
  processor, even though in this case the events are simulated within a single
  interrupt occurrence.*/
  xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
  xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
  xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void loop() {
    // Empty since tasks are handled by FreeRTOS
}
