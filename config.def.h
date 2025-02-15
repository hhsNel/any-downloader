#define MAX_RESPONSE_JUMPS 2
#define MAX_URL_OPTIONS 2

typedef struct _endpoint {
	char *name;
	char *base_url;
	url_option options[MAX_URL_OPTIONS];
	char *url_suffix;
	endpoint_response responses[MAX_RESPONSE_JUMPS];
} endpoint;

endpoint endpoints[] = {
	/* endpoint name     endpoint url                                endpoint url options                           url suffix     (responses in new line) */
	{ "nekos",           "https://nekos.moe/api/v1/random/image",    {{"nsfw","&nsfw=true"},{"",""}},               "",
		{{ENDPOINT_RESPONSE_JSON,"images[0].id","","https://nekos.moe/image/"},{ENDPOINT_RESPONSE_IMAGE,"","",""}} }
};

int count = 1;	/* default count */
int nsfw = 1;	/* default nsfw. 1 = true, 0 = false */
endpoint *chosen_endpoint = &endpoints[0];	/* default endpoint (first endpoint on list) */

#define DOWNLOAD_BUFFER_SIZE 8*1024*1024
/* max download size: 8 mb */

