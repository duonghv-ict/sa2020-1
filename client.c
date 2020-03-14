#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in testaddr[10];
    struct sockaddr_in saddr;
    struct hostent *h;
    int sockfd;
    unsigned int port = 9000;
    char s[100];

    char server_message[2000];
    int read_size;

    if (argc > 1)
    {
        memcpy(s, argv[1], strlen(argv[1]) + 1);
    }
    else
    {
        //Should not execute like that, don't using scanf
        printf("Enter hostname: ");
        scanf("%s", s);
    }

    // create the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        return -1;
    }

    // resolve host address
    if ((h = gethostbyname(s)) == NULL)
    {
        printf("Unknown host\n");
        close(sockfd);
        return -1;
    }

    // init sockaddr
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr = *(struct in_addr *)h->h_addr_list[0];
    saddr.sin_port = htons(port);

    // connect to server
    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        printf("Cannot connect to host\n");
        close(sockfd);
        return -1;
    }

    printf("Connected successfully!\n");

    while (1)
    {
        // TODO: add logics to communicate with server
        printf("To server: \n");

        //Send to server, from keyboard
        //scanf("%s", server_message);
        fgets(server_message, sizeof(server_message), stdin);
        printf("This server message: %s\n",server_message);

        //Send the message to server
        send(sockfd, server_message, strlen(server_message), 0);

        while ((read_size = recv(sockfd, server_message, 2000, 0)) > 0)
        {
            //Reading message from server
            printf("\nFrom server: %s\n",server_message);
            printf("To server: ");

            //Answer to server, from keyboard
            //scanf("%s", server_message);
            fgets(server_message, sizeof(server_message), stdin);
            
            //Send the message back to server
            send(sockfd, server_message, strlen(server_message),0);
        }
        if (read_size == 0)
        {
            puts("Server disconnected");
            fflush(stdout);
        }
        else if (read_size == -1)
        {
            perror("recv failed");
        }
    }
    close(sockfd);
}