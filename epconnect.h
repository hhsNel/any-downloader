#include <curl/curl.h>
#include <string.h>
#include <stdio.h>

static struct download_data {
	char *buffer;
	size_t *size;
};

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
	struct download_data dd = *(struct download_data *)userp;
	memcpy(dd.buffer + *dd.size, buffer, size * nmemb);
	*dd.size += size * nmemb;
	return size * nmemb;
}

void download_http(char *url, char *buff, size_t *size, char **ctype) {
	CURL *curl = curl_easy_init();
	if(!curl) {
		printf("Could not init CURL\n");
		exit(1);
	}

	buff[0] = '\0';
	*size = 0;

	struct download_data dd;
	dd.buffer = buff;
	dd.size = size;

	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dd);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	res = curl_easy_perform(curl);
	if(res != CURLE_OK) {
		printf("Could not download\n");
		exit(1);
	}

	char *content_type = NULL;
	res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
	if(res != CURLE_OK || content_type == NULL) {
		printf("Could not get Content-Type header, continuing...\n");
		*ctype = NULL;
	} else {
		*ctype = strdup(content_type);
	}

	curl_easy_cleanup(curl);
}
