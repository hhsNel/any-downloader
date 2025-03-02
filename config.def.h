#define MAX_RESPONSE_JUMPS 2
/* Maximum amount of URL "hoops" to jump through, eg: nekos API URL -> nekos API image -> actual image file = 2 hoops */
#define MAX_URL_OPTIONS 4
/* Maximum amount of different URL parameters and endpoint could support */

/* ignore this */
typedef struct _endpoint {
	char *name;
	char *base_url;
	url_option options[MAX_URL_OPTIONS];
	char *url_suffix;
	endpoint_response responses[MAX_RESPONSE_JUMPS];
} endpoint;
/* ok now stop ignoring this */

/* different endpoint. How to add one will maybe be later added to readme (check readme anyway because I might have forgotten to edit this) */
endpoint endpoints[] = {
	/* endpoint name     endpoint url                                endpoint url options                                                                                         url suffix     (responses in new line) */
	{ "nekos",           "https://nekos.moe/api/v1/random/image?",    {{"nsfw","&nsfw=true"},{"no-nsfw","&nsfw=false"},{"count","&count=%s"},{"",""}},                            "",	/* nokos.moe API v1 */
		{{ENDPOINT_RESPONSE_JSON,"images[%d].id","","https://nekos.moe/image/"},{ENDPOINT_RESPONSE_IMAGE,"","",""}} },
	{ "waifu",           "https://api.waifu.im/search?",              {{"nsfw","&is_nsfw=true"},{"no-nsfw","&is_nsfw=false"},{"tag","&included_tags=%s"},{"count","&limit=%s"}},  "",	/* waifu.im API version unknown */
		{{ENDPOINT_RESPONSE_JSON,"images[%d].url","",""},{ENDPOINT_RESPONSE_IMAGE,"","",""}} },
	/* If an endpoint you're trying to add does not have a public API, try to contact the dev team or anyone else to see if they are OK with it.
	 * Definietly do not share with others endpoints that were not intended by their developer for application use without that develloper's consent */
};

unsigned int count = 1;	/* default count */
int nsfw = 0;	/* default nsfw. 1 = true, 0 = false */
int sleep_after_image = 0;	/* default sleep. 1 = sleep, 0 = don't sleep */
useconds_t sleep_micros = 50000000;	/* if sleep, sleep for this many microseconds. 1s = 1000ms = 1000000us */
int display = 1;	/* default display. 1 = display the image, 0 = exit when image done */
unsigned long int display_width = 0;	/* If display, set this as max width. 0 or -1 = auto-detect terminal width, use that. */
unsigned long int display_height = -1;	/* If display, set this as max height. 0 = auto-detect terminal height, use that. -1 = auto-detect terminal height, subtract 1, use that */
int use_unicode_halfblock = 1;	/* Uses unicode halfblock to effectively double the resolution on the Y axis. 1 = use, 0 = don't use. Not all terminals support it */
/* int also_farbfeld = 1;	/ * Also convert image to farbfeld and save. Needs display option * / <- not available at the moment */
int repeat = 0;	/* repeat downloading instead of exiting. 1 = repeat, 0 = exit immadietly after everything else done */
int verbose_mode = 1; /* default verbose mode. 1 = verbose, 0 = silent */
int save_as_file = 0; /* Whether to save as a file. 1 = save, 0 = discard after print */
int clear_before_display = 0; /* Whether or not to clear the screen before displaying the image. 1 = clear, 0 = preserve */
endpoint *chosen_endpoint = &endpoints[0];	/* default endpoint (first endpoint on list) */

#define DOWNLOAD_BUFFER_SIZE 16*1024*1024
/* max download size: 16 mb * count */

#define DOWNLOAD_FILENAME "ad-out-%d."
/* filename without file extenstion. For those not knowing format strings, %d means "insert a number here", that number being the # of the currently
 * downloaded image, from 0 till count-1. */

#define USE_HELPER_NO_ARGS 0
/* Whether to display the helper string and exit when no arguments are provided. 1 = do that, 0 = proceed with default options */
#define HELPER_STRING "\
-c count       | --count count       = try to retrieve {count} images at a time                      \n\
-n             | --nsfw              = retrieve nsfw images                                          \n\
-N             | --no-nsfw           = turn nsfw off                                                 \n\
-p param value | --param param valye = add a parameter if the endpoint supports it                   \n\
-e endpoint    | --endpoint endpoint = retrieve from {endpoint}                                      \n\
-d             | --display           = after retrieving image, show it on console                    \n\
-D             | --no-display        = do not show image on console                                  \n\
-f             | --farbfeld          = also convert image to farbfeld                                \n\
-F             | --no-farbfeld       = do not convert to farbfeld                                    \n\
-s micros      | --sleep micros      = after retrieving an image, wait for {micros} microseconds     \n\
-h             | --halfblock         = uses halfblock trick to double vertical resolution            \n\
-H             | --no-halfblock      = use spaces for normal vertical resolution                     \n\
-S             | --no-sleep          = do not wait after retrieving an image                         \n\
-r             | --reapeat           = repeat downloading catgirls                                   \n\
-R             | --no-repeat         = download only once, then exit                                 \n\
-v             | --verbose           = toggle verbose mode on                                        \n\
-V             | --no-verbose        = toggle verbose mode off                                       \n\
-o             | --physical-file     = save downloaded image as a file                               \n\
-O             | --no-physical-file  = don't save downloaded image as a file                         \n\
-x chars       | --x chars           = the rendered image should be {chars} characters wide.         \n\
0 or -1 pull terminal width, then use that. Useless without -d.                                      \n\
-y chars       | --y chars           = the rendered image should be {chars} characters tall.         \n\
0 pulls terminal width, then uses that. -1 pulls terminal width and subtracts 1. Useless without -d. \n\
                 --help              = display helper string and exit.                               \n\
-b             | --clear             = clear screen before displaying image                          \n\
-B             | --no-clear          = don't clear the creen                                         \n"

