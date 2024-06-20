#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct UART UART;

struct UART 
{
    FILE* file;
};

UART* uart_open(const char* device);
size_t uart_write(UART* uart, const char* data);
size_t uart_read(UART* uart, char* buffer, size_t size);
void uart_close(UART* uart);

#endif // UART_H
