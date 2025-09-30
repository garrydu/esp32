#include <Arduino.h>
#include "UartScanf.h"

void readString(char *buff, int maxLength)
{
    int i = 0;
    while (i < maxLength - 1)
    {
        if (Serial.available())
        {
            char c = Serial.read();
            if (c == '\n' || c == '\r')
            {
                break;
            }
            buff[i++] = c;
        }
    }
    buff[i] = '\0'; // Null terminate the string
}

int uart_scanf(const char *format, ...)
{
    char input[1000]; // buffer should be large enough for expected input
    readString(input, sizeof(input));

    va_list args;
    va_start(args, format);
    int ret = vsscanf(input, format, args);
    va_end(args);

    return ret;
}
