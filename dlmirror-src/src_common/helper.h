
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "usb.h"

/******************** HELPER FUNCTIONS ********************/

void rotate_bitmap8(uint8_t *in, uint8_t *out, int w, int h, int rotate);

void screencap_getdata_rgbax8888(uint8_t* data, int count);
void screencap_getdata_bgra8888(uint8_t* data, int count);
void screencap_getdata_rgb888(uint8_t* data, int count);
