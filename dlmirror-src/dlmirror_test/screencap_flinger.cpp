#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <fcntl.h>
#include "screencap_flinger.h"

#define SCREENCAP_COMMAND "adb shell /system/bin/screencap | sed 's/\r$//g'"

#define SCREENCAP_HEADER_SIZE 12
#define SCREENCAP_HEADER_IDX_WIDTH 0
#define SCREENCAP_HEADER_IDX_HEIGHT 1
#define SCREENCAP_HEADER_IDX_FORMAT 2

inline int get_byte_per_pixel(int format);
inline int update(uint32_t *w, uint32_t *h, int *f, uint32_t req_w, uint32_t req_h);

uint32_t width = 0, height = 0, req_width = 0, req_height = 0;
void const *heap;
uint32_t *header;
int format = PIXEL_FORMAT_NONE, heap_size;

extern "C" int screencap_init() {
	FILE *stream = popen(SCREENCAP_COMMAND, "r");
	header = (uint32_t*) malloc(SCREENCAP_HEADER_SIZE);
	fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
	width = header[SCREENCAP_HEADER_IDX_WIDTH];
	height = header[SCREENCAP_HEADER_IDX_HEIGHT];
	format = header[SCREENCAP_HEADER_IDX_FORMAT];
	heap_size = width * height * get_byte_per_pixel(format);
	heap = (void const*) malloc(heap_size);
	pclose(stream);
	return heap_size > 0;
}

extern "C" void const * screencap_getdata() {
	update(&width, &height, &format, req_width, req_height);	
	return heap;
}
	
extern "C" void screencap_release() {
	free((void *)heap);
	free(header);
}
	
extern "C" screencap_info screencap_getinfo() {
	uint32_t w, h;
	int f;
	update(&w, &h, &f, req_width, req_height);
	screencap_info info = {w, h, f, get_byte_per_pixel(f)};
	return info;
}
	
extern "C" size_t screencap_size() {
	return heap_size;
}

extern "C" void screencap_request_size(uint32_t req_w, uint32_t req_h) {
	if ((req_w == 0 && req_h != 0) || (req_w != 0 && req_h == 0)) return;
	req_width = req_w;
	req_height = req_h;
}

int get_byte_per_pixel(int format) {
	switch (format) {
		case PIXEL_FORMAT_BGRA_8888:
		case PIXEL_FORMAT_RGBA_8888:
		case PIXEL_FORMAT_RGBX_8888:
			return 4;
		case PIXEL_FORMAT_RGB_888:
			return 3;
		case PIXEL_FORMAT_RGBA_4444:
		case PIXEL_FORMAT_RGBA_5551:
		case PIXEL_FORMAT_RGB_565:
			return 2;
		default:
			return 0;
	}
}

int update(uint32_t *w, uint32_t *h, int *f, uint32_t req_w, uint32_t req_h) {
	FILE *stream = popen(SCREENCAP_COMMAND, "r");
	fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
	*w = header[SCREENCAP_HEADER_IDX_WIDTH];
	*h = header[SCREENCAP_HEADER_IDX_HEIGHT];
	*f = header[SCREENCAP_HEADER_IDX_FORMAT];
	fread((void *)heap, heap_size, 1, stream);
	pclose(stream);
	return 0;
}
