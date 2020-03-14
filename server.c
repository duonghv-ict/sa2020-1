#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

int main(int argc, char *argv[])
{
    int sockfd, clientfd;
    struct sockaddr_in saddr;
    unsigned short port = 9000;
    unsigned int clen;
    pthread_t input_thread;

    char client_message[2000];
    int read_size;

    // create the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket\n");
        return -1;
    }

    // bind the socket to port
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        printf("Error binding socket\n");
        close(sockfd);
        return -1;
    }

    // then listen
    listen(sockfd, 5);
    clen = sizeof(saddr);

    while (1)
    {
        // wait and accept an incoming connection
        printf("server> accepting incoming connection\n");
        clientfd = accept(sockfd, (struct sockaddr *)&saddr, &clen);

        // TODO: communicate with connected client here
        //Receive a message from client
        while ((read_size = recv(clientfd, client_message, 2000, 0)) > 0)
        {
            //Reading message from client
            printf("\nFrom client: %s\n",client_message);
            printf("To client: ");

            //Answer to client, from keyboard
            //scanf("%s", client_message);
            fgets(client_message, sizeof(client_message), stdin);
            
            //Send the message back to client
            send(clientfd, client_message, strlen(client_message),0);
        }
        if (read_size == 0)
        {
            puts("Client disconnected");
            fflush(stdout);
        }
        else if (read_size == -1)
        {
            perror("recv failed");
        }
    }
    close(clientfd);
}