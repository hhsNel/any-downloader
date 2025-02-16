#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ANSI_BEGIN_FG_SEQ "38;2;"
#define ANSI_BEGIN_BG_SEQ "48;2;"

typedef struct _color {
	uint8_t r, g, b;
} color;

void append_fg(char *str, color c);
void append_bg(char *str, color c);

void append_fg(char *str, color c) {
	char buffer[strlen(ANSI_BEGIN_FG_SEQ) + 3*3 + 2*1 + 1];	/* seq + 3 * three digits + 2 * semicolon + null char */
	sprintf(buffer, ANSI_BEGIN_FG_SEQ "%d;%d;%d", c.r, c.g, c.b);
}

void append_bg(char *str, color c) {
	char buffer[strlen(ANSI_BEGIN_BG_SEQ) + 3*3 + 2*1 + 1];	/* seq + 3 * three digits + 2 * semicolon + null char */
	sprintf(buffer, ANSI_BEGIN_BG_SEQ "%d;%d;%d", c.r, c.g, c.b);
}

