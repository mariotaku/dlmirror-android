#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include <binder/IMemory.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>
#include <utils/Errors.h>

#include "screencap_flinger.hpp"


using namespace android;

static uint32_t DEFAULT_DISPLAY_ID = ISurfaceComposer::eDisplayIdMain;

sp<IBinder> *display = NULL;
ScreenshotClient *screenshot;

extern "C" int screencap_init() {
	*display = SurfaceComposerClient::getBuiltInDisplay(DEFAULT_DISPLAY_ID);
	screenshot = new ScreenshotClient();
	int errno;
	errno = screenshot->update(*display);
	if (!screenshot->getPixels()) return -1;
	if (errno != 0) return -1;
	return 0;
}

extern "C" void const * screencap_getdata() {
	screenshot->update(*display);
	return screenshot->getPixels();
}
	
extern "C" void screencap_release() {
	screenshot->release();
	free(screenshot);
	free(display);
}
	
extern "C" uint32_t screencap_width() {
	return screenshot->getWidth();
}
	
extern "C" uint32_t screencap_height() {
	return screenshot->getHeight();
}
	
extern "C" int screencap_format() {
	return screenshot->getFormat();
}
	
extern "C" size_t screencap_size() {
	return screenshot->getSize();
}
