#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

int g_pid;

void handler(int signal_num)
{
    printf("Signal %d => ", signal_num);
    switch (signal_num)
    {
    case SIGTSTP:
        printf("pause\n");
        kill(g_pid, SIGTERM);
        break;
    case SIGINT:
    case SIGTERM:
        printf("Terminated\n");
        exit(0);
        break;
    }
}

int main(void)
{
    pid_t pid;
    int ret = 1;
    int status;

    char input[100];

    // ctrl z
    signal(SIGTSTP, handler);
    // ctrl c or killed
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    //input from keyboard
    while (1)
    {
        printf("\nEnter command below, [quit] to quit\n");
        scanf("%s", input);

        if (strcmp(input, "quit") == 0)
        {
            printf("\nBye bye\n");
            return 0;
        }

        pid = fork();
        if (pid == -1)
        {
            // pid == -1 means error occured
            printf("can't fork, error occured\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {

            // pid == 0 means child process created
            // getpid() returns process id of calling process
            g_pid = getpid();
            printf("child process, pid = %u\n", getpid());

            char *argv_list[] = {input, NULL};
            execvp(input, argv_list);
            exit(0);
        }
        else
        {
            // a positive number is returned for the pid of
            // parent process
            // getppid() returns process id of parent of
            // calling process
            printf("parent process, pid = %u\n", getppid());

            // the parent process calls waitpid() on the child
            // waitpid() system call suspends execution of
            // calling process until a child specified by pid
            // argument has changed state
 
            if (waitpid(pid, &status, 0) > 0)
            {

                if (WIFEXITED(status) && !WEXITSTATUS(status))
                    printf("program execution successful\n");

                else if (WIFEXITED(status) && WEXITSTATUS(status))
                {
                    if (WEXITSTATUS(status) == 127)
                    {

                        // execvp failed
                        printf("execvp failed\n");
                    }
                    else
                        printf("program terminated normally,"
                               " but returned a non-zero status\n");
                }
                else
                    printf("program didn't terminate normally\n");
            }
            else
            {
                // waitpid() failed
                printf("waitpid() failed\n");
            }
            //exit(0);
        }
    }
    return 0;
}
