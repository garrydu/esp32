#include <Arduino.h>
#include "UartScanf.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  printf("Enter your name and age (example: Bob 25):\r\n");
  /*
  The reason printf can be used directly on platforms like Arduino
  or ESP32 is that the standard output (stdout) used by printf is
  typically retargeted or redirected to the UART serial interface
  by system-level hooks or libraries.
  */
}

void loop()
{
  char name[30];
  int age;

  printf("Enter name and age: \n");

  int count = uart_scanf("%29s %d", name, &age);

  if (count == 2)
  {
    printf("Hello %s, you are %d years old.\r\n\n", name, age);
  }
  else
  {
    printf("Input error, please enter again.\r\n\n");
  }

  delay(2000);
}
