#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

cJSON *get_next_path(cJSON *json_path, char *id);
static void resolve_connection(char *url, endpoint_response *resp, char *buffer, size_t *size, char **ctype);
void resolve_endpoint(endpoint *ep, param *params, char *buffer, size_t *size, char **ctype);

cJSON *get_next_path(cJSON *json_path, char *id) {
	if(id[0] == '\0') return json_path;
	if(cJSON_IsObject(json_path)) {
		cJSON *next_path = cJSON_DetachItemFromObjectCaseSensitive(json_path, id);
		//cJSON_Delete(json_path);
		return next_path;
	}
	if(cJSON_IsArray(json_path)) {
		cJSON *next_path = cJSON_DetachItemFromArray(json_path, atoi(id));
		//cJSON_Delete(json_path);
		return next_path;
	}
}

static void resolve_connection(char *url, endpoint_response *resp, char *buffer, size_t *size, char **ctype) {
	printf("Downloading url: %s\n", url);
	download_http(url, buffer, size, ctype);
	printf("Downloaded, size: %lld\n", (long long int)(*size));
	char *next_url;
	char *id;
	switch(resp->data_type) {
		case ENDPOINT_RESPONSE_PLAINTEXT:
			printf("Treating recieved data as PLAINTEXT\n");
			char *pos1 = strstr(buffer, resp->str1), *pos2 = strstr(buffer, resp->str2);
			if(pos1 == NULL || pos2 == NULL) {
				printf("Response strings not found in PLAINTEXT response\n");
				exit(0);
			}
			pos1 += strlen(resp->str1);
			next_url = malloc(strlen(resp->data_str)+pos2-pos1+1);
			strcpy(next_url, resp->data_str);
			strncat(next_url, pos1, pos2 - pos1 + 1);
			resolve_connection(next_url, resp + 1, buffer, size, ctype);
			free(next_url);
			return;
		case ENDPOINT_RESPONSE_JSON:
			printf("Treating recieved data as JSON\n");
			cJSON *json = cJSON_Parse(buffer);
			char *last_path = resp->str1, *path = resp->str1;
			cJSON *json_path = json;
			while(1) {
				switch(*path) {
					case ']':	/* FALLTHROUGH */
					case '[':
					case '.':
					case '\0':
						id = malloc(path - last_path + 1);
						strncpy(id, last_path, path - last_path);
						id[path-last_path] = '\0';
						json_path = get_next_path(json_path, id);
						free(id);
						last_path = path + 1;
						break;
				}
				if(*path == '\0') break;
				++path;
			}
			char *value = json_path->valuestring;
			next_url = malloc(strlen(resp->data_str)+strlen(value)+1);
			strcpy(next_url, resp->data_str);
			strcat(next_url, value);
			resolve_connection(next_url, resp + 1, buffer, size, ctype);
			//cJSON_Delete(json);
			free(next_url);
			return;
		case ENDPOINT_RESPONSE_IMAGE:
			printf("Treating recieved data as IMAGE\n");
			return;
		case ENDPOINT_RESPONSE_NONE:
			printf("ENDPOINT_RESPONSE_NONE found while resolving connection\n");
			exit(1);
	}
}

void resolve_endpoint(endpoint *ep, param *params, char *buffer, size_t *size, char **ctype) {
	strcpy(buffer, ep->base_url);
	while(params) {
		for(int i = 0; i < MAX_URL_OPTIONS; ++i) {
			if(strcmp(params->id, ep->options[i].option) == 0) {
				size_t chars_written = sprintf(buffer + strlen(buffer), ep->options[i].format, params->value);
				printf("Parameter: %s, detected and supported. Format: %s, value: %s, formatted: %s\n", params->id, ep->options[i].format, params->value, buffer + strlen(buffer) - chars_written);
			}
		}
		params = params->next;
	}
	strcat(buffer, ep->url_suffix);
	char url[strlen(buffer) + 1];
	strcpy(url, buffer);
	resolve_connection(url, ep->responses, buffer, size, ctype);
}

