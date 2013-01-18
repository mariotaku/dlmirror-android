
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "usb.h"

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

// The on-device Huffman table.
extern uint16_t supported_usb_product_ids[6];

/******************** HELPER FUNCTIONS ********************/

// get a device handle according to vendor and product
usb_dev_handle* usb_get_device_handle( int vendor, int product, int interface = 0 );

// convert 24-bit rgb data to 16-bit rgb 565 data.
// host bit order (uint16_t) for compression is the default, data sent 
// to the device from a little-endian machine needs to clear this flag
void rgb24_to_rgb16( uint8_t* rgb24, uint8_t* rgb16, int count, int host_bit_order = 1 );

void rgba32_to_rgb16( uint8_t* rgba32, uint8_t* rgb16, int count, int host_bit_order = 1 );

void screencap_to_rgb16( uint8_t* rgba32, uint8_t* rgb16, int count, int host_bit_order = 1 );

void windows_rgb24_to_rgb16( uint8_t* rgb24, uint8_t* rgb16, int count, int host_bit_order = 1 );

// read raw 24-bit data from a file
void read_rgb24( const char* filename, uint8_t* rgb24, int count );

void read_screencapdata( const char* filename, uint8_t* rgba32, int count );

// read rgb565 data from a 24-bit file. host bit order: see above
uint8_t* read_rgb16( const char* filename, int count, int host_bit_order = 1 );

uint8_t* read_screencap( uint8_t* rgba32, int count, int host_bit_order = 1);

int get_byte_per_pixel(int format);

uint16_t color_rgba8888_to_rgb565(uint8_t* rgba32);
