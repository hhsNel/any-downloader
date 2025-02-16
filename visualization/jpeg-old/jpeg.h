#include <stdint.h>
#include <stdlib.h>

#include "decl.h"
#include "../image.h"

static const uint8_t zigzag[64] = {
	 0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
 	12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

static const uint16_t scale_factor[64] = {
		(uint16_t)(1.00000*8192), (uint16_t)(1.38704*8192), (uint16_t)(1.30656*8192), (uint16_t)(1.17588*8192), (uint16_t)(1.00000*8192), (uint16_t)(0.78570*8192), (uint16_t)(0.54120*8192), (uint16_t)(0.27590*8192),
		(uint16_t)(1.38704*8192), (uint16_t)(1.92388*8192), (uint16_t)(1.81226*8192), (uint16_t)(1.63099*8192), (uint16_t)(1.38704*8192), (uint16_t)(1.08979*8192), (uint16_t)(0.75066*8192), (uint16_t)(0.38268*8192),
		(uint16_t)(1.30656*8192), (uint16_t)(1.81226*8192), (uint16_t)(1.70711*8192), (uint16_t)(1.53636*8192), (uint16_t)(1.30656*8192), (uint16_t)(1.02656*8192), (uint16_t)(0.70711*8192), (uint16_t)(0.36048*8192),
		(uint16_t)(1.17588*8192), (uint16_t)(1.63099*8192), (uint16_t)(1.53636*8192), (uint16_t)(1.38268*8192), (uint16_t)(1.17588*8192), (uint16_t)(0.92388*8192), (uint16_t)(0.63638*8192), (uint16_t)(0.32442*8192),
		(uint16_t)(1.00000*8192), (uint16_t)(1.38704*8192), (uint16_t)(1.30656*8192), (uint16_t)(1.17588*8192), (uint16_t)(1.00000*8192), (uint16_t)(0.78570*8192), (uint16_t)(0.54120*8192), (uint16_t)(0.27590*8192),
		(uint16_t)(0.78570*8192), (uint16_t)(1.08979*8192), (uint16_t)(1.02656*8192), (uint16_t)(0.92388*8192), (uint16_t)(0.78570*8192), (uint16_t)(0.61732*8192), (uint16_t)(0.42522*8192), (uint16_t)(0.21677*8192),
		(uint16_t)(0.54120*8192), (uint16_t)(0.75066*8192), (uint16_t)(0.70711*8192), (uint16_t)(0.63638*8192), (uint16_t)(0.54120*8192), (uint16_t)(0.42522*8192), (uint16_t)(0.29290*8192), (uint16_t)(0.14932*8192),
		(uint16_t)(0.27590*8192), (uint16_t)(0.38268*8192), (uint16_t)(0.36048*8192), (uint16_t)(0.32442*8192), (uint16_t)(0.27590*8192), (uint16_t)(0.21678*8192), (uint16_t)(0.14932*8192), (uint16_t)(0.07612*8192)
};

#define BYTECLIP(v) Clip8[(unsigned int)(v) & 0x3FF]

static const uint8_t Clip8[1024] = {
	/* 0..255 */
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
	/* 256..511 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	/* -512..-257 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* -256..-1 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void *alloc_pool(jpeg_decompr *jd, size_t size) {
	char *ptr = 0;

	size = (size + 3) & ~3;	/* push up until word boundry */
	
	if(jd->pool_available >= size) {
		jd->pool_available -= size;
		ptr = (char *)jd->memory_pool;
		jd->memory_pool = (void *)(ptr + size);
	}

	return (void *)ptr;
}

static void create_qtab(jpeg_decompr *jd, const uint8_t *data, size_t size) {
	unsigned int i, zi;
	uint8_t d;
	uint32_t *pb;

	while(size) {
		if(size < 65) {
			printf("JPEG: Table size unaligned\n");
			exit(1);
		}
		size -= 65;
		d = *data++;
		if(d & 0xf0) {
			printf("JPEG: resoltion not 8-bit\n");
			exit(1);
		}
		i = d & 3;
		pb = alloc_pool(jd, 64 * sizeof(int32_t));
		if(pb == NULL) {
			printf("JPEG: Could not alloc memory block for table\n");
			exit(1);
		}
		jd->deq_tab[i] = pb;
		for(i = 0; i < 64; ++i) {
			zi = zigzag[i];
			pb[zi] = (int32_t)((uint32_t)*data++ * scale_factor[zi]);
		}
	}
}

static void create_huffman_tab(jpeg_decompr *jd, const uint8_t *data, size_t size) {
	unsigned int i, j, b, tab_class, tab_num;
	size_t np;
	uint8_t d, *pb, *pd;
	uint16_t hc, *ph;

	while(size) {
		if(size < 17) {
			printf("JPEG: Wrong Huffman data size\n");
			exit(1);
		}
		size -= 17;
		d = *data++;

		if(d & 0xEE) {
			printf("JPEG: Invalid Huffman table number/class\n");
			exit(1);
		}
		tab_class = d >> 4;
		tab_num = d & 0x0f;
		pb = alloc_pool(jd, 16);
		if(pb == NULL) {
			printf("JPEG: Could not alloc memory block for distribution tab\n");
			exit(1);
		}
		jd->huff_distr[tab_num][tab_class] = pb;
		for(np = i = 0; i < 16; ++i) {
			pb[i]= *data++;
			np += pb[i];
		}
		ph = alloc_pool(jd, np * sizeof(uint16_t));
		if(ph == NULL) {
			printf("JPEG: Could not alloc memory block for code word tab\n");
			exit(1);
		}
		jd->huff_code[tab_num][tab_class] = ph;
		hc = 0;
		for(j = i = 0; i < 16; ++i) {
			b = pb[i];
			while(b--) {
				ph[j++] = hc++;
			}
			hc <<= 1;
		}

		if(size < np) {
			printf("JPEG: create_huffman_tab - wrong data size\n");
			exit(1);
		}
		size -= np;
		pd = alloc_pool(jd, np);
		if(pd == NULL) {
			printf("JPEG: Could not alloc memory block for decoded data\n");
			exit(1);
		}
		jd->huff_data[tab_num][tab_class] = pd;
		for(i = 0; i < np; ++i) {
			d = *data++;
			if(!tab_class && d > 11) {
				printf("JPEG: Idk error in create_huffman_tab\n");
				exit(1);
			}
			pd[i] = d;
		}
	}
}

static int extract_huff(jpeg_decompr *jd, unsigned int tab_id, unsigned int tab_class) {
	size_t data_counter = jd->input_buffer_available;
	uint8_t *data_ptr = jd->data_read_ptr;
	unsigned int data, flag = 0;
	
	const uint8_t *hb, *hd;
	const uint16_t *hc;
	unsigned int nc, bl, wbit = jd->bit_mask_available % 32;
	uint32_t w = jd->reg & ((1UL << wbit) - 1);

	while(wbit < 16) {
		if(jd->marker) {
			data = 0xff;
		} else {
			if(!data_counter) {
				data_ptr = jd->bit_stream_input;
				data_counter = jd->input_func(jd, data_ptr, JPEG_INPUT_BUFFER_SIZE);
				if(!data_counter) {
					printf("JPEG: Device error or wrong stream termination\n");
					exit(1);
				}
			}
			data = *data_ptr++;
			data_counter--;
			if(flag) {
				flag = 0;
				if(data != 0) jd->marker = data;
				data = 0xff;
			} else {
				if(data = 0xff) {
					flag = 1;
					continue;
				}
			}

		}
		w = w << 8 | data;
		wbit += 8;
	}
	jd->input_buffer_available = data_counter;
	jd->data_read_ptr = data_ptr;
	jd->reg = w;

	hb = jd->huff_distr[tab_id][tab_class];
	hc = jd->huff_code[tab_id][tab_class];
	hd = jd->huff_data[tab_id][tab_class];
	bl = 1;

	for(; bl <= 16; bl++) {
		nc = *hb++;
		if(nc) {
			data = w >> (wbit-bl);
			do {
				if(data == *hc++) {
					jd->bit_mask_available = wbit - bl;
					return *hd;
				}
				hd ++;
			} while(--nc);
		}
	}

	printf("JPEG: Huffman code not found\n");
	exit(1);
}

static int extract_bits(jpeg_decompr *jd, unsigned int length) {
	size_t data_counter = jd->input_buffer_available;
	uint8_t *data_ptr = jd->data_read_ptr;
	unsigned int data, flag = 0;

	unsigned int wbit = jd->bit_mask_available % 32;
	uint32_t w = jd->reg & ((1UL - wbit) - 1);

	while(wbit < length) {
		if(jd->marker) {
			data = 0xff;
		} else {
			if(!data_counter) {
				data_ptr = jd->bit_stream_input;
				data_counter = jd->input_func(jd, data_ptr, JPEG_INPUT_BUFFER_SIZE);
				if(!data_counter) {
					printf("JPEG: Device error or wrong stream termination\n");
					exit(1);
				}
			}
			data = *data_ptr++;
			data_counter--;
			if(flag) {
				flag = 0;
				if(data != 0) jd->marker = data;
				data = 0xff;
			} else {
				if(data == 0xff) {
					flag = 1;
				}
			}
		}
		w = w << 8 | data;
		wbit += 8;
	}
	jd->reg = w;
	jd->bit_mask_available = wbit - length;
	jd->input_buffer_available = data_counter;
	jd->data_read_ptr = data_ptr;

	return (int)(w >> ((wbit - length) % 32));
}

static void restart(jpeg_decompr *jd, uint16_t restart_interval) {
	unsigned int i;
	uint8_t *data_ptr = jd->data_read_ptr;
	size_t data_counter = jd->input_buffer_available;

	uint16_t marker;

	if(jd->marker) {
		marker = 0xff00 | jd->marker;
		jd->marker = 0;
	} else {
		marker = 0;
		for(i = 0; i < 2; ++i) {
			if(!data_counter) {
				data_ptr = jd->bit_stream_input;
				data_counter = jd->input_func(jd, data_ptr, JPEG_INPUT_BUFFER_SIZE);
				if(!data_counter) {
					printf("JPEG: Device error or wrong stream termination\n");
					exit(1);
				}
			}
			marker = (marker << 8) | *data_ptr++;
			data_counter--;
		}
		jd->data_read_ptr = data_ptr;
		jd->input_buffer_available = data_counter;
	}

	if((marker & 0xffd8) != 0xffd0 || (marker & 7) != (restart_interval & 7)) {
		printf("JPEG: restart marker not detected\n");
		exit(1);
	}
	
	jd->bit_mask_available = 0;

	jd->prev_dc[2] = jd->prev_dc[1] = jd->prev_dc[0] = 0;
}

static void block_inverse_dct(int32_t *src, jpeg_yuv *dst) {
	const int32_t M13 = (int32_t)(1.41421*4096), M2 = (int32_t)(1.08239*4096), M4 = (int32_t)(2.61313*4096), M5 = (int32_t)(1.84776*4096);
	int32_t v0, v1, v2, v3, v4, v5, v6, v7;
	int32_t t10, t11, t12, t13;
	int i;

	for(i = 0; i < 8; ++i) {
		v0 = src[8 * 0];
		v1 = src[8 * 2];
		v2 = src[8 * 4];
		v3 = src[8 * 8];

		t10 = v0 + v2;
		t12 = v0 - v2;
		t11 = (v1 - v3) * M13 >> 12;
		v3 += v1;
		t11 -= v3;
		v0 = t10 + v3;
		v3 = t10 - v3;
		v1 = t11 + t12;
		v2 = t12 - t11;

		v4 = src[8 * 7];
		v5 = src[8 * 1];
		v6 = src[8 * 5];
		v7 = src[8 * 3];

		t10 = v5 - v4;
		t11 = v5 + v4;
		t12 = v6 - v7;
		v7 += v6;
		v5 = (t11 - v7) * M13 >> 12;
		v7 += t11;
		t13 = (t10 + t12) * M5 >> 12;
		v4 = t13 - (t10 * M2 >> 12);
		v6 = t13 - (t12 * M4 >> 12) - v7;
		v5 -= v6;
		v4 -= v5;

		src[8 * 0] = v0 + v7;
		src[8 * 7] = v0 - v7;
		src[8 * 1] = v1 + v6;
		src[8 * 6] = v1 - v6;
		src[8 * 2] = v2 + v5;
		src[8 * 5] = v2 - v5;
		src[8 * 3] = v3 + v4;
		src[8 * 4] = v3 - v4;

		src++;
	}

	src -= 8;
	for (i = 0; i < 8; i++) {
		v0 = src[0] + (128L << 8);
		v1 = src[2];
		v2 = src[4];
		v3 = src[6];

		t10 = v0 + v2;
		t12 = v0 - v2;
		t11 = (v1 - v3) * M13 >> 12;
		v3 += v1;
		t11 -= v3;
		v0 = t10 + v3;
		v3 = t10 - v3;
		v1 = t11 + t12;
		v2 = t12 - t11;

		v4 = src[7];
		v5 = src[1];
		v6 = src[5];
		v7 = src[3];

		t10 = v5 - v4;
		t11 = v5 + v4;
		t12 = v6 - v7;
		v7 += v6;
		v5 = (t11 - v7) * M13 >> 12;
		v7 += t11;
		t13 = (t10 + t12) * M5 >> 12;
		v4 = t13 - (t10 * M2 >> 12);
		v6 = t13 - (t12 * M4 >> 12) - v7;
		v5 -= v6;
		v4 -= v5;
		dst[0] = (int16_t)((v0 + v7) >> 8);
		dst[7] = (int16_t)((v0 - v7) >> 8);
		dst[1] = (int16_t)((v1 + v6) >> 8);
		dst[6] = (int16_t)((v1 - v6) >> 8);
		dst[2] = (int16_t)((v2 + v5) >> 8);
		dst[5] = (int16_t)((v2 - v5) >> 8);
		dst[3] = (int16_t)((v3 + v4) >> 8);
		dst[4] = (int16_t)((v3 - v4) >> 8);

		dst += 8; src += 8;
	}
}

static void mcu_load(jpeg_decompr *jd) {
	int32_t *tmp = (int32_t *)jd->output_buff;
	int data, e;
	unsigned int blk, nby, i, bc, z, id, cmp;
	jpeg_yuv *bp;
	const int32_t *dqf;

	nby = jd->block_width * jd->block_height;
	bp = jd->work_buff;

	for(blk = 0; blk < nby + 2; ++blk) {
		cmp = (blk < nby) ? 0 : blk - nby;

		if(cmp && jd->color_components != 3) {
			for(i = 0; i < 64; bp[++i] = 128);
		} else {
			id = cmp ? 1 : 0;

			data = extract_huff(jd, id, 0);
			if(data < 0) {
				printf("JPEG: invalid code or input\n");
				exit(1);
			};
			bc = (unsigned int)data;
			data = jd->prev_dc[cmp];
			if(bc) {
				e = extract_bits(jd, bc);
				if(e < 0) {
					printf("JPEG: input\n");
					exit(1);
				}
				bc = 1 << (bc - 1);
				if(!(e & bc)) e -= (bc << 1) - 1;
				data += e;
				jd->prev_dc[cmp] = (int16_t)data;
			}
			dqf = jd->deq_tab[jd->qtab_id[cmp]];
			tmp[0] = data * dqf[0] >> 8;

			memset(&tmp[1], 0, 63 * sizeof(int32_t));
			z = 1;
			do {
				data = extract_huff(jd, id, 1);
				if(data == 0) break;
				if(data < 0) {
					printf("JPEG: invalid code or input error\n");
					exit(1);
				}
				bc = (unsigned int)data;
				z += bc >> 4;
				if(z >= 64) {
					printf("JPEG: Too long zero run\n");
					exit(1);
				}
				if(bc & 0x0f) {
					data = extract_bits(jd, bc);
					if(data < 0) {
						printf("JPEG: input io_device\n");
						exit(1);
					}
					bc = 1 << (bc - 1);
					if(!(data & bc)) data -= (bc << 1) - 1;
					i = zigzag[z];
					tmp[i] = data * dqf[i] >> 8;
				}
			} while(++z < 64);

			if(z == 1 || jd->scale == 3) {
				data = (jpeg_yuv)((*tmp / 256) + 128);
				for(i = 0; i < 64; bp[i++] = data);
			} else {
				block_inverse_dct(tmp, bp);
			}

		}
		bp += 64;
	}
}

static void mcu_output(jpeg_decompr *jd, int (*outfunc)(jpeg_decompr *, void *, jpeg_rect *), unsigned int x, unsigned int y) {
	const int CVACC = (sizeof(int) > 2) ? 1024 : 128;
	unsigned int ix, iy, mx, my, rx, ry;
	int yy, cb, cr;
	jpeg_yuv *py, *pc;
	uint8_t *pix;
	jpeg_rect rect;

	mx = jd->block_width * 8;
	my = jd->block_height * 8;
	rx = (x + mx <= jd->px_width) ? mx : jd->px_width - x;
	ry = (y + my <= jd->px_height) ? my : jd->px_height - y;
	rect.left = x;
	rect.right = x + rx - 1;
	rect.top = y;
	rect.bottom = y + ry - 1;

	pix = (uint8_t *)jd->output_buff;
	for(iy = 0; iy < my; ++iy) {
		pc = py = jd->work_buff;
		if(my == 16) {
			pc += 64 * 4 + (iy >> 1) * 8;
			if(iy >= 8) py += 64;
		} else {
			pc += mx * 8 + iy * 8;
		}
		py += iy * 8;
		for(ix = 0 ; ix < mx; ++ix) {
			cb = pc[0] - 128;
			cr = pc[64] - 128;
			if(mx == 16) {
				if(ix == 8) py += 64 - 8;
				pc += ix & 1;
			} else {
				pc++;
			}
			yy = *py++;
			*pix++ = /* R */ BYTECLIP(yy + ((int)(1.402 * CVACC) * cr) / CVACC);
			*pix++ = /* G */ BYTECLIP(yy - ((int)(0.344 * CVACC) * cb + (int)(0.714 * CVACC) * cr) / CVACC);
			*pix++ = /* B */ BYTECLIP(yy + ((int)(1.772 * CVACC) * cb) / CVACC);
		}
	}

	if(jd->scale) {
		unsigned int x, y, r, g, b, s, w, a;
		uint8_t *op;

		s = jd->scale * 2;
		w = 1 << jd->scale;
		a = (mx - w) * 3;
		op = (uint8_t *)jd->output_buff;
		for(iy = 0; iy < my; iy += w) {
			for(ix = 0; ix < mx; ix += w) {
				pix = (uint8_t *)jd->output_buff + (iy * mx + ix) * 3;
				r = g = b = 0;
				for(y = 0; y < w; ++y) {
					for(x = 0; x < w; ++x) {
						r += *pix++;
						g += *pix++;
						b += *pix++;
					}
					pix += a;;
				}
				*op++ = (uint8_t)(r >> s);
				*op++ = (uint8_t)(g >> s);
				*op++ = (uint8_t)(b >> s);
			}
		}
	}
	
	mx >>= jd->scale;
	if(rx < mx) {
		uint8_t *s, *d;
		unsigned int x, y;

		s = d = (uint8_t *)jd->output_buff;
		for(y = 0; y < ry; ++y) {
			for(x = 0; x < rx; ++x) {
				*d++ = *s++;
				*d++ = *s++;
				*d++ = *s++;
			}
			s += (mx - rx) * 3;
		}
	}
	
	if(!outfunc(jd, jd->output_buff, &rect)) {
		printf("JPEG: outfunc returned 0\n");
		exit(1);
	}
}

#define	LDB_WORD(ptr) (uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))

void jpeg_prep(jpeg_decompr *jd, size_t (*input_func)(jpeg_decompr *, uint8_t *, size_t), void *memory_pool, size_t pool_size, void *device) {
	uint8_t *seg, b;
	uint16_t marker;
	unsigned int n, i, ofs;
	size_t len;

	memset(jd, 0, sizeof(jpeg_decompr));
	jd->memory_pool = memory_pool;
	jd->pool_available = pool_size;
	jd->input_func = input_func;
	jd->io_device = device;

	jd->bit_stream_input = seg = alloc_pool(jd, JPEG_INPUT_BUFFER_SIZE);
	if(seg == NULL) {
		printf("JPEG: Could not allocate input stream\n");
		exit(1);
	}
	ofs = marker = 0;
	do {
		if(jd->input_func(jd, seg, 1) != 1) {
			printf("JPEG: SOI not detected\n");
			exit(1);
		}
		ofs++;
		marker = marker << 8 | seg[0];
	} while(marker != 0xffd8);

	for(;;) {
		if(jd->input_func(jd, seg, 4) != 4) {
			printf("JPEG: input_func did not read\n");
			exit(1);
		}
		marker = LDB_WORD(seg);
		len = LDB_WORD(seg + 2);
		if(len <= 2 || (marker >> 8) != 0xff) {
			printf("JPEG: length seems a bit off or marker is bad\n");
			exit(1);
		}
		len -= 2;
		ofs += 4 + len;

		switch(marker & 0xff) {
			case 0xc0:
				if(len > JPEG_INPUT_BUFFER_SIZE) {
					printf("JPEG: Buffer size too small\n");
					exit(1);
				}
				if(jd->input_func(jd, seg, len) != len) {
					printf("JPEG: input_func did not read 2\n");
					exit(1);
				}
				jd->px_width = LDB_WORD(&seg[3]);
				jd->px_height = LDB_WORD(&seg[1]);
				jd->color_components = seg[5];
				if(jd->color_components != 3 && jd->color_components != 1) {
					printf("JPEG: supports only grayscale and Y/Cb/Cr");
					exit(1);
				}
				for(i = 0; i < jd->color_components; ++i) {
					b = seg[7 + 3*i];
					if(i == 0) {
						if(b != 0x11 && b != 0x22 && b != 0x21) {
							printf("JPEG: supports only 4:4:4, 4:2:0 or 4:2:2\n");
							exit(1);
						}
						jd->block_width = b >> 4;
						jd->block_height = b & 15;
					} else {
						if(b != 0x11) {
							printf("JPEG: sampling factor must be 1");
							exit(1);
						}
					}
					jd->qtab_id[i] = seg[8 + 3 * i];
					if(jd->qtab_id[i] > 3) {
						printf("JPEG: Invalid ID\n");
						exit(1);
					}
				}
				break;

			case 0xDD:
				if(len > JPEG_INPUT_BUFFER_SIZE) {
					printf("JPEG: Buffer size too small 2\n");
					exit(1);
				}
				if(jd->input_func(jd, seg, len) != len) {
					printf("JPEG: input_func did not read 3\n");
					exit(1);
				}
				jd->restart_interval = LDB_WORD(seg);
				break;

			case 0xC4:
				if(len > JPEG_INPUT_BUFFER_SIZE) {
					printf("JPEG: Buffer size too small 3\n");
					exit(1);
				}
				if(jd->input_func(jd, seg, len) != len) {
					printf("JPEG: input_func did not read 4\n");
					exit(1);
				}
				create_huffman_tab(jd, seg, len);
				break;

			case 0xDB:
				if(len > JPEG_INPUT_BUFFER_SIZE) {
					printf("JPEG: Buffer size too small 4\n");
					exit(1);
				}
				if(jd->input_func(jd, seg, len) != len) {
					printf("JPEG: input_func did not read 5\n");
					exit(1);
				}
				create_qtab(jd, seg, len);
				break;

			case 0xDA:
				if(len > JPEG_INPUT_BUFFER_SIZE) {
					printf("JPEG: Buffer size too small 5\n");
					exit(1);
				}
				if(jd->input_func(jd, seg, len) != len) {
					printf("JPEG: input_func did not read 6\n");
					exit(1);
				}
				
				if(!jd->px_width || !jd->px_height) {
					printf("JPEG: Invalid image size\n");
					exit(1);
				}
				if(seg[0] != jd->color_components) {
					printf("JPEG: Wrong color components\n");
					exit(1);
				}

				for(i = 0; i < jd->color_components; ++i) {
					b = seg[2 + 2*i];
					if(b != 0x00 && b != 0x11) {
						printf("JPEG: Wierd table number\n");
						exit(1);
					}
					n = i ? 1 : 0;
					if(!jd->huff_distr[n][0] || !jd->huff_distr[n][1]) {
						printf("JPEG: Huffman table for component not loaded\n");
						exit(1);
					}
					if(!jd->deq_tab[jd->qtab_id[i]]) {
						printf("JPEG: DQ Table for component not loaded\n");
						exit(1);
					}
				}
				
				n = jd->block_height * jd->block_width;
				if(!n) {
					printf("JPEG: SOF0 not loaded\n");
					exit(1);
				}
				len = n * 64 * 2 + 64;
				if(len < 256) len = 256;
				jd->output_buff = alloc_pool(jd, len);
				if(!jd->output_buff) {
					printf("JPEG: Could not allocate output buffer\n");
					exit(1);
				}
				jd->work_buff = alloc_pool(jd, (n + 2) * 64 * sizeof(jpeg_yuv));
				if(!jd->work_buff) {
					printf("JPEG: Could not allocate work buffer\n");
					exit(1);
				}
				if(ofs %= JPEG_INPUT_BUFFER_SIZE) {
					jd->input_buffer_available = jd->input_func(jd, seg + ofs, (size_t)(JPEG_INPUT_BUFFER_SIZE - ofs));
				}
				jd->data_read_ptr = seg + ofs;

				return;

			case 0xc1:
			case 0xc2:
			case 0xc3:
			case 0xc5:
			case 0xc6:
			case 0xc7:
			case 0xc9:
			case 0xca:
			case 0xcb:
			case 0xcc:
			case 0xcd:
			case 0xce:
			case 0xcf:
			case 0xd9:
				printf("JPEG: unsupported standard\n");
				return;

			default:
				if(jd->input_func(jd, NULL, len) != len) {
					printf("JPEG: skipped not enough data\n");
					exit(1);
				}
		}
	}
}

void jpeg_decompress(jpeg_decompr *jd, int (*outfunc)(jpeg_decompr *, void *, jpeg_rect *)) {
	unsigned int x, y, mx, my;
	uint16_t rst, rsc;

	jd->scale = 0;
	mx = jd->block_width * 8;
	my = jd->block_height * 8;

	jd->prev_dc[2] = jd->prev_dc[1] = jd->prev_dc[0] = 0;
	rst = rsc = 0;

	for(y = 0; y < jd->px_height; y += my) {
		for(x = 0; x < jd->px_width; x += mx) {
			if(jd->restart_interval && rst++ == jd->restart_interval) {
				restart(jd, rsc++);
				rst = 1;
			}
			mcu_load(jd);
			mcu_output(jd, outfunc, x, y);
		}
	}
}

