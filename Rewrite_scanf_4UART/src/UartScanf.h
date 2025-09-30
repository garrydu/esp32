#ifndef UARTSCANF_H
#define UARTSCANF_H

#include <stdarg.h>

void readString(char *buff, int maxLength);

int uart_scanf(const char *format, ...);

#endif // UARTSCANF_H
