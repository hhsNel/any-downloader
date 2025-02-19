#include <sys/ioctl.h>
#include <unistd.h>

#include "image.h"

void render_image(char *buffer, size_t buffer_size, char *type) {
	if(strcmp(type, "jpeg")) {
		struct imgRawImage *img = load_jpeg_from_buff(buffer, buffer_size);

		unsigned long int w = display_width;
		unsigned long int h = display_height;
		struct winsize win;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
		if(w == 0 || w == -1) {
			w = win.ws_col;
		}
		if(h == 0 || h == -1) {
			h = win.ws_row;
			if(display_height == -1) {
				--h;
			}
		}
		if(use_unicode_halfblock) {
			h *= 2;
		}

		struct imgRawImage *dimg = downscale_image(*img, w, h);

		if(!use_unicode_halfblock) {
			output_raw_image(*dimg);
		} else {
			output_raw_image_halfblock(*dimg);
		}
		free(img->lpData);
		free(img);
		free(dimg->lpData);
		free(dimg);
	} else {
		LOG("Unsupported format. Not rendering\n");
	}
	if(sleep_after_image) {
		usleep(sleep_micros);
	}
	printf("\n");
}
