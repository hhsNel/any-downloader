#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

typedef struct _http_hb {
	char headers[SOCK_BUFFER_SIZE];
	char body[SOCK_BUFFER_SIZE];
} http_hb;

void http_request(char *host, char *path, char *method, http_hb *request);
void read_request(char *buffer, http_hb *hb);
void write_request(char *buffer, http_hb hb, char *method, char *path);

void http_request(char *host, char *path, char *method, http_hb *request) {
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[SOCK_BUFFER_SIZE];

	portno = 80;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		printf("Could not open socket\n");
		exit(1);
	}
	
	server = gethostbyname(host);
	if(server == NULL) {
		printf("Could not find host %s\n", host);
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

	serv_addr.sin_port = htons(portno);
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Could not connect\n");
		exit(1);
	}

	write_request(buffer, *request, method, path);
	if(write(sockfd, buffer, strlen(buffer)) < 0) {
		printf("Could not write to socket");
		exit(1);
	}

	bzero(buffer, SOCK_BUFFER_SIZE);
	if(read(sockfd, buffer, SOCK_BUFFER_SIZE-1) < 0) {
		printf("Could not read from socket");
		exit(1);
	}
	close(sockfd);
	read_request(buffer, request);
}

void read_request(char *buffer, http_hb *hb) {
	char *i = buffer;
	i = strchr(i, '\n') + 1;
	char *headers_end = strstr(i, "\n\n");
	strncpy(hb->headers, i, headers_end - i);
	i = headers_end + 2;
	strcpy(hb->body, i);
}

void write_request(char *buffer, http_hb hb, char *method, char *path) {
	strcpy(buffer, method);
	strcat(buffer, " ");
	strcat(buffer, path);
	strcat(buffer, "HTTP/1.1\n");
	strcat(buffer, hb.headers);
	strcat(buffer, "\n");
	strcat(buffer, hb.body);
	printf("  dd  %s", buffer);
}
