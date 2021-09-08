/*
Compile with: cc UDP_revEchod.c -o echo_serv
*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#define BUF_LEN 48

int string_upcase(char *in_str, char *out_str)
{
        int i, len;
        len = strlen(in_str);
        for (i = 0; i < len; i++)
        {
            out_str[i] = toupper(in_str[i]);
        }
        out_str[len] = '\0';
        return len;
}

int main(int argc, char *argv[])
{
        int ssd;                                /* server socket descriptor */
        struct sockaddr_in server;              /* server address structure */
        struct sockaddr_in client;              /* client address structure */
        int client_len;                         /* size of above client structure */
        short echo_port;                        /* servers port number */
        int max_iterations;                     /* maximum iterations to perform */
        int out_cnt, in_cnt;                    /* byte coutns for send and receive */
        int recv_cnt, i;                        /* more counters */
        char client_string[BUF_LEN];            /* buffer to hold send string */
        char server_upcase_string[BUF_LEN];     /*buffer to hold recieve string */
        int ret_code;                           /* generic return code holder */

        /* Check for correct command line usage */
        if (argc != 3)
        {
                fprintf(stderr, "Usage: %s insufficient parameters\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        /* Grab the command line arguments and decode them */
        echo_port = atoi(argv[1]);
        max_iterations = atoi(argv[2]);

        /* create the socket */
        ssd = socket(PF_INET, SOCK_DGRAM, 17);

        /* if there's a problem, report it an exit */
        if (ssd < 0) 
        {
                perror("While calling socket()");
                exit(EXIT_FAILURE);
        }

        /* set up the server address details in preparation for bind */
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(echo_port);

        /* bind the details in the server sockaddr_in structure to the socket */
        ret_code = bind(ssd, (struct sockaddr *)&server, sizeof(server));
        if (ret_code < 0) 
        {
                perror("While calling bind()");
                exit(EXIT_FAILURE);
        }

        for (i = 0; i < max_iterations; i++)
        {
                fprintf(stderr, "Iteration %d of %d. Waiting for client...\n", 
                                i+1, max_iterations);
                client_len = sizeof(client);

                /* make the server block until message receipt */
                in_cnt = recvfrom(ssd, client_string, BUF_LEN, 0,
                                  (struct sockaddr *)&client,(socklen_t *)&client_len);

                if (in_cnt < 0) 
                {
                        perror("While calling recvfrom()");
                        exit(EXIT_FAILURE);
                }

                fprintf(stderr, "Message received is %d bytes long\n", in_cnt);
                fprintf(stderr, "Message received is \"%s\"\n", client_string);

                /* up case the string */
                recv_cnt = string_upcase(client_string, server_upcase_string);

                fprintf(stderr, "Reversed string is %d bytes long\n", recv_cnt);
                fprintf(stderr, "Reversed string is \"%s\"\n", server_upcase_string);

                out_cnt = sendto(ssd, server_upcase_string, recv_cnt + 1, 0,
                                (struct sockaddr *)&client, sizeof(client));

                if (out_cnt < 0) 
                {
                        perror("While calling sendto()");
                        exit(EXIT_FAILURE);
                }

                fprintf(stderr, "Client request now serviced reply send.\n");
        }

        close(ssd);

        fprintf(stderr, "Server has shut down\n");

        return 0;
}
