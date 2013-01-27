#include <binder/IMemory.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>
#include <private/gui/ComposerService.h>

#include "screencap_flinger.h"

using namespace android;

static uint32_t DEFAULT_DISPLAY_ID = ISurfaceComposer::eDisplayIdMain;

inline int get_byte_per_pixel(int format);
inline int update(uint32_t *w, uint32_t *h, int *f, uint32_t req_w, uint32_t req_h);

sp<IBinder> display;
sp<IMemoryHeap> heap;
sp<ISurfaceComposer> service;
uint32_t width = 0, height = 0, req_width = 0, req_height = 0;
int format = PIXEL_FORMAT_NONE;

extern "C" int screencap_init() {
	display = SurfaceComposerClient::getBuiltInDisplay(DEFAULT_DISPLAY_ID);
	service = ComposerService::getComposerService();
	return display != NULL && service != NULL;
}

extern "C" void const * screencap_getdata() {
	if (!update(&width, &height, &format, req_width, req_height)) return heap->getBase();
	return 0;
}
	
extern "C" void screencap_release() {
	heap = 0;
}
	
extern "C" screencap_info screencap_getinfo() {
	uint32_t w, h;
	int f;
	update(&w, &h, &f, req_width, req_height);
	screencap_info info = {w, h, f, get_byte_per_pixel(f)};
	return info;
}
	
extern "C" size_t screencap_size() {
	return heap->getSize();
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
	return service->captureScreen(display, &heap, w, h, f, req_w, req_h, 0, -1UL);
}
