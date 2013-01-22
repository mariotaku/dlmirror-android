#include "android_screencap.h"
#include "tubecable.h"
#include "helper.h"
#include <getopt.h>

#define XRES 1400
#define YRES 1050

inline void show_screen_info();
inline void show_usage(char* pname);
inline void update_screen(usb_dev_handle* handle, dl_cmdstream* cs);
inline void clear_screen(usb_dev_handle* handle, dl_cmdstream* cs);

int main(int argc, char* argv[]) {

	int c;

	while (c != -1) {
		static struct option long_options[] = {
			{"info", no_argument, 0, 'i'}
		};

		int option_index = 0;

		c = getopt_long (argc, argv, "ih", long_options, &option_index);

		if (c == -1) break;

		switch (c) {
			case 'i':
				show_screen_info();
				return 0;
			case 'h':
				return 0;
			case '?':
				show_usage(argv[0]);
				return 1;
		}
	}

	dl_cmdstream cs;
	dl_create_stream(&cs, XRES * YRES * 4);

	usb_dev_handle* handle = dl_get_supported_device_handle();

	if (!handle) {
		printf("Couldn't find supported device; exiting.\n");
		return 1;
	}

	// startup control messages & decompressor table
	dl_init(handle);

	// default register set & offsets
	printf("setting default registers for %dx%d@60Hz..\n",XRES,YRES);
	dl_reg_set_all( &cs, DL_MODE_XY(XRES,YRES));
	dl_reg_set_offsets( &cs, 0x000000, XRES*2, 0x555555, XRES);
	dl_reg_set(&cs, DL_REG_BLANK_SCREEN, 0x00); // enable output
	dl_reg_set(&cs, DL_REG_SYNC, 0xFF);
	dl_cmd_sync(&cs);
	dl_send_command(handle, &cs, 1);

	clear_screen(handle, &cs);
	update_screen(handle, &cs);

	printf("goodbye.\n");
	dl_destroy_stream(&cs);
	usb_close(handle);
	return 0;
}

void clear_screen(usb_dev_handle* handle, dl_cmdstream* cs) {
	dl_rle_word black = { 0x00, 0x0000 };
	int i;
	for (i = 0; i < YRES; i++) {
		int j;
		for (j = 0; j < XRES; j += 256) {
			dl_gfx_rle(cs, i * XRES * 2 + j * 2, 0, &black);
		}
	}
	dl_cmd_sync(cs);
	dl_send_command(handle, cs, 1);
}

void update_screen(usb_dev_handle* handle, dl_cmdstream* cs) {
	screencap_info info = get_screencap_info();
	int w = info.width, h = info.height, f = info.format, rw = h, rh = w;
	int pv = (YRES - rh) / 2, ph = (XRES - rw) / 2;
	int image_size = w * h, bpp = get_byte_per_pixel(f), data_size = image_size * bpp;
	uint32_t* image = (uint32_t*) malloc(data_size);
	uint32_t* pixbuf32 = (uint32_t*) malloc(256 * bpp);
	uint8_t* pixbuf16 = (uint8_t*) malloc(256 * 2);
	while (1) {
		do_screencap(image, data_size);
		int y;
		for (y = 0; y < rh; y++) {
			int x;
			for (x = 0; x < rw; x+= 256) {
				int rem = rw - x, pcount = rem > 256 ? 256 : rem;
				int c;
				for (c = 0; c < pcount; c++) {
					int pixel = image[(c + x) * rh + y];
					pixbuf32[c] = pixel;
				}
				rgba8888_to_rgb565_8(pixbuf32, pixbuf16, pcount);
				int sx = ph + x, sy = YRES - (pv + y);
				int addr = sy * XRES * 2 + sx * 2;
				dl_gfx_write(cs, addr, pcount, pixbuf16);
			}
		}
		dl_cmd_sync(cs);
		dl_send_command(handle, cs, 1);
		printf("drawed 1 frame\n");
	}
	free(image);
}

void show_screen_info() {
	screencap_info info = get_screencap_info();
	printf("Device screen info:\n");
	printf("width: %d, height: %d, format: %d\n", info.width, info.height, info.format);
}

void show_usage(char* pname) {
	printf("Usage: %s [-i | -h]\n", pname);
}
