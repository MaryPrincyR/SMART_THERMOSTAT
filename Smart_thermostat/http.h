#ifndef HTTP_H
#define HTTP_H

#include "uart.h"

// Function to write an HTTP-like request to a UART device
void uart_write_http_request(UART* uart, const char* method, const char* path, const char* host, const char* body);

// Function to read an HTTP-like response from a UART device
void uart_read_http_response(UART* uart, char* buffer, size_t size);

// Function to send an HTTP request over UART
int sendHttpRequest(char* arguments[]);

#endif // HTTP_H

