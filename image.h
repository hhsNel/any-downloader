#include <stdio.h>
#include <string.h>
#include <jpeglib.h>
#include <jerror.h>

#define TO_BIG_ENDIAN(val) \
	(((val) % 256) << 24 | ((val) / 256) % 256 << 16 | ((val) / 65536) % 256 << 8 | ((val) / 16777216) % 256)
#define DEBUG

struct imgRawImage {
	unsigned int numComponents;
	unsigned long int width, height;

	unsigned char* lpData;
};

struct imgRawImage *load_jpeg(struct jpeg_decompress_struct *info) {
	struct imgRawImage* lpNewImage;

	unsigned long int imgWidth, imgHeight;
	int numComponents;

	unsigned long int dwBufferBytes;
	unsigned char* lpData;

	unsigned char* lpRowBuffer[1];

	jpeg_read_header(info, TRUE);

	jpeg_start_decompress(info);
	imgWidth = info->output_width;
	imgHeight = info->output_height;
	numComponents = info->num_components;

	#ifdef DEBUG
	fprintf(
		stderr,
		"%s:%u: Reading JPEG with dimensions %lu x %lu and %u components\n",
		__FILE__, __LINE__,
		imgWidth, imgHeight, numComponents
	);
	#endif

	dwBufferBytes = imgWidth * imgHeight * 3; /* We only read RGB, not A */
	lpData = (unsigned char*)malloc(sizeof(unsigned char)*dwBufferBytes);

	lpNewImage = (struct imgRawImage*)malloc(sizeof(struct imgRawImage));
	lpNewImage->numComponents = numComponents;
	lpNewImage->width = imgWidth;
	lpNewImage->height = imgHeight;
	lpNewImage->lpData = lpData;

	/* Read scanline by scanline */
	while(info->output_scanline < info->output_height) {
			lpRowBuffer[0] = (unsigned char *)(&lpData[3*info->output_width*info->output_scanline]);
			jpeg_read_scanlines(info, lpRowBuffer, 1);
	}

	jpeg_finish_decompress(info);
	jpeg_destroy_decompress(info);

	return lpNewImage;
}

struct imgRawImage* load_jpeg_from_file(char* lpFilename) {
	struct jpeg_decompress_struct info;
	struct jpeg_error_mgr err;

	jpeg_create_decompress(&info);
	info.err = jpeg_std_error(&err);

	FILE* fHandle;

	fHandle = fopen(lpFilename, "rb");
	if(fHandle == NULL) {
		#ifdef DEBUG
		fprintf(stderr, "%s:%u: Failed to read file %s\n", __FILE__, __LINE__, lpFilename);
		#endif
		return NULL; /* ToDo */
	}

	jpeg_stdio_src(&info, fHandle);

	struct imgRawImage *img = load_jpeg(&info);

	fclose(fHandle);
	return img;
}

struct imgRawImage *load_jpeg_from_buff(char *buffer, size_t size) {
	struct jpeg_decompress_struct info;
	struct jpeg_error_mgr err;

	jpeg_create_decompress(&info);
	info.err = jpeg_std_error(&err);
	jpeg_mem_src(&info, buffer, size);

	return load_jpeg(&info);
}

void save_ff(struct imgRawImage *img, char *filename) {
	FILE *f = fopen(filename, "wb");
        fprintf(f, "farbfeld");
        unsigned int ffw = TO_BIG_ENDIAN(img->width);
        unsigned int ffh = TO_BIG_ENDIAN(img->height);
        fwrite(&ffw, sizeof(unsigned int), 1, f);
        fwrite(&ffh, sizeof(unsigned int), 1, f);
        for(unsigned long y = 0; y < img->height; ++y) {
                for(unsigned long x = 0; x < img->width; ++x) {
                        size_t i = 3 * (y * img->width + x);
	                        fwrite(&img->lpData[i], sizeof(unsigned char), 1, f);
	                        fwrite(&img->lpData[i], sizeof(unsigned char), 1, f);
	                        fwrite(&img->lpData[i+1], sizeof(unsigned char), 1, f);
	                        fwrite(&img->lpData[i+1], sizeof(unsigned char), 1, f);
	                        fwrite(&img->lpData[i+2], sizeof(unsigned char), 1, f);
 				fwrite(&img->lpData[i+2], sizeof(unsigned char), 1, f);
	                        fputc(-1, f);
	                        fputc(-1, f);
																														                }
        }
}

struct imgRawImage *downscale_image(struct imgRawImage img, unsigned long int max_w, unsigned long int max_h) {
	struct imgRawImage *downscaled = malloc(sizeof(struct imgRawImage));
	if(max_w * img.height > max_h * img.width) {
		downscaled->height = max_h;
		downscaled->width = img.width * downscaled->height / img.height;
	} else {
		downscaled->width = max_w;
		downscaled->height = img.height * downscaled->width / img.width;
	}
	downscaled->numComponents = img.numComponents;
	downscaled->lpData = malloc(sizeof(unsigned char) * downscaled->width * downscaled->height * downscaled->numComponents);
	for(unsigned int x = 0; x < downscaled->width; ++x) {
		for(unsigned int y = 0; y < downscaled->height; ++y) {
			size_t downscaled_index = 3 * (y * downscaled->width + x);
			size_t img_x = x * img.width / downscaled->width;
			size_t img_y = y * img.height / downscaled->height;
			size_t img_index = 3 * (img_y * img.width + img_x);
			//printf("Copying pixel [%lu; %lu], color: {%d, %d, %d}, to pos [%d; %d]\n", img_x, img_y, img.lpData[img_index],img.lpData[img_index+1],img.lpData[img_index+2],
			//		x, y);
			for(unsigned int c = 0; c < img.numComponents; ++c) {
				downscaled->lpData[downscaled_index + c] = img.lpData[img_index + c];
			}
		}
	}
	return downscaled;
}

void output_raw_image(struct imgRawImage img) {
	char *buffer = malloc(strlen("\033[48;2;000;000;000m ") * img.width + 1);
	for(unsigned long int i=0;i<img.height;++i) {
		size_t length = 0;
		for(unsigned long int j=0;j<img.width;++j) {
			size_t px_offset = 3 * (i * img.width + j);
			length += sprintf(buffer + length, "\033[48;2;%d;%d;%dm ", (int)img.lpData[px_offset], (int)img.lpData[px_offset+1], (int)img.lpData[px_offset+2]);
		}
		if(i != img.height-1) printf("%s\033[0m\n", buffer);
	}
	printf("%s\033[m", buffer);
	free(buffer);
}

void output_raw_image_halfblock(struct imgRawImage img) {
	char *buffer = malloc(strlen("\033[48;2;000;000;000;38;2;000;000;000m\u2584") * img.width + 1);
	for(unsigned long long int i = 0; i < img.height; i += 2) {
		size_t length = 0;
		for(unsigned long int j=0; j < img.width; ++j) {
			size_t px1_offset = 3 * (i * img.width + j);
			size_t px2_offset = 3 * ((i+1) * img.width + j);
			length += sprintf(buffer + length, "\033[48;2;%d;%d;%d;38;2;%d;%d;%dm\u2584", 
					(int)img.lpData[px1_offset], (int)img.lpData[px1_offset+1], (int)img.lpData[px1_offset+2],
					(int)img.lpData[px2_offset], (int)img.lpData[px2_offset+1], (int)img.lpData[px2_offset+2]
					);
		}
		if(i != img.height-1) printf("%s\033[0m\n", buffer);
	}
	/* skip last row if height is odd */
	printf("%s\033[m", buffer);
	free(buffer);
}

