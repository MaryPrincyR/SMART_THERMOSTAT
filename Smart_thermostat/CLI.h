#define _CRT_SECURE_NO_WARNINGS
#ifndef CLI_H
#define CLI_H

#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

/*Define maximum number of commands and buffer lengths*/
#define MAX_NUM_COMMANDS 10
#define MAX_BUFFER_LENGTH 50

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
#define MAX_VARIABLES 10 /*For time being set to 10 */ 

/*Define login status constants*/
#define LOGGED_IN 1
#define NOT_LOGGED_IN 0

/*Function prototype for command functions*/
typedef int (*CmdFunction)(char* []);

/*Structure for command registration*/
typedef struct {
    const char* cmdName;     /*Name of the command*/
    CmdFunction cmdFunction; /*Function pointer for the command*/
    const char* helpText;    /*Help text for the command*/
    int expectedNumParams;  /*Expected number of parameters for the command*/
} CommandLineInterface;

/*Structure to store and retrieve variables and their values*/
typedef struct {
    char name[MAX_BUFFER_LENGTH]; /*Name of the variable*/
    int value;                    /*Value of the variable*/
} Variable;

/*Global array to store commands*/
extern CommandLineInterface CLI_Array[MAX_NUM_COMMANDS];

/*Global array to store variables*/
extern Variable variables[MAX_VARIABLES];

/*User credentials*/
extern const char* correctUsername;
extern const char* correctPassword;

/*Global flag to indicate if user is logged in or not*/
extern int loginStatus;

/*Function prototypes*/
void RegisterCommand(CommandLineInterface* command);
void showHelp();
void ProcessCommand(const char* inputCommand, char* arguments[]);
void userLogin();
int scheduleTemperature(char* arguments[]);
int setValue(char* arguments[]);
int getValue(char* arguments[]);
int getHelp(char* arguments[]);

#endif /*CLI_H*/
