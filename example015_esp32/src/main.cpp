#include <Arduino.h>

/* The task to be created.  Two instances of this task are created. */
static void prvPrintTask( void *pvParameters );

/* The function that uses a mutex to control access to standard out. */
static void prvNewPrintString( const char *pcString );

/*-----------------------------------------------------------*/

/* Declare a variable of type SemaphoreHandle_t.  This is used to reference the
mutex type semaphore that is used to ensure mutual exclusive access to stdout. */
SemaphoreHandle_t xMutex;

void setup( void )
{
  Serial.begin(9600);

  /* Create a mutex type semaphore. */
  xMutex = xSemaphoreCreateMutex();

  /* Seed the random number generator. */
  srand( 567 );

  /* Check the semaphore was created successfully. */
  if( xMutex != NULL )
  {
    /* Create two instances of the tasks that attempt to write to stdout. */
    xTaskCreate( prvPrintTask, "Print1", 2000, (void*)"Task 1 ******************************************\r\n", 1, NULL );
    xTaskCreate( prvPrintTask, "Print2", 2000, (void*)"Task 2 ------------------------------------------\r\n", 2, NULL );
  }
}

/*-----------------------------------------------------------*/

static void prvNewPrintString( const char *pcString )
{
  /* Attempt to take the semaphore, blocking indefinitely if the mutex is not
  available immediately. */
  xSemaphoreTake( xMutex, portMAX_DELAY );
  {
    /* Output to the serial monitor */
    Serial.print(pcString);
    Serial.flush();
  }
  xSemaphoreGive( xMutex );
}
/*-----------------------------------------------------------*/

static void prvPrintTask( void *pvParameters )
{
  char *pcStringToPrint;

  /* Get the string to print from task parameters */
  pcStringToPrint = ( char * ) pvParameters;

  for( ;; )
  {
    /* Print out the string using the newly defined function. */
    prvNewPrintString( pcStringToPrint );

    /* Wait for a pseudo-random time */
    vTaskDelay( ( rand() & 0x1FF ) / portTICK_PERIOD_MS );
  }
}

/*-----------------------------------------------------------*/

void loop() 
{
  /* The loop function is not used as tasks are running indefinitely */
}
