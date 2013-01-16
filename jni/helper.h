
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "usb.h"

/******************** HELPER FUNCTIONS ********************/

// get a device handle according to vendor and product
usb_dev_handle* usb_get_device_handle( int vendor, int product, int interface = 0 );

// convert 24-bit rgb data to 16-bit rgb 565 data.
// host bit order (uint16_t) for compression is the default, data sent 
// to the device from a little-endian machine needs to clear this flag
void rgb24_to_rgb16( uint8_t* rgb24, uint8_t* rgb16, int count, int host_bit_order = 1 );

// read raw 24-bit data from a file
void read_rgb24( const char* filename, uint8_t* rgb24, int count );

// read rgb565 data from a 24-bit file. host bit order: see above
uint8_t* read_rgb16( const char* filename, int count, int host_bit_order = 1 );

