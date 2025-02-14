#define MAX_RESPONSE_JUMPS 2
#define MAX_URL_OPTIONS 2

typedef struct _endpoint {
	char *name;
	char *base_url;
	url_option options[MAX_URL_OPTIONS];
	char *url_suffix;
	char *http_headers;
	endpoint_response responses[MAX_RESPONSE_JUMPS];
} endpoint;

endpoint endpoints[] = {
	/* endpoint name  endpoint url                             endpoint url options                           url suffix  additional header(s)  (responses in new line) */
	{ "nekos",        "https://nekos.moe/api/v1/random/image", {{"nsfw","&nsfw=true"},{"count","&count=%d"}}, "",         "",
		{{ENDPOINT_RESPONSE_JSON,"images[0].id","","https://nekos.moe/image/"},{ENDPOINT_RESPONSE_IMAGE,"","",""}} }
};

