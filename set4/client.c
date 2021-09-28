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
#define PORT 80

int main(int argc, char *argv[])
{
    int csd;                        /* client socket descriptor */
    struct sockaddr_in server;      /* server address structure */
    struct hostent *server_host;    /* pointer to server host details structure returned by resolver */
    short server_port;              /* servers port number */
    int send_count, rec_count;        /* byte counts for send and receive */
    char client_send_string[BUF_LEN];       /* buffer to hold send string */
    char client_receive_string[BUF_LEN];    /* buffer to receive upcast string */
    char *ret;                      /* Temporarily receive string */
    int status_code;                /* status code */
    int flag = 0;                   /* Judgment symbol */
    char *s = ";";                  /* Break character */

    /* Check for correct command line usage */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s request_type IP_address\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Requesting service from %s. The default port is %d.\n", argv[2], PORT);

    /* Use the resolver to get the address of the server */
    server_host = gethostbyname(argv[2]);

    /* if there's a problem, report it and exit */
    if (server_host == NULL)
    {
        herror("While calling gethostbyname()");
        exit(EXIT_FAILURE);
    }

    server_port = PORT;

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

    /* connect to server */
    if ( ( connect(csd, (struct sockaddr *)&server, sizeof(server)) ) != 0)
    {
        perror("While calling connect()");
        close(csd);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "The connection to the remote server is successful.\n");

    /* Clear the buffer */
    memset(client_send_string, '\0', BUF_LEN * sizeof(char));

    /* Judge the user's input, and then generate the command. */
    if (!strcmp(argv[1], "-h"))
    {
        sprintf(client_send_string, "HEAD / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", argv[2]);
    }
    else if(!strcmp(argv[1], "-g"))
    {
        sprintf(client_send_string, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", argv[2]);
    }
    else
    {
        fprintf(stderr, "Bad request");
        close(csd);
        exit(EXIT_FAILURE);
    }

    /* send command to server */
    send_count = write(csd, client_send_string, strlen(client_send_string));

    if (send_count < 0)
    {
        perror("While calling write()");
        close(csd);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Send a request: \n%s", client_send_string);
    fprintf(stderr, "Message received from the server: \n\n");

    /* Receive string from server, until connection close */
    while(1){
        /* Clear the buffer */
        memset(client_receive_string, '\0', BUF_LEN * sizeof(char));

        /* receive the message */
        rec_count = read(csd, client_receive_string, BUF_LEN);
        if (rec_count > 0)
        {
            /* View status code */
            sscanf(client_receive_string, "HTTP/1.1 %d", &status_code);

            /* Determine the message sent to the server. */
            if (!strcmp(argv[1], "-h"))
            {
                /* Find the string from the message, if found,
                 * return the position of the changed character, if it fails, return NULL.*/
                if ((ret = strstr(client_receive_string, "Content-Type:")) != NULL)
                {
                    /* Break the string, and then print. */
                    strtok(ret, s);
                    fprintf(stderr, "%s;\n", ret);
                }

                if ((ret =strstr(client_receive_string, "Last-Modified:")) != NULL)
                {
                    strtok(ret, s);
                    fprintf(stderr, "%s;\n", ret);
                }
            }

            else if(!strcmp(argv[1], "-g"))
            {
                /* If the status code is 200, print the body. */
                if(status_code == 200)
                {
                    /* The initial flag is 0. If the string at the beginning
                     * of the body paragraph is found, the flag will become 1. */
                    if(flag)
                    {
                        fprintf(stdout, "%s", client_receive_string);
                    }
                    else
                    {
                        ret = strstr(client_receive_string, "<!doctype html>");
                        if(ret)
                        {
                            fprintf(stdout, "%s", ret);
                            flag = 1;
                        }
                    }
                }
                /* If the status code is not 200, print out the code. */
                else
                {
                    fprintf(stderr, "status code = %d\n", status_code);
                    fprintf(stderr, "\nClient closed connection.\n");
                    break;
                }
            }
        }

        /* After receiving the message, close the connection */
        else if (rec_count == 0)
        {
            fprintf(stderr, "\nClient closed connection.\n");
            close(csd);
            exit(EXIT_FAILURE);
        }

        else if (rec_count < 0)
        {
            perror("While calling write()");
            close(csd);
            exit(EXIT_FAILURE);
        }

    }

    return 0;
}
