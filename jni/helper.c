#include "helper.h"
#include "tubecable.h"

#define DISPLAY_LINK_VENDOR_ID 0x17E9
#define HOST_BIT_ORDER 1

uint16_t supported_usb_product_ids[6] = {
	0x01ae, // DL-120
	0x0141, // DL-160
	0x03c1, // DL-165
	0x401a, // Nanovision Mimo
	0x019b, // ForwardVideo
	0x0103, // Samsung U70
};

/******************** HELPER FUNCTIONS ********************/

usb_dev_handle* usb_get_device_handle(int vendor, int product, int interface) {

	usb_init();
	usb_find_busses();
	usb_find_devices();

	struct usb_bus* busses = usb_get_busses();
	struct usb_bus* bus;
	for (bus = busses; bus; bus = bus->next) {
		struct usb_device* dev;
		for (dev = bus->devices; dev; dev = dev->next) {
			if ((dev->descriptor.idVendor == vendor) && (dev->descriptor.idProduct == product)) {
				usb_dev_handle* handle = usb_open(dev);
				if (!handle) return 0;
				if (usb_claim_interface(handle,0) < 0) return 0;
				return handle;
			}
		}
	}
	return 0;
}

usb_dev_handle* dl_get_supported_device_handle() {
	usb_dev_handle* handle = 0;
	int i, size = sizeof(supported_usb_product_ids);
	for (i = 0; i < size; i++) {
		handle = usb_get_device_handle(DISPLAY_LINK_VENDOR_ID, supported_usb_product_ids[i], 0);
		if (handle) break;
	}
	return handle;
}

void rgb888_to_rgb565(uint8_t* rgb888, uint8_t* rgb565, int count) {
	int i;
	for (i = 0; i < count; i++) {
		unsigned int r = rgb888[i * 3 + 0], g = rgb888[i * 3 + 1], b = rgb888[i * 3 + 2];
		rgb565[i * 2 + 0] = (r & 0xF8) | ((g & 0xE0) >> 5);
		rgb565[i * 2 + 1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
	}
}

void rgba8888_to_rgb565_8(uint32_t* rgba8888, uint8_t* rgb565, int count) {
	int i;
	for (i = 0; i < count; i++) {
		uint32_t pixel = rgba8888[i];
		unsigned int r = pixel & 0xFF, g = (pixel >> 8) & 0xFF, b = (pixel >> 16) & 0xFF;
		rgb565[i * 2 + 0] = (r & 0xF8) | ((g & 0xE0) >> 5);
		rgb565[i * 2 + 1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
	}
}

void rgba8888_to_rgb565_16(uint32_t* rgba8888, uint16_t* rgb565, int count) {
	int i;
	for (i = 0; i < count; i++) {
		rgb565[i] = color_rgba8888_to_rgb565(rgba8888[i]);
	}
}

uint16_t color_rgba8888_to_rgb565(uint32_t rgba32) {
	if (sizeof(rgba32) != 4) return 0;
	unsigned int r = rgba32 & 0xFF, g = (rgba32 >> 8) & 0xFF, b = (rgba32 >> 16) & 0xFF;
	return (r >> 3 << 11) | (g >> 2 << 5) | b >> 3;
}

void scale_rgba8888(uint32_t* in, uint32_t* out, int w, int h, int scale) {
	int i, count = w * h;
	for (i = 0; i < count; i+= scale) {
		int x = i / h;
		if (x % scale != 0) continue;
		int y = i % h;
		out[w * y / (scale * scale) + x / scale] = in[i];
	}
}

void rotate_scale_rgba8888(uint32_t* in, uint32_t* out, int w, int h, int scale) {
	int i, count = w * h;
	int rw = h, rh = w;
	for (i = 0; i < count; i+= scale) {
		int rx = i / rh;
		if (rx % scale != 0) continue;
		int ry = i % rh;
		out[rw * ry / (scale * scale) + rx / scale] = in[i];
	}
}
