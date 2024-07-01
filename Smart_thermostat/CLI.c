#include "CLI.h"
#include "uart.h"
#include "json.h"

//Initialize global variables
CommandLineInterface CLI_Array[MAX_NUM_COMMANDS];
Variable variables[MAX_VARIABLES];
const char* correctUsername = "user";
const char* correctPassword = "password";
int loginStatus = 0;

//Function to register a new command and its corresponding function
void RegisterCommand(CommandLineInterface* command)
{
    static int numCommands = 0;

    // Only register specified commands
    if (numCommands < MAX_NUM_COMMANDS)
    {
        CLI_Array[numCommands] = *command;
        numCommands++;
    }
    else
    {
        fprintf(stderr, "Maximum number of commands reached\n");
    }
}

//Function to process and execute the command
void ProcessCommand(const char* inputCommand, char* arguments[])
{
    for (int i = 0; i < MAX_NUM_COMMANDS; ++i)
    {
        if (CLI_Array[i].cmdName != NULL && strcmp(inputCommand, CLI_Array[i].cmdName) == 0)
        {
            //Check if the correct number of parameters is provided
            int expectedNumParams = CLI_Array[i].expectedNumParams;
            int providedNumParams = 0;

            //calculate the provided num of parameters by user 
            while (arguments[providedNumParams] != NULL)
            {
                providedNumParams++;
            }

            //function pointer should be called only if the number of parameters matches
            //the expected one or it is s help cmd
            if ((expectedNumParams == -1) || (expectedNumParams == providedNumParams))
            {
                //Call the corresponding function using function pointer
                int result = CLI_Array[i].cmdFunction(arguments);
                /*
                if (result == 0)
                {
                    printf("Command executed successfully.\n");
                }
                else
                {
                    printf("Command did not complete successfully.\n");
                }*/

                return;
            }
            else
            {
                printf("Error: Incorrect number of parameters for '%s' command\n", inputCommand);
                return;
            }
        }
    }

    printf("Error: Unknown command: %s\n", inputCommand);
    showHelp();
}

//Function to display help information for all registered commands
void showHelp()
{
    printf("Available commands:\n");
    for (int i = 0; i < MAX_NUM_COMMANDS; ++i)
    {
        if (CLI_Array[i].cmdName != NULL)
        {
            printf("  %s: %s\n", CLI_Array[i].cmdName, CLI_Array[i].helpText);
        }
    }
}


//Function to handle user login
void userLogin()
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    //Code for printing the title of the project

    char* title = "SMART THERMOSTAT";
    int lineLength = 120;  // Length of the asterisk line
    int titleLength = strlen(title);

    // Calculate the padding needed on each side of the title
    int padding = (lineLength - titleLength) / 2;

    // Print the top asterisk line
    for (int j = 0; j < lineLength; j++) {
        printf("*");
    }
    printf("\n");

    // Print the title line with padding
    for (int j = 0; j < padding; j++) {
        printf(" ");
    }
    printf("%s", title);
    for (int j = 0; j < padding; j++) {
        printf(" ");
    }
    printf("\n");

    // Print the bottom asterisk line
    for (int i = 0; i < lineLength; i++) {
        printf("*");
    }
    printf("\n");

    //-----end of title code------


    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);

    //Remove newline character
    size_t len = strlen(username);
    if (len > 0 && username[len - 1] == '\n')
    {
        username[len - 1] = '\0';
    }

    printf("Enter your password: ");
    fgets(password, sizeof(password), stdin);

    //Remove newline character
    len = strlen(password);
    if (len > 0 && password[len - 1] == '\n')
    {
        password[len - 1] = '\0';
    }

    //Perform basic login validation
    if (strcmp(username, correctUsername) == 0 && strcmp(password, correctPassword) == 0)
    {
        printf("Login successful. Welcome, %s!\n", correctUsername);
        loginStatus = LOGGED_IN;
    }
    else
    {
        printf("Invalid login credentials. Please try again.\n");
        loginStatus = NOT_LOGGED_IN;
    }
}

int setValue(char* arguments[]) {
    if (loginStatus)
    {
        // Open the UART device
        UART* uart = uart_open("thermostat.txt");
        if (uart == NULL) {
            printf("Failed to open UART\n");
            return 1;
        }

        // Prepare the HTTP-like request
        char method[] = "POST";
        char path[128];
        snprintf(path, sizeof(path), "/set?%s=%s", arguments[0], arguments[1]);
        char host[] = "localhost";
        char body[] = "";

        // Print the HTTP-like request to the output window
        printf("%s %s HTTP/1.1\r\nHost: %s\r\n\r\n%s\r\n", method, path, host, body);

        // Write the command and its arguments to the UART
        char formatted_args[128];
        snprintf(formatted_args, sizeof(formatted_args), "{\"%s\":\"%s\"}", arguments[0], arguments[1]);
        uart_write(uart, "set", formatted_args);

        // Print a user-friendly message to the output window
        printf("\n");
        printf("********************************************\n");
        printf("Setting temperature to %s.\n", arguments[1]);
        printf("********************************************\n");
        printf("\n");

        // Close the UART
        uart_close(uart);

        return 0; //Indicate that the function has completed its job
    }
    else
    {
        printf("Please log in first.\n");
    }
    return 1; //Indicate that the function has not completed its job
}

int getValue(char* arguments[]) {
    if (loginStatus)
    {
        // Open the UART device
        UART* uart = uart_open("thermostat.txt");
        if (uart == NULL) {
            printf("Failed to open UART\n");
            return 1;
        }

        // Prepare the HTTP-like request
        char method[] = "GET";
        char path[128];
        snprintf(path, sizeof(path), "/get?%s", arguments[0]);
        char host[] = "localhost";
        char body[] = "";

        // Print the HTTP-like request to the output window
        printf("%s %s HTTP/1.1\r\nHost: %s\r\n\r\n%s\r\n", method, path, host, body);

        // Read the value from the UART
        char buffer[1024];
        uart_read(uart, buffer, sizeof(buffer) - 1);

        // Parse the JSON response
        jsmntok_t tokens[128];
        if (json_parse(buffer, tokens, sizeof(tokens) / sizeof(tokens[0])) != 0) {
            printf("Failed to parse JSON response\n");
            return 1;
        }

        // Get the value of the specified variable
        char* value = json_get_value(buffer, tokens, sizeof(tokens) / sizeof(tokens[0]), arguments[0]);
        if (value == NULL) {
            printf("Failed to get value of variable '%s'\n", arguments[0]);
            return 1;
        }

        // Print the received value
        printf("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"%s\": \"%s\"}\n", arguments[0], value);
        printf("\n");
        printf("********************************************\n");
        printf("Current temperature is %s.\n", value);
        printf("********************************************\n");
        printf("\n");
        // Free the value string
        free(value);

        // Close the UART
        uart_close(uart);

        return 0; //Indicate that the function has completed its job
    }
    else
    {
        printf("Please log in first.\n");
    }
    return 1; //Indicate that the function has not completed its job
}

//Command function for "help" command
int getHelp(char* argument[])
{
    //check if user is loggedin first
    if (loginStatus)
    {
        if (argument[0] != NULL)
        {
            //Display help for a specific command
            for (int i = 0; i < MAX_NUM_COMMANDS; ++i)
            {
                if (CLI_Array[i].cmdName != NULL && strcmp(argument[0], CLI_Array[i].cmdName) == 0)
                {
                    printf("Help for command '%s': %s\n", argument[0], CLI_Array[i].helpText);
                    return 0; //Indicate that the function has completed its job
                }
            }
            printf("Error: Unknown command: %s\n", argument[0]);
            return 1; //Indicate that the function has not completed its job
        }
        else
        {
            //Display general help for all commands
            showHelp();
            return 0; //Indicate that the function has completed its job
        }
    }
    else
    {
        printf("Please log in first.\n");
        return 1; //Indicate that the function has not completed its job
    }
}

// Function to handle the 'scheduleTemp' command
int scheduleTemperature(char* arguments[]) {
    if (loginStatus)
    {
        // Prepare the HTTP-like request
        char method[] = "POST";
        char path[128];
        snprintf(path, sizeof(path), "/scheduleTemp?temperature=%s&time=%s:%s&meridiem=%s", arguments[0], arguments[1], arguments[2], arguments[3]);
        char host[] = "localhost";
        char body[] = "";

        // Print the HTTP-like request to the output window
        printf("%s %s HTTP/1.1\r\nHost: %s\r\n\r\n%s\r\n", method, path, host, body);

        // Open the UART device
        UART* uart = uart_open("thermostat.txt");
        if (uart == NULL) {
            printf("Failed to open UART\n");
            return 1;
        }

        // Write the command and its arguments to the UART
        char formatted_args[128];
        snprintf(formatted_args, sizeof(formatted_args), "{\"temperature\": %s, \"time\": \"%s:%s\", \"meridiem\": \"%s\"}", arguments[0], arguments[1], arguments[2], arguments[3]);
        uart_write(uart, "scheduleTemp", formatted_args);
        printf("\n");
        printf("****************************************************************\n");
        printf("Temperature change scheduled for %s degrees at %s:%s %s.\n", arguments[0], arguments[1], arguments[2], arguments[3]);
        printf("****************************************************************\n");
        printf("\n");

        // Close the UART
        uart_close(uart);

        return 0; //Indicate that the function has completed its job
    }
    else
    {
        printf("Please log in first.\n");
    }
    return 1; //Indicate that the function has not completed its job
}


