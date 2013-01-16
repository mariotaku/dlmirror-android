#include "helper.h"
#include "tubecable.h"

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

void read_rgb24( const char* filename, uint8_t* rgb24, int count ) {
	int f = open(filename,O_RDONLY);
	lseek( f, 15, SEEK_SET );
	read( f, rgb24, count*3 );
	close(f);
}

uint8_t* read_rgb16( const char* filename, int count, int host_bit_order ) {

	uint8_t* rgb16 = (uint8_t*)malloc(count*2);
	uint8_t* rgb24 = (uint8_t*)malloc(count*3);

	read_rgb24(filename,rgb24,count);
	rgb24_to_rgb16(rgb24,rgb16,count,host_bit_order);

	free( rgb24 );
	return rgb16;
}

