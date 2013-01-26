
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "usb.h"

// The on-device Huffman table.
extern uint16_t supported_usb_product_ids[6];

/******************** HELPER FUNCTIONS ********************/

// get a device handle according to vendor and product
usb_dev_handle* usb_get_device_handle(int vendor, int product, int interface);

usb_dev_handle* dl_get_supported_device_handle();

// convert 24-bit rgb data to 16-bit rgb 565 data.
// host bit order (uint16_t) for compression is the default, data sent
// to the device from a little-endian machine needs to clear this flag
void rgb888_to_rgb565(uint8_t* rgb888, uint8_t* rgb565, int count);

void rgba8888_to_rgb565_8(uint32_t* rgba8888, uint8_t* rgb565, int count);

void rgba8888_to_rgb565_16(uint32_t* rgba8888, uint16_t* rgb565, int count);

uint16_t color_rgba8888_to_rgb565(uint32_t rgba32);

void scale_rgba8888(uint32_t* in, uint32_t* out, int w, int h, int scale);

void rotate_scale_rgba8888(uint32_t* in, uint32_t* out, int w, int h, int scale);

void rotate_bitmap32(uint32_t *in, uint32_t *out, int w, int h, int rotate);
