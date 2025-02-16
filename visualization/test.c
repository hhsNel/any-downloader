#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>


#define TO_BIG_ENDIAN(val) \
	(((val) % 256) << 24 | ((val) / 256) % 256 << 16 | ((val) / 65536) % 256 << 8 | ((val) / 16777216) % 256)



int main() {
	struct imgRawImage *img = loadJpegImageFile("../ad-out.jpeg");
	save_ff(img, "test.ff");
	struct imgRawImage *dimg = downscale_image(*img, 256, 192);
	save_ff(dimg, "dtest.ff");
	output_raw_image(*dimg);
}	

