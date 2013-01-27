#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
	uint32_t width;
	uint32_t height;
	int format;
	int bytepp;
} screencap_info;

int screencap_init();
void screencap_release();
void const *screencap_getdata();
screencap_info screencap_getinfo();
size_t screencap_getsize();

#ifdef __cplusplus
}

#endif
