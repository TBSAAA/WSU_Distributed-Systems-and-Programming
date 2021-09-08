/*
 *  Haoran Huang
 *
 *  19966292
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BUF_LEN 512

int main(int argc, char *argv[])
{
    int csd;                        /* client socket descriptor */
    struct sockaddr_in server;      /* server address structure */
    struct hostent *server_host;    /* pointer to server host details structure returned by resolver */
    short server_port;              /* servers port number */
    int out_count, in_count;        /* byte counts for send and receive */
    char client_send_string[BUF_LEN];       /* buffer to hold send string */
    char server_upcast_string[BUF_LEN];     /* buffer to receive upcast string */

    /* Check for correct command line usage */
    if (argc != 3)
    {
        fprintf(stderr, "Enter at least 3 parameters\n%s Server Port --> such as: %s 127.1 1996\n",
                argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Use the resolver to get the address of the server */
    server_host = gethostbyname(argv[1]);

    /* if there's a problem, report it and exit */
    if (server_host == NULL)
    {
        herror("While calling gethostbyname()");
        exit(EXIT_FAILURE);
    }

    server_port = atoi(argv[2]);

    /* Create the socket */
    csd = socket(PF_INET, SOCK_STREAM, 0);

    /* if there's a problem, report it and exit */
    if (csd < 0)
    {
        perror("While calling socket()");
        exit(EXIT_FAILURE);
    }

    /* set up the server address details in preparation for sending the message */
    server.sin_family = AF_INET;
    memcpy(&server.sin_addr.s_addr, server_host->h_addr_list[0],
           server_host->h_length);
    server.sin_port = htons(server_port);

    if ( ( connect(csd, (struct sockaddr *)&server, sizeof(server)) ) != 0)
    {
        perror("While calling connect()");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "The connection to the remote server is successful.\n");

    while (1)
    {
        /* Clear the buffer everytime */
        memset(server_upcast_string, 0, BUF_LEN * sizeof(char));

        /* Retrieve string from server */
        in_count = read(csd, server_upcast_string, BUF_LEN);

        if (in_count < 0)
        {
            perror("While calling read()");
            exit(EXIT_FAILURE);
        }
        else if (in_count == 0)
        {
            fprintf(stderr, "Server has closed connection.\n");
            break;
        }
        else if (in_count > 0)
        {
            fprintf(stderr, "Message received from the server: \n%s\n", server_upcast_string);
        }

        /* Receive user input and send it to the server. */
        while (1)
        {
            fgets(client_send_string, BUF_LEN, stdin);

            out_count = write(csd, client_send_string, strlen(client_send_string));

            if (out_count < 0)
            {
                perror("While calling write()");
                exit(EXIT_FAILURE);
            }

            /* Check whether to start sending the next string */
            if (client_send_string[strlen(client_send_string) - 2] == '&') break;
        }

        /* Check for exit */
        if (client_send_string[0] == 'X') break;
    }

    fprintf(stderr, "client exit!\n");
    close(csd);
    return 0;
}
