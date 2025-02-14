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
	char *headers;
	char *body;
} http_hb;

typedef struct _http_read {
	char buff[SOCK_BUFFER_SIZE];
	int reading_stage;
	size_t len;
} http_read;

void http_request(char *host, char *path, char *method, http_hb *request);
void read_request(http_read *read, http_hb *hb);
void write_request(http_read *read, http_hb hb, char *method, char *path);

void http_request(char *host, char *path, char *method, http_hb *request) {
	int sockfd, portno, n, bytes_comm;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	//char buffer[SOCK_BUFFER_SIZE];
	http_read read_progress;

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

	read_progress.reading_stage = 0;
	read_progress.len = 0;
	do {
		read_progress.buff[0] = '\0';
		write_request(&read_progress, *request, method, path);
		printf("%s<END_OF_BUFF_>", read_progress.buff);
		bytes_comm = write(sockfd, read_progress.buff, strlen(read_progress.buff));
	       	if(bytes_comm < 0) {
			printf("Could not write to socket");
			exit(1);
		}
	} while(bytes_comm != 0);
	printf("\nSent!\n");
	
	read_progress.reading_stage = 0;
	do {
		bzero(read_progress.buff, SOCK_BUFFER_SIZE);
		if(read(sockfd, read_progress.buff, SOCK_BUFFER_SIZE-1) < 0) {
			printf("Could not read_progress from socket");
			exit(1);
		}
		read_request(&read_progress, request);
		printf("%s<END_OF_BUFF_>", read_progress.buff);
	} while(bytes_comm != 0);
	close(sockfd);
}

void read_request(http_read *read, http_hb *hb) {
	char *buff = read->buff;
	if(read->reading_stage == 0) {	/* read HTTP header */
		char *header_end = strchr(buff, '\n');
		if(header_end == NULL) {
			return;	/* reading header and it is not read yet, discard */
		} else {
			buff = header_end + 1;	/* set currectly-reading-buffer to after the http header ends */
			read->reading_stage = 1;	/* set reading_stage to headers */
			hb->headers[0] = '\0';	/* set headers to an empty string */
		}
	}
	if(read->reading_stage == 1) {	/* read headers sent by server */
		char *headers_end = strstr(buff, "\n\n");
		if(headers_end == NULL) {	/* reading headers and they have not been fully read yet */
			strcat(hb->headers, buff);	/* copy the buffer to headers */
		} else {
			strncat(hb->headers, buff, headers_end - buff);	/* move the rest of the headers to hb */
			buff = headers_end + 2;	/* set buffer to after the headers end */
			read->reading_stage = 2;	/* set reading_stage to body */
			hb->body[0] = '\0';	/* set bpdy to an empty string */
		}
	}
	if(read->reading_stage == 2) {
		strcat(hb->body, buff);	/* append buff to body */
	}
}

void write_request(http_read *read, http_hb hb, char *method, char *path) {
	if(read->reading_stage == 0) {	/* method */
		size_t method_len = strlen(method);
		if(read->len + sizeof(read->buff) < method_len + 1) {	/* we cannot write the whole method */
			strncpy(read->buff, method + read->len, sizeof(read->buff) - 1);	/* copy whatever we can */
			read->len += sizeof(read->buff) - 1;	/* mark that portion as read */
		} else {
			strcpy(read->buff, method + read->len);	/* copy the rest of the method */
			read->reading_stage = 1;	/* set reading_stage to the space after method */
		}
	}
	if(read->reading_stage == 1) {	/* space after method */
		if(strlen(read->buff) + 1 >= sizeof(read->buff)) {	/* no more space */
			return;
		} else {
			strcat(read->buff, " ");	/* add the space */
			read->reading_stage = 2;	/* set reading_stage to path */
			read->len = 0;
		}
	}
	if(read->reading_stage == 2) {	/* path */
		size_t path_len = strlen(path);
		if(read->len + sizeof(read->buff) < path_len + 1) {	/* we cannot write the whole path */
			strncat(read->buff, path + read->len, sizeof(read->buff) - 1);	/* copy whatever we can */
			read->len += sizeof(read->buff) - 1;	/* mark that portion as read */
		} else {
			strcat(read->buff, path + read->len);	/* copy the rest of the path */
			read->reading_stage = 3;	/* set reading_stage to the space after path */
		}
	}
	if(read->reading_stage == 3) {	/* space after path */
		if(strlen(read->buff) + 1 >= sizeof(read->buff)) {	/* no more space */
			return;
		} else {
			strcat(read->buff, " ");	/* add the space */
			read->reading_stage = 4;	/* set reading_stage to HTTP/1.1 */
			read->len = 0;
		}
	}
#define HTTP_STRING "HTTP/1.1"
	if(read->reading_stage == 4) {	/* HTTP/1.1 */
		size_t http_str_len = strlen(HTTP_STRING);
		if(read->len + sizeof(read->buff) < http_str_len + 1) {	/* we cannot write the whole string */
			strncat(read->buff, HTTP_STRING + read->len, sizeof(read->buff) - 1);	/* copy whatever we can */
			read->len += sizeof(read->buff) - 1;	/* mark that portion as read */
		} else {
			strcat(read->buff, HTTP_STRING + read->len);	/* copy the rest of the string */
			read->reading_stage = 5;	/* set reading_stage to the newline after the header */
		}
	}
#undef HTTP_STRING
	if(read->reading_stage == 5) {	/* newline after header */
		if(strlen(read->buff) + 1 >= sizeof(read->buff)) {	/* no more space */
			return;
		} else {
			strcat(read->buff, "\n");	/* add the newline */
			read->reading_stage = 6;	/* set reading_stage to the headers */
			read->len = 0;
		}
	}
	if(read->reading_stage == 6) {	/* headers */
		size_t headers_len = strlen(hb.headers);
		if(read->len + sizeof(read->buff) < headers_len + 1) {	/* we cannot write all of the headers */
			strncat(read->buff, hb.headers + read->len, sizeof(read->buff) - 1);	/* copy whatever we can */
			read->len += sizeof(read->buff) - 1;	/* mark that portion as read->*/
		} else {
			strcat(read->buff, hb.headers + read->len);	/* copy the rest of the headers */
			read->reading_stage = 7;	/* set read->ng_stage to the newline after the headers */
		}
	}
	if(read->reading_stage == 7) {	/* newline after headers */
		if(strlen(read->buff) + 1 >= sizeof(read->buff)) {	/* no more space */
			return;
		} else {
			strcat(read->buff, "\n");	/* add the newline */
			read->reading_stage = 8;	/* set read->ng_stage to the body */
			read->len = 0;
		}
	}
	if(read->reading_stage == 8) {	/* body */
		size_t body_len = strlen(hb.body);
		if(read->len + sizeof(read->buff) < body_len + 1) {	/* we cannot write all of the body */
			strncat(read->buff, hb.body + read->len, sizeof(read->buff) - 1);	/* copy whatever we can */
			read->len += sizeof(read->buff) - 1;	/* mark that portion as read->*/
		} else {
			strcat(read->buff, hb.body + read->len);	/* copy the rest of the body */
			read->reading_stage = 9;	/* set read->ng_stage to the newline after the body */
		}
	}
	if(read->reading_stage == 9) {	/* newline after body */
		if(strlen(read->buff) + 1 >= sizeof(read->buff)) {	/* no more space */
			return;
		} else {
			strcat(read->buff, "\n");	/* add the newline */
			read->reading_stage = 10;	/* set read->ng_stage to nothing */
		}
	}
}
