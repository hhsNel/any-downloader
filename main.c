#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "response.h"
#include "urloption.h"
#include "config.h"
#include "epconnect.h"
#include "params.h"
#include "endpointresolver.h"
#include "imgrenderer.h"

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
					c_flag:
					if(arg + 1 >= argc) {
						printf("Expected another argument after -c\n");
						exit(1);
					}
					count = atoi(argv[++arg]);
					break;
				case 'n':
					n_flag:
					nsfw = 1;
					break;
				case 'N':
					N_flag:
					nsfw = 0;
					break;
				case 'p':
					p_flag:
					if(arg + 2 >= argc) {
						printf("Expected two more arguments after -t\n");
						exit(1);
					}
					add_param(&head, argv[++arg], argv[++arg]);
					break;
				case 'e':
					e_flag:
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
				case 'd':
					d_flag:
					display = 1;
					break;
				case 'D':
					D_flag:
					display = 0;
					break;
				case 'h':
					h_flag:
					use_unicode_halfblock = 1;
					break;
				case 'H':
					H_flag:
					use_unicode_halfblock = 0;
					break;
				case 's':
					s_flag:
					if(arg + 1 >= argc) {
						printf("Expected another argument after -s\n");
						exit(1);
					}
					sleep_after_image = 1;
					sleep_micros = atoi(argv[++arg]);
					break;
				case 'S':
					S_flag:
					sleep_after_image = 0;
					break;
				case 'r':
					r_flag:
					repeat = 1;
					break;
				case 'R':
					R_flag:
					repeat = 0;
					break;
				case '-':
					if(strcmp(argv[arg]+2, "count") == 0) goto c_flag;
					else if(strcmp(argv[arg]+2, "nsfw") == 0) goto n_flag;
					else if(strcmp(argv[arg]+2, "no-nsfw") == 0) goto N_flag;
					else if(strcmp(argv[arg]+2, "param") == 0) goto p_flag;
					else if(strcmp(argv[arg]+2, "endpoint") == 0) goto e_flag;
					else if(strcmp(argv[arg]+2, "display") == 0) goto d_flag;
					else if(strcmp(argv[arg]+2, "no-display") == 0) goto D_flag;
					else if(strcmp(argv[arg]+2, "halfblock") == 0) goto h_flag;
					else if(strcmp(argv[arg]+2, "no-halfblock") == 0) goto H_flag;
					else if(strcmp(argv[arg]+2, "sleep") == 0) goto s_flag;
					else if(strcmp(argv[arg]+2, "no-sleep") == 0) goto S_flag;
					else if(strcmp(argv[arg]+2, "repeat") == 0) goto r_flag;
					else if(strcmp(argv[arg]+2, "no-repeat") == 0) goto R_flag;
					else {
						printf("Unknown flag: %s\n", argv[arg]);
					}
					break;
			}
		} else {
			printf("Unknown flag 2: %s\n", argv[arg]);
		}
	}
	if(nsfw) {
		printf("Adding nsfw\n");
		add_param(&head, "nsfw", "true");
	}
	count %= 1000;
	if(count > 1) {
		char *count_str = malloc(sizeof(char) * 4);
		snprintf(count_str, 3, "%d", count);
		printf("Adding count: %s", count_str);
		add_param(&head, "count", count_str);
	}
	printf("Using endpoint: %s\n", chosen_endpoint->name);
	param *param_print = &head;
	while(param_print->next != NULL) {
		param_print = param_print->next;
		printf("\tParameter: %s, value: %s\n", param_print->id, param_print->value);
	}

	char *buffer[count];
	for(unsigned int i = 0; i < count; ++i) {
		buffer[i] = malloc(DOWNLOAD_BUFFER_SIZE);
	}
	size_t sizes[count];
	char *types[count];
	resolve_endpoint(chosen_endpoint, &head, buffer, sizes, (char **)types);
	char *file_formats[count];
	for(unsigned int i = 0; i < count; ++i) {
		printf("Recieved: %lld bytes, Content-Type: %s\n", (long long int)sizes[i], types[i] ? types[i] : "NULL");
		if(types[i]) {
			file_formats[i] = strchr(types[i], '/') + 1;
			printf("Extracted file format: %s\n", file_formats[i]);
		} else {
			file_formats[i] = "unknown";
		}

		char filename[strlen(DOWNLOAD_FILENAME) + 3 + strlen(file_formats[i]) + 1];
		sprintf(filename, DOWNLOAD_FILENAME, i);
		strcat(filename, file_formats[i]);
		printf("Writing to: %s\n", filename);
		FILE *file = fopen(filename, "w");
		fwrite(buffer[i], sizeof(char), sizes[i], file);
		fclose(file);
	}
	for(unsigned int i = 0; i < count; ++i) {
		if(display) {
			render_image(buffer[i], sizes[i], file_formats[i]);
		}
	}
}
