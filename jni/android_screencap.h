#include <stdio.h>

#define PIXEL_FORMAT_RGBA_8888 1
#define PIXEL_FORMAT_RGBX_8888 2
#define PIXEL_FORMAT_RGB_888 3
#define PIXEL_FORMAT_RGB_565 4
#define PIXEL_FORMAT_BGRA_8888 5
#define PIXEL_FORMAT_RGBA_5551 6
#define PIXEL_FORMAT_RGBA_4444 7

#define SCREENCAP_HEADER_SIZE 12
#define SCREENCAP_HEADER_INDEX_WIDTH 0
#define SCREENCAP_HEADER_INDEX_HEIGHT 2
#define SCREENCAP_HEADER_INDEX_FORMAT 4

typedef struct {
	int width;
	int height;
	int format;
} screencap_info;

screencap_info get_screencap_info();

void do_screencap(uint32_t* buffer, int data_size);

int get_byte_per_pixel(int format);
