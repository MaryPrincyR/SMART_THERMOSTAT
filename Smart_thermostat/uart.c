#include "uart.h"
#include "CLI.h"

UART* uart_open(const char* device) 
{
    UART* uart = malloc(sizeof(UART));
    if (uart == NULL) 
    {
        return NULL;
    }

    errno_t err = fopen_s(&uart->file, device, "w+");
    //errno_t err = fopen_s(&uart->file, device, "a+");
    if (err != 0 || uart->file == NULL) 
    {
        free(uart);
        return NULL;
    }

    return uart;
}

size_t uart_write(UART* uart, const char* data) 
{
    return fwrite(data, sizeof(char), strlen(data), uart->file);
}

size_t uart_read(UART* uart, char* buffer, size_t size) 
{
    return fread(buffer, sizeof(char), size, uart->file);
}

void uart_close(UART* uart) 
{
    fclose(uart->file);
    free(uart);
}
