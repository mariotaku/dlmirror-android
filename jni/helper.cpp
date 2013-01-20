#include "helper.h"
#include "tubecable.h"

#define DISPLAY_LINK_VENDOR_ID 0x17E9

uint16_t supported_usb_product_ids[6] = {
	0x01ae, // DL-120
	0x0141, // DL-160
	0x03c1, // DL-165
	0x401a, // Nanovision Mimo
	0x019b, // ForwardVideo
	0x0103, // Samsung U70
};

/******************** HELPER FUNCTIONS ********************/

usb_dev_handle* usb_get_device_handle( int vendor, int product, int interface ) {

	usb_init();
	usb_find_busses();
	usb_find_devices();

	struct usb_bus* busses = usb_get_busses();

	for ( struct usb_bus* bus = busses; bus; bus = bus->next ) {
		for ( struct usb_device* dev = bus->devices; dev; dev = dev->next ) {
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
	for (unsigned int i = 0; i < sizeof(supported_usb_product_ids); i++) {
		handle = usb_get_device_handle(DISPLAY_LINK_VENDOR_ID, supported_usb_product_ids[i]);
		if (handle) break;
	}
	return handle;
}

void rgb24_to_rgb16( uint8_t* rgb24, uint8_t* rgb16, int count, int host_bit_order ) {
	for (int i = 0; i < count; i++) {
		uint8_t r = rgb24[i*3+0];
		uint8_t g = rgb24[i*3+1];
		uint8_t b = rgb24[i*3+2];
		if (host_bit_order) {
			rgb16[i*2+1] =  (r & 0xF8)       | ((g & 0xE0) >> 5);
			rgb16[i*2+0] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
		} else {
			rgb16[i*2+0] =  (r & 0xF8)       | ((g & 0xE0) >> 5);
			rgb16[i*2+1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
		}
	}
}

void rgba32_to_rgb16( uint8_t* rgba32, uint8_t* rgb16, int count, int host_bit_order ) {
	for (int i = 0; i < count; i++) {
		uint8_t r = rgba32[i*3+0];
		uint8_t g = rgba32[i*3+1];
		uint8_t b = rgba32[i*3+2];
		if (host_bit_order) {
			rgb16[i*2+1] =  (r & 0xF8)       | ((g & 0xE0) >> 5);
			rgb16[i*2+0] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
		} else {
			rgb16[i*2+0] =  (r & 0xF8)       | ((g & 0xE0) >> 5);
			rgb16[i*2+1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
		}
	}
}

void screencap_to_rgb16( uint32_t* rgba32, uint16_t* rgb16, int count, int host_bit_order ) {
	for (int i = 0; i < count; i++) {
		uint32_t pixel = rgba32[i]; 
		unsigned int r = pixel & 0xFF, g = (pixel >> 8) & 0xFF, b = (pixel >> 16) & 0xFF;
		if (host_bit_order) {
			rgb16[i*2+1] =  (r & 0xF8)       | ((g & 0xE0) >> 5);
			rgb16[i*2+0] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
		} else {
			rgb16[i*2+0] =  (r & 0xF8)       | ((g & 0xE0) >> 5);
			rgb16[i*2+1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
		}
	}
}

void windows_rgb24_to_rgb16( uint8_t* rgb24, uint8_t* rgb16, int count, int host_bit_order ) {
	for (int i = 0; i < count; i++) {
		uint8_t g = rgb24[(count - i) * 3 - 2];
		uint8_t r = rgb24[(count - i) * 3 - 1];
		uint8_t b = rgb24[(count - i) * 3 - 0];
		if (host_bit_order) {
			rgb16[i*2+1] =  (r & 0xF8)       | ((g & 0xE0) >> 5);
			rgb16[i*2+0] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
		} else {
			rgb16[i*2+0] =  (r & 0xF8)       | ((g & 0xE0) >> 5);
			rgb16[i*2+1] = ((g & 0x1C) << 3) | ((b & 0xF8) >> 3);
		}
	}
}

void read_rgb24( const char* filename, uint8_t* rgb24, int count ) {
	int f = open(filename,O_RDONLY);
	lseek( f, 54, SEEK_SET );
	read( f, rgb24, count*3 );
	close(f);
}

uint8_t* read_rgb16( const char* filename, int count, int host_bit_order ) {

	uint8_t* rgb16 = (uint8_t*)malloc(count*2);
	uint8_t* rgb24 = (uint8_t*)malloc(count*3);

	read_rgb24(filename,rgb24,count);
	windows_rgb24_to_rgb16(rgb24,rgb16,count,host_bit_order);

	free( rgb24 );
	return rgb16;
}

uint16_t color_rgba8888_to_rgb565(uint32_t rgba32) {
	if (sizeof(rgba32) != 4) return 0;
	unsigned int r = rgba32 & 0xFF, g = (rgba32 >> 8) & 0xFF, b = (rgba32 >> 16) & 0xFF;
	return (r >> 3 << 11) | (g >> 2 << 5) | b >> 3;
}

void scale_rgba8888(uint32_t* in, uint32_t* out, int w, int h, int scale) {
	int count = w * h;
	for (int i = 0; i < count; i+= scale) {
		int x = i / h;
		if (x % scale != 0) continue;
		int y = i % h;
		out[w * y / (scale * scale) + x / scale] = in[i];
	}
}

void rotate_scale_rgba8888(uint32_t* in, uint32_t* out, int w, int h, int scale) {
	int count = w * h;
	int rw = h, rh = w;
	for (int i = 0; i < count; i+= scale) {
		int rx = i / rh;
		if (rx % scale != 0) continue;
		int ry = i % rh;
		out[rw * ry / (scale * scale) + rx / scale] = in[i];
	}
}
