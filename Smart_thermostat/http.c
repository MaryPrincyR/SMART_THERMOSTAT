#define _CRT_SECURE_NO_WARNINGS
#include "uart.h"
#include "CLI.h"

// Function to write an HTTP-like request to a UART device
void uart_write_http_request(UART* uart, const char* method, const char* path, const char* host, const char* body) {
    if (uart != NULL && uart->file != NULL) {
        // Move the file position indicator to the end of the file
        fseek(uart->file, 0, SEEK_END);

        // Create an HTTP-like string
        char http_string[128];
        snprintf(http_string, sizeof(http_string), "%s %s HTTP/1.1\r\nHost: %s\r\n\r\n%s\r\n", method, path, host, body);

        // Write the HTTP-like string to the file
        fprintf(uart->file, "%s", http_string);
        fflush(uart->file);
    }
}

// Function to read an HTTP-like response from a UART device
void uart_read_http_response(UART* uart, char* buffer, size_t size) {
    // Move the file position indicator to the beginning of the file
    fseek(uart->file, 0, SEEK_SET);

    // Read the JSON data from the file
    size_t bytesRead = fread(buffer, sizeof(char), size, uart->file);
    buffer[bytesRead] = '\0';

    // Prepare the HTTP-like response
    char status_line[] = "HTTP/1.1 200 OK";
    char headers[] = "Content-Type: application/json";
    char body[1024];
    strncpy(body, buffer, sizeof(body));

    // Print the HTTP-like response to the output window
    printf("%s\r\n%s\r\n\r\n%s\r\n", status_line, headers, body);
}


// Function to send an HTTP request over UART
int sendHttpRequest(char* arguments[]) {
    // Open the UART device
    UART* uart = uart_open("test.txt");
    if (uart == NULL) {
        printf("Failed to open UART\n");
        return 1;
    }

    // Send an HTTP request
    uart_write_http_request(uart, arguments[0], arguments[1], arguments[2], arguments[3]);

    // Close the UART device
    uart_close(uart);

    return 0; // Indicate that the function has completed its job
}
