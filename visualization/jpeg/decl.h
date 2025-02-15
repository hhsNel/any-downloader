#ifndef JPEG_DECL
#define JPEG_DECL

#include <string.h>
#include <stdint.h>

#include "config.h"

typedef int16_t jpeg_yuv;

typedef struct _jpeg_rect {
	uint16 left;
	uint16 right;
	uint16 top;
	uint16 bottom;
} jpeg_rect;

typedef struct _jpeg_decompr {
	size_t input_buffer_available;
	uint8_t *data_read_ptr;
	uint8_t *bit_stream_input;
	uint8_t bit_mask_available;
	uint8_t scale;
	uint8_t block_width, block_height;
	uint8_t qtab_id[3];
	uint8_t color_components;
	int16_t prev_dc[3];
	uint16_t restart_interval;
	uint16_t px_width, px_height;
	uint8_t *huff_distr[2][2];
	uint16_t *huff_code[2][2];
	uint8_t *huff_data[2][2];
	int32_t *deq_tab[4];

	uint32_t reg;
	uint8_t marker;

	void *output_buff;
	jpeg_yuv work_buff;
	void *memory_pool;
	size_t pool_available;
	size_t (*input_func)(_jpeg_decompr *, uint8_t *, size_t);
	void *io_device;
} jpeg_decompr;

void jpeg_prep(jpeg_decompr *decompr, size_t (*input_func)(jpeg_decompr *, uint8_t *, size_t), void *pool, size_t pool_size, void *device);
void jpeg_decompress(jpeg_decompr *decompr, int (*output_func)(jpeg_decompr *, void *, jpeg_rect *), uint8_t scale);

#endif

