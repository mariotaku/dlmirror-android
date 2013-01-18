/*
 * libtubecable - displaylink protocol reference implementation
 *
 * version 0.1.2 - more efficient Huffman table by Henrik Pedersen
 *                 fixed two more encoder glitches
 *                 June 6th, 2009
 *
 * version 0.1.1 - added missing Huffman sequences
 *                 fixed 2 bugs in encoder 
 *                 June 5th, 2009
 *
 * version 0.1   - initial public release
 *                 May 30th, 2009
 *
 * written 2008/09 by floe at butterbrot.org
 * in cooperation with chrisly at platon42.de
 * this code is released as public domain.
 *
 * this is so experimental that the warranty shot itself.
 * so don't expect any.
 *
 */

#include "tubecable.h"
#include "helper.h"

#define XRES 1280
#define YRES 1024

inline void write_pixel(dl_cmdstream* cs, int i, int j, dl_rle_word* color);
inline void update_screen(usb_dev_handle* handle);

int main2(int argc, char* argv[]) {
	FILE* stream = popen("screencap", "r");
	uint16_t* header = (uint16_t*) malloc(SCREENCAP_HEADER_SIZE);
	fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
	int w = header[SCREENCAP_HEADER_INDEX_WIDTH];
	int h = header[SCREENCAP_HEADER_INDEX_HEIGHT];
	int f = header[SCREENCAP_HEADER_INDEX_FORMAT];
	int bpp = get_byte_per_pixel(f);
	int data_size = w * h * bpp;
	uint8_t* image_data = (uint8_t*) malloc(data_size);
	while (1) {
		stream = popen("screencap", "r");
		fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
		fread(image_data, data_size, 1, stream);
		struct timeval  tv;
		gettimeofday(&tv, NULL);
		printf("time: %d, width: %d, height: %d, bpp: %d\n", (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000, w, h, bpp);
		pclose(stream);
	}
	return 0;
}

int main(int argc, char* argv[]) {

	printf("\ndisplaylink userspace controller demo 0.1\n\n");
	printf("written 2008/09 by floe at butterbrot.org\n");
	printf("in cooperation with chrisly at platon42.de\n");
 	printf("this code is released as public domain.\n\n");
	printf("this is so experimental that the warranty shot itself.\n");
	printf("so don't expect any.\n\n");


	dl_cmdstream cs;
	dl_create_stream( &cs, 1024 * 1024);

	// load huffman table
	dl_huffman_load_table( "tubecable_huffman.bin" );

	usb_dev_handle* handle;

	for (int i = 0; i < sizeof(supported_usb_product_ids); i++) {
		handle = usb_get_device_handle( 0x17E9, supported_usb_product_ids[i]);
		if (handle) break;
	}

	if (!handle) {
		printf("Couldn't find support device; exiting.\n");
		return 1;
	}

	// startup control messages & decompressor table
	dl_init( handle );

	// default register set & offsets
	printf("setting default registers for %dx%d@60Hz..\n",XRES,YRES);
	dl_reg_set_all( &cs, DL_MODE_XY(XRES,YRES) );
	dl_reg_set_offsets( &cs, 0x000000, XRES*2, 0x555555, XRES );
	dl_reg_set( &cs, DL_REG_BLANK_SCREEN, 0x00 ); // enable output
	dl_reg_set( &cs, DL_REG_SYNC, 0xFF );
	dl_cmd_sync( &cs );
	dl_send_command( handle, &cs );

	sleep(1);

	update_screen(handle);
	
	printf("goodbye.\n");
	dl_destroy_stream(&cs);
	usb_close(handle);
}

void write_pixel(dl_cmdstream* cs, int y, int x, dl_rle_word* color) {
	dl_gfx_rle(cs, y * XRES * 2 + x * 2, 1, color );
}

void update_screen(usb_dev_handle* handle) {
	dl_cmdstream cs;
	dl_rle_word color = { 0x00, 0x0000 };
	FILE* stream = popen("screencap", "r");
	uint16_t* header = (uint16_t*) malloc(SCREENCAP_HEADER_SIZE);
	fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
	int w = header[SCREENCAP_HEADER_INDEX_WIDTH];
	int h = header[SCREENCAP_HEADER_INDEX_HEIGHT];
	int f = header[SCREENCAP_HEADER_INDEX_FORMAT];
	int screen_size = w * h;
	int bpp = get_byte_per_pixel(f);
	int data_size = screen_size * bpp;
	uint8_t* pixel = (uint8_t*) malloc(bpp);
	while (1) {
		stream = popen("screencap", "r");
		fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
		dl_create_stream( &cs, XRES * YRES * 16);
		for (int i = 0; i < screen_size; i++) {
			int y = YRES - i % w;
			int x = i / w;
			fread(pixel, bpp, 1, stream);
			color.value = color_rgba8888_to_rgb565(pixel);
			write_pixel(&cs, y, x, &color);
		}
		dl_cmd_sync(&cs);
		dl_send_command(handle, &cs);
		dl_destroy_stream(&cs);
		pclose(stream);
	}
}
