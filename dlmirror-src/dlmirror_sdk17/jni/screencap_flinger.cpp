#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include <binder/IMemory.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>
#include <ui/PixelFormat.h>
#include <utils/Errors.h>
#include <private/gui/ComposerService.h>

#include "screencap_flinger.h"


using namespace android;

static uint32_t DEFAULT_DISPLAY_ID = ISurfaceComposer::eDisplayIdMain;

inline int get_byte_per_pixel(int format);
inline int update();

sp<IBinder> display;
sp<IMemoryHeap> heap;
sp<ISurfaceComposer> service;
uint32_t width;
uint32_t height;
PixelFormat format;

extern "C" int screencap_init() {
	display = SurfaceComposerClient::getBuiltInDisplay(DEFAULT_DISPLAY_ID);
	service = ComposerService::getComposerService();
	width = 0;
	height = 0;
	format = PIXEL_FORMAT_NONE;
	return display != NULL && service != NULL;
}

extern "C" void const * screencap_getdata() {
	if (!update()) return heap->getBase();
	return 0;
}
	
extern "C" void screencap_release() {
	heap = 0;
}
	
extern "C" screencap_info screencap_getinfo() {
	update();
	screencap_info info = {width, height, format, get_byte_per_pixel(format)};
	return info;
}
	
extern "C" size_t screencap_size() {
	return heap->getSize();
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

int update() {
	return service->captureScreen(display, &heap, &width, &height, &format, 0, 0, 0, -1UL);
}
