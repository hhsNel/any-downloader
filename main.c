#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "response.h"
#include "urloption.h"
#include "config.h"
#include "epconnect.h"
#include "params.h"
#include "endpointresolver.h"

int main(int argc, char **argv) {
	curl_global_init(CURL_GLOBAL_ALL);
	printf("any-downloader\n");

	param head;
	head.id = "";
	head.value = "";
	head.next = NULL;
	for(int arg = 1; arg < argc; ++arg) {
		if(argv[arg][0] == '-') {
			switch(argv[arg][1]) {
				case 'c':
					if(arg + 1 >= argc) {
						printf("Expected another argument after -c\n");
						exit(1);
					}
					count = atoi(argv[++arg]);
					break;
				case 'n':
					nsfw = 1;
					break;
				case 'p':
					if(arg + 2 >= argc) {
						printf("Expected two more arguments after -t\n");
						exit(1);
					}
					add_param(&head, argv[++argc], argv[++argc]);
					break;
				case 'e':
					if(arg + 1 >= argc) {
						printf("Expected another argument after -e\n");
						exit(1);
					}
					char *new_endpoint_name = argv[++arg];
					int i;
					for(i = 0; i < sizeof(endpoints)/sizeof(endpoint); ++i) {
						if(strcmp(new_endpoint_name, endpoints[i].name) == 0) {
							chosen_endpoint = &endpoints[i];
							break;
						}
					}
					if(i == sizeof(endpoints)/sizeof(endpoint)) {
						printf("Cannot find endpoint %s\n", new_endpoint_name);
						exit(1);
					}
					break;
			}
		}
	}
	if(nsfw) {
		printf("Adding nsfw\n");
		add_param(&head, "nsfw", "true");
	}
	printf("Using endpoint: %s\n", chosen_endpoint->name);
	param *param_print = &head;
	while(param_print->next != NULL) {
		param_print = param_print->next;
		printf("\tParameter: %s, value: %s\n", param_print->id, param_print->value);
	}

	char *buffer = malloc(DOWNLOAD_BUFFER_SIZE);
	size_t size;
	char *type;
	resolve_endpoint(chosen_endpoint, &head, buffer, &size, &type);
	printf("Recieved: %lld bytes, Content-Type: %s\n", (long long int)size, type ? type : "NULL");
	char *file_format;
	if(type) {
		file_format = strchr(type, '/') + 1;
		printf("Extracted file format: %s\n", file_format);
	} else {
		file_format = "unknown";
	}
	char filename[strlen(DOWNLOAD_FILENAME) + strlen(file_format) + 1];
	strcpy(filename, DOWNLOAD_FILENAME);
	strcat(filename, file_format);
	printf("Writing to: %s\n", filename);
	FILE *file = fopen(filename, "w");
	fwrite(buffer, sizeof(char), size, file);
	fclose(file);
}
