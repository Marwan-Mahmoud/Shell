#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_INPUT 256
#define MAX_VARIABLES 32
#define MAX_EXPRESSION 128

struct Variables
{
    char names[MAX_VARIABLES][MAX_EXPRESSION];
    char values[MAX_VARIABLES][MAX_EXPRESSION];
} variables;

void getInput(char input[]);
int countArgs(char input[]);
void replaceExpression(char input[]);
void parseInput(char input[], char **command, char *args[]);

int isExit(char *command);
int isExport(char *command);
int isEcho(char *command);
int isCD(char *command);
int isBackground(char input[]);
int containsExpression(char input[]);

int export(char input[]);
int echo(char input[]);
int cd(char input[]);
void execute(char *command, char *args[], int isBackground);

int startShell();

#endif
