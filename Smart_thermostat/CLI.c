#include "CLI.h"
#include "uart.h"
#include "json.h"

//Initialize global variables
CommandLineInterface CLI_Array[MAX_NUM_COMMANDS];
Variable variables[MAX_VARIABLES];
const char* correctUsername = "Princy";
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
                if (result == 0)
                {
                    printf("Command executed successfully.\n");
                }
                else
                {
                    printf("Command did not complete successfully.\n");
                }
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


int setValue(char* arguments[])
{
    if (loginStatus)
    {
        // Open the UART device
        UART* uart = uart_open("test.txt");
        if (uart == NULL) {
            printf("Failed to open UART\n");
            return 1;
        }
        // Split the command and its arguments
        char* command = "set";
        char formatted_args[128];
        snprintf(formatted_args, sizeof(formatted_args), "{\"%s\":\"%s\"}", arguments[0], arguments[1]);

        // Send the 'SET' command and the desired value to the UART
        uart_write(uart, command, formatted_args);

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

int getValue(char* arguments[])
{
    if (loginStatus)
    {
        // Open the UART device
        UART* uart = uart_open("test.txt");
        if (uart == NULL) {
            printf("Failed to open UART\n");
            return 1;
        }

        // Split the command and its arguments
        char* command = "get";
        char formatted_arg[128];
        snprintf(formatted_arg, sizeof(formatted_arg), "%s", arguments[0]);

        // Send the 'GET' command to the UART
        uart_write(uart, command, formatted_arg);

        // Read the response from the UART
        char buffer[128];
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
        printf("Current value for %s: %s\n", arguments[0], value);

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


/*int retrieveValue(char* arguments[])
{
    // Open the UART device
    UART* uart = uart_open("test.txt");
    if (uart == NULL) {
        printf("Failed to open UART\n");
        return 1;
    }

    // Read the response from the UART
    char buffer[128];
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
    printf("Current value for %s: %s\n", arguments[0], value);

    // Free the value string
    free(value);

    // Close the UART
    uart_close(uart);

    return 0;
}*/

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
int scheduleTemperature(char* arguments[]) 
{
    // Open the UART device
    UART* uart = uart_open("test.txt");
    if (uart == NULL) {
        printf("Failed to open UART\n");
        return 1;
    }
    
    // Split the command and its arguments
    char* command = "scheduleTemp";
    char args[128];
    if (arguments[0] != NULL && arguments[1] != NULL && arguments[2] != NULL) {
        snprintf(args, sizeof(args), "%s %s:%s", arguments[0], arguments[1], arguments[2]);

        // Send the 'scheduleTemp' command, the desired temperature, and the schedule time to the UART
        uart_write(uart, command, args);
    }
    else {
        printf("Error: One or more arguments are NULL.\n");
        return 1; // Indicate that an error occurred
    }

    // Close the UART
    uart_close(uart);

    return 0;
}



