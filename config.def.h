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
	/* endpoint name     endpoint url                                endpoint url options                                      url suffix     (responses in new line) */
	{ "nekos",           "https://nekos.moe/api/v1/random/image?",    {{"nsfw","&nsfw=true"},{"",""}},                         "",	/* nokos.moe API v1 */
		{{ENDPOINT_RESPONSE_JSON,"images[0].id","","https://nekos.moe/image/"},{ENDPOINT_RESPONSE_IMAGE,"","",""}} },
	{ "waifu",           "https://api.waifu.im/search?",              {{"nsfw","&is_nsfw=true"},{"tag","&included_tags=%s"}},  "",	/* waifu.im API version unknown */
		{{ENDPOINT_RESPONSE_JSON,"images[0].url","",""},{ENDPOINT_RESPONSE_IMAGE,"","",""}} },
};

#define COMPILE_JPEG_SUPPORT
/* comment (using //, so: //#define ...) to remove jpeg support. Displaying and converting jpeg to farbfeld will no longer work. -ljpeg no longer needed on compile */

int count = 1;	/* default count */
int nsfw = 1;	/* default nsfw. 1 = true, 0 = false */
int sleep = 0;	/* default sleep. 1 = sleep, 0 = don't sleep */
useconds_t sleep_micros = 10000000;	/* if sleep, sleep for this many microseconds. 1s = 1000ms = 1000000us */
int display = 1;	/* default display. 1 = display the image, 0 = exit when image done */
unsigned long int display_width = 0;	/* If display, set this as max width. 0 or -1 = auto-detect terminal width, use that. */
unsigned long int display_height = -1;	/* If display, set this as max height. 0 = auto-detect terminal height, use that. -1 = auto-detect terminal height, subtract 1, use that */
endpoint *chosen_endpoint = &endpoints[0];	/* default endpoint (first endpoint on list) */

#define DOWNLOAD_BUFFER_SIZE 8*1024*1024
/* max download size: 8 mb */

#define DOWNLOAD_FILENAME "ad-out."
/* filename without file extenstion */

