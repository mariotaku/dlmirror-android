#include "helper.h"
#include "android_screencap.h"

#ifdef ANDROID
	#define SCREENCAP_COMMAND "/system/bin/screencap"
#else
	#define SCREENCAP_COMMAND "adb shell screencap | sed 's/\r$//'"
#endif

screencap_info get_screencap_info() {
	FILE* stream = popen(SCREENCAP_COMMAND, "r");
	uint16_t* header = (uint16_t*) malloc(SCREENCAP_HEADER_SIZE);
	fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
	screencap_info info = {0, 0, 0};
	info.width = header[SCREENCAP_HEADER_INDEX_WIDTH];
	info.height = header[SCREENCAP_HEADER_INDEX_HEIGHT];
	info.format = header[SCREENCAP_HEADER_INDEX_FORMAT];
	free(header);
	pclose(stream);
	return info;
}

void do_screencap(uint32_t* buffer, int data_size) {
	FILE* stream = popen(SCREENCAP_COMMAND, "r");
	uint16_t* header = (uint16_t*) malloc(SCREENCAP_HEADER_SIZE);
	// Skip first 12 bytes data.
	fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
	fread(buffer, data_size, 1, stream);
	free(header);
	pclose(stream);
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
