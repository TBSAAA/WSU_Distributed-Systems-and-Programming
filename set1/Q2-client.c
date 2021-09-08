/*
 * Compile with: gcc Q2-client.c -o Q2-client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <unistd.h>

#define BUF_LEN 48

int main(int argc, char *argv[]) 
{
	int csd;                                /* client socket descriptor */
	struct sockaddr_in server;              /* server address structure */
	struct hostent *server_host;            /* pointer to server host details structure returned by resolver */
	int server_len;                         /* size of above structure */
	int string_size;                        /* size of send string including trailing null */
	short server_port;                      /* servers port number */
	int in_cnt, out_cnt;                    /* byte counts for send and receive */
	char client_send_string[BUF_LEN];       /* buffer to hold send string */
	char server_upcase_string[BUF_LEN];   /* buffer to hold receive string */

    /* Check for correct command line usage */
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s insufficient parameters\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    /* Grab the command line arguments and decode them */

    /* Use the resolver to get the address of the server */
	server_host = gethostbyname(argv[1]);

    /* if there's a problem, report it and exit */
	if (server_host == NULL) 
	{
        herror("While calling gethostbyname()");
		exit(EXIT_FAILURE);
	}

	server_port = atoi(argv[2]);
	strncpy(client_send_string, argv[3], BUF_LEN);

    /* create the socket */
	csd = socket(PF_INET, SOCK_DGRAM, 0);

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

	string_size = strlen(client_send_string)+1;

	int con_return = connect(csd, (struct sockaddr *)&server, sizeof(server));
    /* if there's a problem, report it and exit */
	if (con_return < 0)
	{
		perror("While calling connect()");
		exit(EXIT_FAILURE);
	}

    /* send the message off to the server */
    out_cnt = write(csd, client_send_string, string_size);
    /* if there's a problem, report it and exit */
	if (out_cnt < 0)
	{
		perror("While calling write()");
		exit(EXIT_FAILURE);
	}

    fprintf(stderr,"You have sent \"%s\"\n",client_send_string);
    fprintf(stderr,"Have reached read().\n");

	in_cnt = read(csd, server_upcase_string, BUF_LEN);
    /* if there's a problem, report it and exit */
	if (in_cnt < 1)
	{
		perror("While calling read()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "The server has responded with: \"%s\"\n", server_upcase_string);

    /* close the socket now */
	close(csd);
}
