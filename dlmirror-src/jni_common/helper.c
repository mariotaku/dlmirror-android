#include "helper.h"
#include "tubecable.h"

#include "screencap_flinger.h"

#define DISPLAY_LINK_VENDOR_ID 0x17E9
#define HOST_BIT_ORDER 1

uint16_t supported_usb_product_ids[8] = {
	0x01ae, // DL-120 Series
	0x0141, // DL-160 Series
	0x03c1, // DL-165 Series
	0x01e2, // DL-195 Series
	0x401a, // Nanovision Mimo
	0x019b, // ForwardVideo
	0x0103, // Samsung U70
	0x016b // Green House GH-USD16
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
		unsigned int r = i * 3, g = i * 3 + 1, b = r * 3 + 2;
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
