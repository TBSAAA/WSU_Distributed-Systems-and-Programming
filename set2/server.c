/*
 *  Haoran Huang
 *
 *  19966292
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define SERVER_TCP_PORT 1996

#define BUF_LEN 512
#define COM_BUF_LEN 32

void manage_connection(int, int);
int server_processing(char *instr, char *outstr);
void handle_sigcld(int);

int main()
{
    int connect_fd;                     /* This is the socket where the connection is established. */
    int comm_fd;                        /* This is the socket used for communication. */
    int error_code;
    socklen_t client_addr_len;          /* The size of the structure sockaddr_in (client). */
    int pid;
    struct sockaddr_in server, client;  /* address structures for server and clients */
    struct hostent *client_details;
    struct sigaction cldsig;           /* used to handle SIGCHLD to prevent zombies */

    fprintf(stderr, "THE TEST SERVER IS STARTING UP...\n");
    fprintf(stderr, "The open ports are: %d\n",SERVER_TCP_PORT);

    /* Set SIGCHLD to deal with any zombie child processes. */
    cldsig.sa_handler=handle_sigcld;
    sigfillset(&cldsig.sa_mask);
    cldsig.sa_flags=SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD,&cldsig,NULL);

    /* Create a socket for establishing a connection. */
    connect_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (connect_fd < 0)
    {
        perror("While creating connection socket");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"Test-server: connect_fd = %d\n",connect_fd);

    /* Set the server address details to bind them to the connection socket. */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SERVER_TCP_PORT);

    /* Bind the socket */
    if ((error_code = bind(connect_fd, (struct sockaddr *) &server, sizeof(server))) < 0)
    {
        perror("While calling bind()");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Test-server: Bind finished\n");

    /* Set up to 5 connections. */
    if ((error_code = listen(connect_fd, 5)) < 0)
    {
        perror("While calling listen()");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Test-server: Listening\n");
    fprintf(stderr, "Test-server: Setup complete. Ready to accept connections\n");

    /* Waiting for clients to connect. */
    while(1)
    {
        client_addr_len = sizeof(client);

        /* accept() will return a socket that can be handled by a communication socket. */
        if ((comm_fd = accept(connect_fd, (struct sockaddr *) &client, &client_addr_len)) < 0)
        {
            perror("While calling accept()");
            exit(EXIT_FAILURE);
        }

        /* Get the detail of the client. */
        client_details = gethostbyaddr((void *) &client.sin_addr.s_addr,
                                       4, AF_INET);
        if (client_details == NULL)
        {
            herror("While calling gethostbyaddr()");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "\nTest-server: Accepted connection from %s on port %d. comm_fd = %d\n",
                client_details->h_name,
                ntohs(client.sin_port),
                comm_fd);

        /* create a process to handle the client */
        if ((pid = fork()) == 0)
        {
            /* connect_fd has handed the client to comm_fd.
             * Connect_fd is no longer needed here. */
            close(connect_fd);
            manage_connection(comm_fd, comm_fd);
            /* The client has been handled so we can finish with success.*/
            exit(EXIT_SUCCESS);
        }
        else
        {
            /* Because comm_fd has been executed in child, the parent does not need it here. */
            close(comm_fd);
            fprintf(stderr, "\n            Process %d will server this.\n\n", pid);
        }
    }

    close(connect_fd);
}

void manage_connection(int in, int out)
{
    int read_count, buf_count;
    char out_buf[BUF_LEN], in_data[BUF_LEN], hostname[40];
    char prefix[100];   /* The output of which process is printed at the beginning of the message. */
    char end = '&';     /* termination char */
    int i, revcnt;
    char revbuf[BUF_LEN];

    gethostname(hostname, 40);
    /* Set the beginning of each message and ask to contain the pid of the child. */
    sprintf(prefix, "Process %d:", getpid());
    fprintf(stderr, "%s starting up\n", prefix);

    /* When the client connects, the message returned to the client. */
    sprintf(out_buf, "\nWelcome to connect to test server on host: %s\n"\
                    "Enter ’X’ as the first character to exit.\n"\
                    "Use '&' as the message termination character.\n", hostname);
    write(out, out_buf, strlen(out_buf));

    /* Keep reading until the termination char is received or the buffer is used up. */
    while(1)
    {
        buf_count = 0;
        /* Clear the buffer everytime */
        memset(in_data, 0, BUF_LEN * sizeof(char));
        while(1)
        {
            read_count = read(in, in_data + buf_count, COM_BUF_LEN);
            if (read_count > 0)
            {
                /* If it exceeds the buffer. */
                if ( (read_count+buf_count) > BUF_LEN)
                {
                    fprintf(stderr, "\n%s Receive buffer size exceeded!",prefix);
                    close(in);
                    exit(EXIT_FAILURE);
                }

                /* dump what was read */
                fprintf(stderr, "%s Have read in:\n", prefix);

                /* telnet */
                if( in_data[read_count+buf_count-2] == '\r')
                {
                    for (i = buf_count; i < buf_count + read_count - 2; i++)
                    {
                        fprintf(stderr, "%s\t%c\n", prefix, in_data[i]);
                    }
                    buf_count = buf_count + read_count - 2;
                    printf("\n");
                }

                /* client */
                else
                {
                    for (i = buf_count; i < buf_count + read_count - 1 ; i++)
                    {
                        fprintf(stderr, "%s\t%c\n", prefix, in_data[i]);
                    }
                    buf_count = buf_count + read_count - 1;
                    printf("\n");
                }

                /* Check if there is a termination char. */
                if (in_data[buf_count - 1] == end) break;
            }
            else if (read_count == 0)
            {
                fprintf(stderr, "\n%s Client has closed connection.\n", prefix);
                close(in);
                exit(EXIT_FAILURE);
            }
            else
            {
                sprintf(prefix, "%d: While calling read()", getpid());
                perror(prefix);
                close(in);
                exit(EXIT_FAILURE);
            }
        }

        /* Check close char. */
        if (in_data[0] == 'X') break;
        in_data[buf_count - 1] = '\0';
        revcnt = server_processing(in_data, revbuf);

        /* send it back with a message and next prompt */
        sprintf(out_buf, "The server receieved %d characters, all characters upcast:"\
                                 "\n%s\n\nEnter next string: ", revcnt, revbuf);

        if ( (write(out, out_buf, strlen(out_buf) ) ) < 0)
        {
            perror("While calling write()");
            exit(EXIT_FAILURE);
        }
        printf("\n\nStart new input\n");

    }

    fprintf(stderr, "\n%s Client has exited the session. Closing down\n", prefix);
    close(in);
}

/*  Upcast the characters */
int server_processing(char *in_str, char *out_str)
{
    int i, len;
    len = strlen(in_str);
    for (i = 0; i <= len; i++)
    {
        out_str[i] = toupper(in_str[i]);
    }
    out_str[len] = '\0';
    return len;
}

void handle_sigcld(int sig_num)
{
    pid_t child;

    while(0<waitpid(-1,NULL,WNOHANG));
}
