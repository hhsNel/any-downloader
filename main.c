#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "response.h"
#include "urloption.h"
#include "config.h"

int main(int argc, char **argv) {
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
				case 't':
					printf("Not implemented yet\n");
					exit(1);
					if(arg + 1 >= argc) {
						printf("Expected another argument after -t\n");
						exit(1);
					}
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
	return 0;
}
