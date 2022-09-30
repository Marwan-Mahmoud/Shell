#include "Shell.h"

void handle_SIGCHILD()
{
    FILE *logFile = fopen("log file.txt", "a");
    if (logFile == NULL)
    {
        printf("Error in opening file");
        return;
    }
    fprintf(logFile, "Child process was terminated\n");
    fclose(logFile);
}

int main(int argc, char const *argv[])
{
    signal(SIGCHLD, &handle_SIGCHILD);
    int status = startShell();
    return status;
}
