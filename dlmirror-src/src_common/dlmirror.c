#include <getopt.h>
#include <pthread.h>

#include "screencap_flinger.h"
#include "tubecable.h"
#include "helper.h"

#define MULTI_THREAD_OPTION_DEFAULT 0
#define TRUE_COLOR_OPTION_DEFAULT 0

#define XRES 1400
#define YRES 1050

inline void show_screen_info();
inline void show_usage(char *pname);
inline void show_help(char *pname);
inline void update_screen(usb_dev_handle *handle, dl_cmdstream *cs);
inline void clear_screen(usb_dev_handle *handle, dl_cmdstream *cs);
inline void *screencap_task(void *args);
inline void do_screencap();

static uint8_t *rgb565, *rgb323 = 0;
static screencap_info screen_info;
static int image_size, image_format;
static int multi_thread = MULTI_THREAD_OPTION_DEFAULT, true_color = TRUE_COLOR_OPTION_DEFAULT;

int main(int argc, char *argv[]) {

	if (!screencap_init()) {
		fputs("Couldn't initialize screencap method!\n", stderr);
		return 1;
	}

	char *pname = argv[0];
	int c;

	while (c != -1) {
		static struct option long_options[] = {
			{"multi-thread", no_argument, 0, 'm'},
			{"info", no_argument, 0, 'i'},
			{"help", no_argument, 0, 'h'},
			{"scale", no_argument, 0, 's'},
			{"rotate", no_argument, 0, 'r'},
			{"true-color", no_argument, 0, 't'}
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "hsrtmi", long_options, &option_index);

		if (c == -1) break;

		switch (c) {
			case 'm':
				multi_thread = 1;
				break;
			case 's':
				screencap_request_size(400, 640);
				break;
			case 't':
				true_color = 1;
				break;
			case 'i':
				show_screen_info();
				screencap_release();
				return 0;
			case 'h':
				show_help(pname);
				screencap_release();
				return 0;
			case '?':
				show_usage(pname);
				screencap_release();
				return 1;
		}
	}

	screen_info = screencap_getinfo();
	image_format = screen_info.format;
	image_size = screen_info.width * screen_info.height;
	rgb565 = (uint8_t*) malloc(image_size * 2);
	if (true_color) {
		rgb323 = (uint8_t*) malloc(image_size);
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
	dl_reg_set_all(&cs, DL_MODE_XY(XRES,YRES));
	dl_reg_set_offsets(&cs, 0x000000, XRES * 2, XRES * YRES * 2, XRES);
	dl_reg_set(&cs, DL_REG_BLANK_SCREEN, 0x00); // enable output
	dl_reg_set(&cs, DL_REG_SYNC, 0xFF);
	dl_cmd_sync(&cs);
	dl_send_command(handle, &cs, 1);

	clear_screen(handle, &cs);
	update_screen(handle, &cs);

	free(rgb565);
	if (rgb323) {
		free(rgb323);
	}
	screencap_release();
	
	printf("Disconnecting device...\n");
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
	int w = screen_info.width, h = screen_info.height, rw = h, rh = w;
	int pv = (YRES - rh) / 2, ph = (XRES - rw) / 2;
	uint8_t *pixbuf565 = (uint8_t*) malloc(256 * 2), *pixbuf323 = (uint8_t*) malloc(256);
	if (multi_thread) {
		pthread_t thread;
		pthread_create(&thread, NULL, screencap_task, NULL);
	}
	while (1) {
		if (!multi_thread) {
			do_screencap();
		}
		int y;
		for (y = 0; y < rh; y++) {
			int x;
			for (x = 0; x < rw; x+= 256) {
				int rem = rw - x, pcount = rem > 256 ? 256 : rem;
				int c;
				for (c = 0; c < pcount; c++) {
					int idx = (c + x) * rh + y;
					pixbuf565[c * 2] = rgb565[idx * 2];
					pixbuf565[c * 2 + 1] = rgb565[idx * 2 + 1];
					if (rgb323) {
						pixbuf323[c] = rgb323[idx];
					}
				}
				int sx = ph + x, sy = YRES - (pv + y);
				int addr565 = sy * XRES * 2 + sx * 2;
				dl_gfx_write(cs, addr565, pcount, pixbuf565);
				if (rgb323) {
					int addr323 = XRES * YRES * 2 + sy * XRES + sx;
					dl_gfx_write(cs, addr323, pcount, pixbuf323);
				}
			}
		}
		dl_cmd_sync(cs);
		int ret = dl_send_command(handle, cs, 1);
		if (ret < 0) {
			fputs("USB disconnected.\n", stderr);
			break;
		}
	}
	free(pixbuf565);
	free(pixbuf323);
}

void show_screen_info() {
	screencap_info info = screencap_getinfo();
	printf("Device screen info:\n");
	printf("width: %d, height: %d, format: %d, bytepp: %d\n", info.width, info.height, info.format, info.bytepp);
}

void show_usage(char* pname) {
	printf("Usage: %s [-i | -h | -t | -s | -r | -m]\n", pname);
}

void show_help(char* pname) {
	show_usage(pname);
	printf(" -m, --multi-thread\t\tuse different thread for screencap\n");
	printf(" -t, --true-color\t\tenable 24-bit color mode\n");
	printf(" -i, --info\t\t\tprint technical info\n");
	printf(" -h, --help\t\t\tprint this help\n");
}

void *screencap_task(void *args) {
	while (1) {
		do_screencap();
	}
}

void do_screencap() {
	switch (image_format) {
		case PIXEL_FORMAT_RGBA_8888:
		case PIXEL_FORMAT_RGBX_8888:
			screencap_getdata_rgbax8888(rgb565, rgb323, image_size);
			break;
		case PIXEL_FORMAT_BGRA_8888:
			screencap_getdata_bgra8888(rgb565, rgb323, image_size);
			break;
		case PIXEL_FORMAT_RGB_888:
			screencap_getdata_rgb888(rgb565, rgb323, image_size);
			break;
	}
}
