#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <signal.h>

int pid = 0;

void handler(int signal_num)
{
    printf("\npaused\n");
    switch (signal_num)
    {
    case SIGTSTP:
        if (pid != 0)
        {
            kill(pid, SIGTERM);
        }
        exit(0);
    default:
        break;
    }
}

int main(int argc, char const *argv[])
{
    char input[100];
    char *args[10];
    signal(SIGTSTP, handler);

    FILE *pFile;
    int n = 0;
    char name[100];

    pFile = fopen("command.log", "w");
    if (pFile == NULL) 
    { 
        printf("Could not open file"); 
        return 0; 
    } 
    

    while (1)
    {
        // ask for command
        printf("Enter command:");
        fgets(input, sizeof(input), stdin);
        n++;
        fprintf(pFile,"%d.%s\n", n, input);

        // initialization of args everytime
        memset(args, 0, sizeof(args));

        // transform the input string
        // to array of args
        // so that execvp can use
        int argc = 0;
        int len = strlen(input);
        char *prevArg = input;
        for (int i = 0; i < len; i++)
        {
            if (input[i] == ' ')
            {
                args[argc++] = prevArg;
                prevArg = &input[i + 1];
                input[i] = '\0';
            }
            if (input[i] == '\n')
            {
                input[i] = '\0';
            }
        }
        args[argc++] = prevArg;

        // dump the info for debugging purpose
        printf("Input : %s\n", input);
        if (strcmp(input, "quit") == 0)
        {
            fclose(pFile);
            FILE* rFile = fopen("command.log", "r");
            char line[100];
            printf("Command history is:\n");
            while (1) {
				memset(line, 0, sizeof(line));
				fgets(line, sizeof(line), rFile);
				if (strlen(line) == 0) {
					break;
				}
				printf(" + %s", line);
			}
			fclose(rFile);

            break;
        }

        printf("- argc : %d\n", argc);
        printf("- args : \n");
        for (int i = 0; i <= argc; i++)
        {
            printf("  + args[%d]=%s\n", i, args[i]);
        }

        // fork() + exec() combo
        int pid = fork();
        if (pid == 0)
        {
            execvp(args[0], args);
        }
        else
        {
            waitpid(pid, NULL, 0);
        }
    }

    fclose(pFile);
    printf("Terminating.\n");
    return 0;
}