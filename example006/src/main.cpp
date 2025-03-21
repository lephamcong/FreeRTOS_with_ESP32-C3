#include <Arduino.h>

/* The task functions. */
void vContinuousProcessingTask( void *pvParameters );
void vPeriodicTask( void *pvParameters );

/* Define the strings that will be passed in as the task parameters.  These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Continuous task 1 running\r\n";
const char *pcTextForTask2 = "Continuous task 2 running\r\n";
const char *pcTextForPeriodicTask = "Periodic task is running\r\n";

/*-----------------------------------------------------------*/

void setup( void )
{
  Serial.begin(9600);
  /* Create two instances of the continuous processing task, both at priority 1. */
  // xTaskCreatePinnedToCore( vContinuousProcessingTask, "Task 1", 2000, (void*)pcTextForTask1, 1, NULL,1 );
  // xTaskCreatePinnedToCore( vContinuousProcessingTask, "Task 2", 2000, (void*)pcTextForTask2, 1, NULL,1 );
  xTaskCreate( vContinuousProcessingTask, "Task 1", 2000, (void*)pcTextForTask1, 1, NULL );
  xTaskCreate( vContinuousProcessingTask, "Task 2", 2000, (void*)pcTextForTask2, 1, NULL );

  /* Create one instance of the periodic task at priority 2. */
  // xTaskCreatePinnedToCore( vPeriodicTask, "Task 3", 2000, (void*)pcTextForPeriodicTask, 2, NULL,1 );
  xTaskCreate( vPeriodicTask, "Task 3", 2000, (void*)pcTextForPeriodicTask, 2, NULL );

  /* Start the scheduler so our tasks start executing. */
  // vTaskStartScheduler();

  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

void vContinuousProcessingTask( void *pvParameters )
{
char *pcTaskName;

  /* The string to print out is passed in via the parameter.  Cast this to a
  character pointer. */
  pcTaskName = ( char * ) pvParameters;

  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    /* Print out the name of this task.  This task just does this repeatedly
    without ever blocking or delaying. */
    Serial.print( pcTaskName );
    delay(100);

  }
}
/*-----------------------------------------------------------*/

void vPeriodicTask( void *pvParameters )
{
TickType_t xLastWakeTime;
char *pcTaskName;

  /* The xLastWakeTime variable needs to be initialized with the current tick
  count.  Note that this is the only time we access this variable.  From this
  point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
  API function. */
  xLastWakeTime = xTaskGetTickCount();
  pcTaskName = ( char * ) pvParameters;
  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    /* Print out the name of this task. */
    Serial.print( pcTaskName );

    /* We want this task to execute exactly every 500 milliseconds. */
    vTaskDelayUntil( &xLastWakeTime, (500 / portTICK_PERIOD_MS ) );
  }
}
//------------------------------------------------------------------------------
void loop() {}
