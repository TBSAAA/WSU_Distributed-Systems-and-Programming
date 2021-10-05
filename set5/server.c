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
#include <time.h>


#define SERVER_TCP_PORT 1996

#define BUF_LEN 512


void manage_connection(int, int);
char *server_processing(char *instr);
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
    int read_count;
    char in_data[BUF_LEN], hostname[40];
    char *out_buf;
    char prefix[100];   /* The output of which process is printed at the beginning of the message. */

    gethostname(hostname, 40);
    /* Set the beginning of each message and ask to contain the pid of the child. */
    sprintf(prefix, "Process %d:", getpid());
    fprintf(stderr, "%s starting up\n", prefix);


    /* Clear the buffer everytime */
    memset(in_data, 0, BUF_LEN * sizeof(char));
    read_count = read(in, in_data, BUF_LEN);
    if (read_count > 0)
    {
        /* If it exceeds the buffer. */
        if (read_count > BUF_LEN)
        {
            fprintf(stderr, "\n%s Receive buffer size exceeded!", prefix);
            close(in);
            exit(EXIT_FAILURE);
        }
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

    /* Process the received information. */
    out_buf = server_processing(in_data);
    /* Write data back to the client. */
    if ( (write(out, out_buf, strlen(out_buf) ) ) < 0)
    {
        perror("While calling write()");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n%s Client has exited the session. Closing down\n", prefix);
    close(in);
}

/*  Upcast the characters */
char * server_processing(char *receive)
{
    char *return_buf = (char *)malloc(BUF_LEN), body_buf[BUF_LEN] , tem_buf[40];
    int i = 0;
    char *request;
    char *location;
    char *vision;
    char *token;                    /* Temporarily receive string */
    char *s = " ", *b = "\r\n";     /* Break character */
    int random_num;
    char lyrics[] = "Hickory Dickory Dock, \r\n"
                    "The mouse ran up the clock. \r\n"
                    "The clock struck one, \r\n"
                    "The mouse ran down! \r\n"
                    "Hickory Dickory Dock. \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The bird looked at the clock, \r\n"
                    "The clock struck two 2, \r\n"
                    "Away she flew, \r\n"
                    "Hickory Dickory Dock \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The dog barked at the clock, \r\n"
                    "The clock struck three 3, \r\n"
                    "Fiddle-de-dee, \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The bear slept by the clock, \r\n"
                    "The clock struck four 4, \r\n"
                    "He ran out the door, \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The bee buzzed round the clock, \r\n"
                    "The clock struck five 5, \r\n"
                    "She went to her hive, \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The hen pecked at the clock, \r\n"
                    "The clock struck six 6, \r\n"
                    "Oh, fiddle-sticks, \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The cat ran round the clock, \r\n"
                    "The clock struck seven 7, \r\n"
                    "She wanted to get 'em, \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The horse jumped over the clock, \r\n"
                    "The clock struck eight 8, \r\n"
                    "He ate some cake, \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The cow danced on the clock, \r\n"
                    "The clock struck nine 9, \r\n"
                    "She felt so fine, \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The pig oinked at the clock, \r\n"
                    "The clock struck ten 10, \r\n"
                    "She did it again, \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The duck quacked at the clock \r\n"
                    "The clock struck eleven 11, \r\n"
                    "The duck said 'oh heavens!' \r\n"
                    "Hickory Dickory Dock! \r\n"
                    "Hickory Dickory Dock, \r\n"
                    "The mouse ran up the clock \r\n"
                    "The clock struck noon \r\n"
                    "He's here too soon! \r\n"
                    "Hickory Dickory Dock!";

    /* get the request */
    token = strtok(receive, s);
    request = (char *)malloc(sizeof(char) * strlen(token));
    strcpy(request, token);
    while(1)
    {
        i++;
        token = strtok(NULL, s);
        if(token == NULL) break;
        if(i == 1)
        {
            /* get the location */
            location = (char *)malloc(sizeof(char) * strlen(token));
            strcpy(location, token);
        }
        if(i == 2)
        {
            /* get the vision */
            vision = (char *)malloc(sizeof(char) * strlen(token));
            strcpy(vision, token);
        }
        if(i == 3)
        {
            /* At most 3 parameters are accepted. */
            strcpy(return_buf, "Can't accept more requests.\r\n");
            return return_buf;
        }
    }

    srand((unsigned) time(NULL));

    /* Identify each parameter. */
    if (strcmp(request, "HEAD") == 0 || strcmp(request, "GET") == 0)
    {
        if (strcmp(location, "/") == 0 || strcmp(location, "/\r\n") == 0)
        {
            if (strcmp(vision, "HTTP/1.1\r\n") == 0)
            {
                if (strncmp(request, "GET", 3) == 0)
                {
                    /* body */
                    random_num = 1 + rand() % 60;
                    /* Interrupt with \r\n */
                    token = strtok(lyrics, b);
                    while (random_num > 0)
                    {
                        strcat(body_buf, token);
                        strcat(body_buf, b);
                        random_num--;
                        token = strtok(NULL, b);
                    }
                    strcpy(return_buf, "HTTP/1.1 200 OK\r\n" \
                                        "Content-Type: text/plain\r\n");
                    sprintf(tem_buf, "Content-Length: %d\r\n",(int)strlen(body_buf));
                    strcat(return_buf, tem_buf);
                    strcat(return_buf, body_buf);
                }
                else
                {
                    strcpy(return_buf, "HTTP/1.1 200 OK\r\n" \
                                       "Content-Type: text/plain\r\n" \
                                       "Content Length: 0\r\n");
                }
                return return_buf;
            }
            else
            {
                strcpy(return_buf, "Unable to recognize vision, just receive HTTP/1.1\r\n");
                return return_buf;
            }

        }
        else
        {
            /* Return 404 */
            strcpy(return_buf, "HTTP/1.1 404 Not Found\r\n" \
                               "Content-Type: text/plain\r\n" \
                               "Content-Length: 0\r\n");
            return return_buf;
        }
    }
    else
    {
        /* Return 501 */
        strcpy(return_buf, "HTTP/1.1 501 Not Implemented\r\n" \
                           "Content-Type: text/plain\r\n"\
                           "Content-Length: 0\r\n");
        return return_buf;
    }

}

void handle_sigcld(int sig_num)
{
    pid_t child;
    while(0<waitpid(-1,NULL,WNOHANG));
}
