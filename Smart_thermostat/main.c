#include "CLI.h"
#include "uart.h" 

int main()
{
    /*Registering specified commands*/

    CommandLineInterface helpCommand = { "help", getHelp, "Display help information for commands(Format:help <command name> // or help )", -1 };
    RegisterCommand(&helpCommand);

    CommandLineInterface setCommand = { "set", setValue, "Set temperature value (Format:set temp <value>)", 2 };
    RegisterCommand(&setCommand);

    CommandLineInterface getCommand = { "get", getValue, "Get current temperature value (Format:get temp)", 1 };
    RegisterCommand(&getCommand);

    CommandLineInterface scheduleTempCmd = { "scheduleTemp", scheduleTemperature, "Schedule a temperature change. Format: scheduleTemp <temperature> <hour> <minute> <meridiem indicator>", 4 };
    RegisterCommand(&scheduleTempCmd);

    /*Command-line input processing*/
    char userInputBuffer[100];

    while (1)
    {
        if (loginStatus != NOT_LOGGED_IN)
        {
            printf("\nEnter command or enter Ctrl^Z to exit : ");
            if (fgets(userInputBuffer, sizeof(userInputBuffer), stdin) == NULL)
            {
                break; /*Exit on EOF*/
            }

            /*Remove newline character*/
            size_t len = strlen(userInputBuffer);
            if (len > 0 && userInputBuffer[len - 1] == '\n')
            {
                userInputBuffer[len - 1] = '\0';
            }

            /*Split command and argument*/
            char* command = strtok(userInputBuffer, " ");
            char* arguments[10];
            int argIndex = 0;

            /*add the parameters in the arguments array*/
            while ((arguments[argIndex] = strtok(NULL, " ")) != NULL)
            {
                argIndex++;
            }

            /*if command is not NULL then process the command*/
            if (command != NULL)
            {
                ProcessCommand(command, arguments);
            }
            else
            {
                printf("Error: Empty command\n");
            }
        }
        else
        {
            userLogin();
        }

    }
    return 0;
}
