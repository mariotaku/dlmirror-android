#include "helper.h"
#include "tubecable.h"

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

void screencap_to_rgb16( uint8_t* rgba32, uint8_t* rgb16, int count, int host_bit_order ) {
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

void do_screencap(uint8_t* rgba32, int count ) {
	FILE* data = popen("screencap", "r");
	pclose(data);
}

uint8_t* read_screencap( uint8_t* rgba32, int count, int host_bit_order ) {

	uint8_t* rgb16 = (uint8_t*)malloc(count*2);
	screencap_to_rgb16(rgba32,rgb16,count,host_bit_order);

	free(rgba32);
	return rgb16;
}

uint8_t* read_rgb16( const char* filename, int count, int host_bit_order ) {

	uint8_t* rgb16 = (uint8_t*)malloc(count*2);
	uint8_t* rgb24 = (uint8_t*)malloc(count*3);

	read_rgb24(filename,rgb24,count);
	windows_rgb24_to_rgb16(rgb24,rgb16,count,host_bit_order);

	free( rgb24 );
	return rgb16;
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

uint16_t color_rgba8888_to_rgb565(uint8_t* rgba32) {
	if (sizeof(rgba32) != 4) return 0;
	return (rgba32[0] << 11) | (rgba32[1] << 5) | rgba32[2];
}
