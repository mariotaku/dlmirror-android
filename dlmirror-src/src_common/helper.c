#include "helper.h"
#include "tubecable.h"

#include "screencap_flinger.h"

#define HOST_BIT_ORDER 1

/******************** HELPER FUNCTIONS ********************/

void screencap_getdata_rgbax8888(uint8_t* data, int count) {
	uint32_t *rgbax8888 = (uint32_t*) screencap_getdata();
	int i;
	for (i = 0; i < count; i++) {
		uint32_t pixel = rgbax8888[i];
		unsigned int r = pixel & 0xFF, g = (pixel >> 8) & 0xFF, b = (pixel >> 16) & 0xFF;
		data[i * 2 + 0] = (r & 0xF8) | ((g & 0xE0) >> 5);
		data[i * 2 + 1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
	}
}

void screencap_getdata_rgb888(uint8_t* data, int count) {
	uint8_t *rgb888 = (uint8_t*) screencap_getdata();
	int i;
	for (i = 0; i < count; i++) {
		unsigned int r = rgb888[i * 3], g = rgb888[i * 3 + 1], b = rgb888[r * 3 + 2];
		data[i * 2 + 0] = (r & 0xF8) | ((g & 0xE0) >> 5);
		data[i * 2 + 1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
	}
}

void screencap_getdata_bgra8888(uint8_t* data, int count) {
	uint32_t *bgra8888 = (uint32_t*) screencap_getdata();
	int i;
	for (i = 0; i < count; i++) {
		uint32_t pixel = bgra8888[i];
		unsigned int b = pixel & 0xFF, g = (pixel >> 8) & 0xFF, r = (pixel >> 16) & 0xFF;
		data[i * 2 + 0] = (r & 0xF8) | ((g & 0xE0) >> 5);
		data[i * 2 + 1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
	}
}

void rotate_bitmap8(uint8_t *in, uint8_t *out, int w, int h, int rotate) {
	int cos, sin;
	switch (rotate) {
		case 0:
			cos = 1;
			sin = 0;
			break;
		case 90:
			cos = 0;
			sin = 1;
			break;
		case 180:
			cos = -1;
			sin = 0;
			break;
		case 270:
			cos = -1;
			sin = 0;
			break;
		default:
			//show some errors here.
			return;
	}
	int y, xr, yr;
	for (y = 0; y <= h; y++) {
		xr = (- w / 2) * cos + (h / 2) * sin + w / 2;
		yr = (h / 2) * cos - (-w / 2) * sin + h / 2;
		int x;
		for (x = 0; x <= w; x++) {
			out[yr * w + xr] = in[y * h + x];
			xr += cos;
			yr -= sin;
		}
	}
}
