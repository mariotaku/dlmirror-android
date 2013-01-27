
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "usb.h"

// The on-device Huffman table.
extern uint16_t supported_usb_product_ids[8];

/******************** HELPER FUNCTIONS ********************/

// get a device handle according to vendor and product
usb_dev_handle* usb_get_device_handle(int vendor, int product, int interface);

usb_dev_handle* dl_get_supported_device_handle();

void rotate_bitmap8(uint8_t *in, uint8_t *out, int w, int h, int rotate);

void screencap_getdata_rgbax8888(uint8_t* data, int count);
void screencap_getdata_bgra8888(uint8_t* data, int count);
void screencap_getdata_rgb888(uint8_t* data, int count);
