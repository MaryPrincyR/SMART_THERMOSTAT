#define _CRT_SECURE_NO_WARNINGS
#include "uart.h"
#include "CLI.h"

UART* uart_open(const char* device) 
{
    UART* uart = malloc(sizeof(UART));
    if (uart == NULL) 
    {
        return NULL;
    }

    //errno_t err = fopen_s(&uart->file, device, "a+");
    errno_t err = fopen_s(&uart->file, device, "r+");
    if (err != 0 || uart->file == NULL) 
    {
        fprintf(stderr, "Failed to open file: %s\n", strerror(err));
        free(uart);
        return NULL;
    }

    return uart;
}


void uart_write(UART* uart, const char* command, const char* argument) {
    if (uart != NULL && uart->file != NULL) {
        // Move the file position indicator to the end of the file
        fseek(uart->file, 0, SEEK_END);

        // Create a JSON string
        char json_string[128];
        snprintf(json_string, sizeof(json_string), "{\"%s\": %s}\n", command, argument);

        // Write the JSON string to the file
        fprintf(uart->file, "%s", json_string);
        fflush(uart->file);
    }
}

size_t uart_read(UART* uart, char* buffer, size_t size) {
    // Move the file position indicator to the beginning of the file
    fseek(uart->file, 0, SEEK_SET);

    size_t bytesRead = fread(buffer, sizeof(char), size, uart->file);
    buffer[bytesRead] = '\0';
    //printf("Read: '%s'\n", buffer);  // Debug print
    return bytesRead;
}

void uart_close(UART* uart) 
{
    fclose(uart->file);
    free(uart);
}
