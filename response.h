#define ENDPOINT_RESPONSE_PLAINTEXT 0
#define ENDPOINT_RESPONSE_JSON 1
#define ENDPOINT_RESPONSE_IMAGE 2
#define ENDPOINT_RESPONSE_NONE 3
typedef struct _endpoint_response {
	int data_type;
	char *str1;
	char *str2;
	char *data_str;
} endpoint_response;

