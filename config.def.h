#define MAX_RESPONSE_JUMPS 2
#define MAX_URL_OPTIONS 3

typedef struct _endpoint {
	char *name;
	char *base_url;
	url_option options[MAX_URL_OPTIONS];
	char *url_suffix;
	endpoint_response responses[MAX_RESPONSE_JUMPS];
} endpoint;

endpoint endpoints[] = {
	/* endpoint name     endpoint url                                endpoint url options                                                            url suffix     (responses in new line) */
	{ "nekos",           "https://nekos.moe/api/v1/random/image?",    {{"nsfw","&nsfw=true"},{"count","&count=%s"},{"",""}},                         "",	/* nokos.moe API v1 */
		{{ENDPOINT_RESPONSE_JSON,"images[%d].id","","https://nekos.moe/image/"},{ENDPOINT_RESPONSE_IMAGE,"","",""}} },
	{ "waifu",           "https://api.waifu.im/search?",              {{"nsfw","&is_nsfw=true"},{"tag","&included_tags=%s"},{"count","&limit=%s"}},  "",	/* waifu.im API version unknown */
		{{ENDPOINT_RESPONSE_JSON,"images[%d].url","",""},{ENDPOINT_RESPONSE_IMAGE,"","",""}} },
};

unsigned int count = 1;	/* default count */
int nsfw = 1;	/* default nsfw. 1 = true, 0 = false */
int sleep_after_image = 0;	/* default sleep. 1 = sleep, 0 = don't sleep */
useconds_t sleep_micros = 10000000;	/* if sleep, sleep for this many microseconds. 1s = 1000ms = 1000000us */
int display = 1;	/* default display. 1 = display the image, 0 = exit when image done */
unsigned long int display_width = 0;	/* If display, set this as max width. 0 or -1 = auto-detect terminal width, use that. */
unsigned long int display_height = -1;	/* If display, set this as max height. 0 = auto-detect terminal height, use that. -1 = auto-detect terminal height, subtract 1, use that */
int use_unicode_halfblock = 1;	/* Uses unicode halfblock to effectively double the resolution on the Y axis. 1 = use, 0 = don't use. Not all terminals support it */
/* int also_farbfeld = 1;	/* Also convert image to farbfeld and save. Needs display option * / <- not available at the moment */
int repeat = 0;	/* repeat downloading instead of exiting. 1 = repeat, 0 = exit immadietly after everything else done */
int verbose_mode = 1; /* default verbose mode. 1 = verbose, 0 = silent */
int save_as_file = 0; /* Whether to save as a file. 1 = save, 0 = discard after print */
endpoint *chosen_endpoint = &endpoints[0];	/* default endpoint (first endpoint on list) */

#define DOWNLOAD_BUFFER_SIZE 8*1024*1024
/* max download size: 8 mb */

#define DOWNLOAD_FILENAME "ad-out-%d."
/* filename without file extenstion */

