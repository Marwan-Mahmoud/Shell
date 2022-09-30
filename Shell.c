#include <string.h>
#include "Shell.h"
#include <ctype.h>

int top = 0;

void removeDoubleqQuotes(char *value);
void shiftInput(char input[]);

int startShell()
{
    char input[MAX_INPUT];
    while (1)
    {
        getInput(input);
        int numArgs = countArgs(input);

        char *command;
        char *args[numArgs + 2];

        int background = isBackground(input);
        if (background)
        {
            int n = strlen(input);
            input[n - 1] = '\0';
            input[n - 2] = '\0';
        }

        int expression = containsExpression(input);
        if (expression)
        {
            replaceExpression(input);
        }

        if (export(input))
        {
            continue;
        }

        if (echo(input))
        {
            continue;
        }

        if (cd(input))
        {
            continue;
        }

        parseInput(input, &command, args);
        execute(command, args, background);
    }
    return 0;
}

void getInput(char input[])
{
    fgets(input, MAX_INPUT, stdin);
    input[strlen(input) - 1] = '\0';
}

int countArgs(char input[])
{
    int countSpaces = 0;
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == ' ')
        {
            countSpaces++;
        }
    }
    return countSpaces;
}

int containsExpression(char input[])
{
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '$')
        {
            return 1;
        }
    }
    return 0;
}

void replaceExpression(char input[])
{
    char temp[MAX_INPUT];
    // Before $
    int i = 0;
    while (input[i] != '$')
    {
        temp[i] = input[i];
        i++;
    }
    temp[i] = '\0';

    // Variable
    char name[MAX_EXPRESSION];
    int j = i + 1, k = 0;
    while (input[j] != '\0')
    {
        if (isalpha(input[j]) == 0)
        {
            break;
        }
        name[k++] = input[j++];
    }
    name[k] = '\0';

    // Search Variable
    for (k = 0; k < MAX_VARIABLES && k < top; k++)
    {
        if (strcmp(name, variables.names[k]) == 0)
        {
            strcat(temp, variables.values[k]);
            break;
        }
    }

    // After expression
    k = strlen(temp);
    while (input[j] != '\0')
    {
        temp[k++] = input[j++];
    }
    temp[k] = '\0';

    // new input
    i = 0;
    while (temp[i] != '\0')
    {
        input[i] = temp[i];
        i++;
    }
    input[i] = '\0';
}

int export(char input[])
{
    char temp[MAX_INPUT];
    strcpy(temp, input);
    char *token = strtok(temp, " ");
    if (token != NULL && isExport(token))
    {
        char *name = strtok(NULL, "=");
        char *value = strtok(NULL, "");
        removeDoubleqQuotes(value);
        for (int i = 0; i < MAX_VARIABLES; i++)
        {
            if (i == top)
            {
                strcpy(variables.names[i], name);
                strcpy(variables.values[i], value);
                top++;
                break;
            }
            else if (strcmp(name, variables.names[i]) == 0)
            {
                strcpy(variables.values[i], value);
                break;
            }
        }
        return 1;
    }
    return 0;
}

int echo(char input[])
{
    char temp[MAX_INPUT];
    strcpy(temp, input);
    char *token = strtok(temp, " ");
    if (token != NULL && isEcho(token))
    {
        char *value = strtok(NULL, "");
        removeDoubleqQuotes(value);
        printf("%s\n", value);
        return 1;
    }
    return 0;
}

int cd(char input[])
{
    char temp[MAX_INPUT];
    strcpy(temp, input);
    char *token = strtok(temp, " ");
    if (token != NULL && isCD(token))
    {
        char *path = strtok(NULL, "");
        if (path != NULL)
        {
            char cwd[MAX_INPUT];
            if ((strcmp(path, "~") == 0))
            {
                path = getenv("HOME");
            }

            int ch = chdir(path);
            if (ch == 0)
            {
                getcwd(cwd, sizeof(cwd));
                printf("%s\n", cwd);
            }
            else
            {
                printf("Error in changing directory\n");
            }
        }
        return 1;
    }
    return 0;
}

void parseInput(char input[], char **command, char *args[])
{
    char *token = strtok(input, " ");
    if (token != NULL)
    {
        *command = token;
        args[0] = token;

        token = strtok(NULL, " ");
        int i = 1;
        while (token != NULL)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
    }
}

void execute(char *command, char *args[], int isBackground)
{
    if (isExit(command))
    {
        exit(0);
    }
    else
    {
        int pid = fork();
        if (pid == -1)
        {
            printf("Error in forking\n");
            exit(1);
        }
        else if (pid == 0)
        {
            // Child
            int err = execvp(command, args);
            if (err == -1)
            {
                printf("Command not found\n");
                exit(1);
            }
        }
        else if (!isBackground)
        {
            // Parent
            waitpid(pid, NULL, 0);
        }
    }
}

void removeDoubleqQuotes(char *value)
{
    if (value[0] == '\"')
    {
        shiftInput(value);
        value[strlen(value) - 1] = '\0';
    }
}

void shiftInput(char input[])
{
    int i = 1;
    while (input[i] != '\0')
    {
        input[i - 1] = input[i];
        i++;
    }
    input[i - 1] = '\0';
}

int isExit(char *command)
{
    return (strcmp(command, "exit") == 0);
}

int isEcho(char *command)
{
    return (strcmp(command, "echo") == 0);
}

int isCD(char *command)
{
    return (strcmp(command, "cd") == 0);
}

int isExport(char *command)
{
    return (strcmp(command, "export") == 0);
}

int isBackground(char input[])
{
    return (input[strlen(input) - 1] == '&');
}
