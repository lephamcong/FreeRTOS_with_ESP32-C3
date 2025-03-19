#include <freertos\FreeRTOS.h>
#include <esp_freertos_hooks.h>
#include <Arduino.h>
#include <FreeRTOSConfig.h>
 /* The task function. */
void vTaskFunction( void *pvParameters );

/* A variable that is incremented by the idle task hook function. */
volatile unsigned long ulIdleCycleCount = 0UL;


/* Define the strings that will be passed in as the task parameters.  These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\t\n";

/*-----------------------------------------------------------*/

void setup( void )
{
  Serial.begin(9600);
  /* Create the first task at priority 1... */
  xTaskCreate( vTaskFunction, "Task 1", 2000, (void*)pcTextForTask1, 1, NULL );

  /* ... and the second task at priority 2.  The priority is the second to
  last parameter. */
  xTaskCreate( vTaskFunction, "Task 2", 2000, (void*)pcTextForTask2, 2, NULL );

  /* Start the scheduler so our tasks start executing. */
  // vTaskStartScheduler();

  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

void vTaskFunction( void *pvParameters )
{
char *pcTaskName;

  /* The string to print out is passed in via the parameter.  Cast this to a
  character pointer. */
  pcTaskName = ( char * ) pvParameters;

  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    /* Print out the name of this task AND the number of times ulIdleCycleCount
        has been incremented. */
    Serial.print(pcTaskName);
    Serial.println(ulIdleCycleCount );

    /* Delay for a period.  This time we use a call to vTaskDelay() which
    puts the task into the Blocked state until the delay period has expired.
    The delay period is specified in 'ticks'. */
    vTaskDelay( 250 / portTICK_PERIOD_MS );
  }
}


/*-----------------------------------------------------------*/

/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters,
and return void. */
extern "C" { 
  void __attribute__((weak)) vApplicationIdleHook(void) {
    ulIdleCycleCount++;
  }
}


//------------------------------------------------------------------------------
void loop() {}

