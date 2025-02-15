#include <curl/curl.h>
#include <string.h>
#include <stdio.h>

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
	strcat(*(char **)userp, buffer);
	return size;
}

void download_http(char *url, char *buff) {
	CURL *curl = curl_easy_init();
	if(!curl) {
		printf("Could not init CURL\n");
		exit(1);
	}

	buff[0] = '\0';

	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
}
