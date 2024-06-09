#include "uart.h"

int main() {
    // Open the UART device
    UART* uart = uart_open("test.txt");
    if (uart == NULL) {
        printf("Failed to open UART\n");
        return 1;
    }

    // Write some data to the UART
    const char* message = "Hello, UART!";
    size_t bytes_written = uart_write(uart, message);
    printf("Wrote %zu bytes to UART\n", bytes_written);

    // Reset the file pointer to the beginning of the file
    rewind(uart->file);

    // Read the data back from the UART
    char buffer[128];
    size_t bytes_read = uart_read(uart, buffer, sizeof(buffer) - 1);
    if (bytes_read == 0) {
        printf("Failed to read from UART\n");
    }
    else {
        buffer[bytes_read] = '\0';
        printf("Read %zu bytes from UART: %s\n", bytes_read, buffer);
    }

    // Close the UART
    uart_close(uart);

    return 0;
}
