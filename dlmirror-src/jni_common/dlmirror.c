#include <getopt.h>
#include <pthread.h>

#include "screencap_flinger.h"
#include "tubecable.h"
#include "helper.h"

#define PTHREAD_OPTION_DEFAULT 0

#define XRES 1400
#define YRES 1050

inline void show_screen_info();
inline void show_usage(char *pname);
inline void show_help(char *pname);
inline void update_screen(usb_dev_handle *handle, dl_cmdstream *cs);
inline void clear_screen(usb_dev_handle *handle, dl_cmdstream *cs);
inline void *screencap_task(void *args);

static uint8_t *image;
static screencap_info info;
static int image_size, data_size;
static int use_pthread = PTHREAD_OPTION_DEFAULT;

int main(int argc, char *argv[]) {

	int screencap_ret = screencap_init();
	if (!screencap_ret) {
		fputs("Couldn't initialize screencap method!\n", stderr);
		return 1;
	}
	info = screencap_getinfo();
	image_size = info.width * info.height;
	data_size = image_size * 2;
	image = (uint8_t*) malloc(data_size);

	char *pname = argv[0];
	int c;

	while (c != -1) {
		static struct option long_options[] = {
			{"pthread", no_argument, 0, 'p'},
			{"info", no_argument, 0, 'i'},
			{"help", no_argument, 0, 'h'},
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "pih", long_options, &option_index);

		if (c == -1) break;

		switch (c) {
			case 'p':
				use_pthread = 1;
				break;
			case 'i':
				show_screen_info();
				return 0;
			case 'h':
				show_help(pname);
				return 0;
			case '?':
				show_usage(pname);
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

	free(image);
	screencap_release();
	
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
	int w = info.width, h = info.height, f = info.format, bytepp = info.bytepp, rw = h, rh = w;
	int pv = (YRES - rh) / 2, ph = (XRES - rw) / 2;
	uint8_t* pixbuf16 = (uint8_t*) malloc(256 * 2);
	if (use_pthread) {
		pthread_t thread;
		pthread_create(&thread, NULL, screencap_task, NULL);
	}
	while (1) {
		if (!use_pthread) {
			screencap_getdata_rgba8888(image, image_size);
		}
		int y;
		for (y = 0; y < rh; y++) {
			int x;
			for (x = 0; x < rw; x+= 256) {
				int rem = rw - x, pcount = rem > 256 ? 256 : rem;
				int c;
				for (c = 0; c < pcount; c++) {
					int addr = ((c + x) * rh + y) * 2;
					pixbuf16[c * 2] = image[addr];
					pixbuf16[c * 2 + 1] = image[addr + 1];
				}
				int sx = ph + x, sy = YRES - (pv + y);
				int addr = sy * XRES * 2 + sx * 2;
				dl_gfx_write(cs, addr, pcount, pixbuf16);
			}
		}
		dl_cmd_sync(cs);
		int ret = dl_send_command(handle, cs, 1);
		if (ret < 0) {
			fputs("USB disconnected.\n", stderr);
			break;
		}
	}
	free(pixbuf16);
}

void show_screen_info() {
	screencap_info info = screencap_getinfo();
	printf("Device screen info:\n");
	printf("width: %d, height: %d, format: %d, bytepp: %d\n", info.width, info.height, info.format, info.bytepp);
}

void show_usage(char* pname) {
	printf("Usage: %s [-i | -h | -p]\n", pname);
}

void show_help(char* pname) {
	show_usage(pname);
	printf(" -p, --pthread\t\tUse different thread for screencap, faster but may cause wrong graphics\n");
	printf(" -i, --info\t\tShow screen info\n");
	printf(" -h, --help\t\tShow this help\n");
}

void *screencap_task(void *args) {
	while (1) {
		screencap_getdata_rgba8888(image, image_size);
	}
}
