#include "android_screencap.h"
#include "tubecable.h"
#include "helper.h"

#define XRES 1400
#define YRES 1050

inline void write_pixel(dl_cmdstream* cs, int i, int j, dl_rle_word* color);
inline void update_screen(usb_dev_handle* handle, dl_cmdstream* cs);
inline void show_screen(usb_dev_handle* handle, dl_cmdstream* cs);
inline void clear_screen(usb_dev_handle* handle, dl_cmdstream* cs);

int main(int argc, char* argv[]) {

	dl_cmdstream cs;
	dl_create_stream(&cs, XRES * YRES * 8);

	// load huffman table
	dl_huffman_load_table("/home/mariotaku/tubecable_huffman.bin");

	usb_dev_handle* handle = dl_get_supported_device_handle();

	if (!handle) {
		printf("Couldn't find supported device; exiting.\n");
		screencap_info info = get_screencap_info();
		printf("w: %d, h: %d, fm: %d\n", info.width, info.height, info.format);
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

	sleep(1);

	clear_screen(handle, &cs);
	update_screen(handle, &cs);
	//show_screen(handle);

	printf("goodbye.\n");
	dl_destroy_stream(&cs);
	usb_close(handle);
	return(0);
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

void show_screen(usb_dev_handle* handle, dl_cmdstream* cs) {
	printf("execute screencap command\n");
	screencap_info info = get_screencap_info();
	int w = info.width, h = info.height, f = info.format; 
	int bpp = get_byte_per_pixel(f);
	printf("width: %d, height: %d, bpp: %d\n", w, h, bpp);
	int screen_size = w * h;
	int data_size = screen_size * bpp;
	uint32_t* image = (uint32_t*) malloc(data_size);
	uint8_t* image8 = (uint8_t*) malloc(screen_size * 2);
	do_screencap(image, data_size);
	printf("convert to rgb16...\n");
	rgba8888_to_rgb565_8(image, image8, screen_size);
	free(image);
	uint16_t* image16 = (uint16_t*) image8;

	int myaddr = 0;
	int mypcnt = 0;
	int imgsize = w * h;

	// very important: before adding compressed blocks, set register 0x20 to 0xFF once
	dl_reg_set(cs, DL_REG_SYNC, 0xFF);

	printf("compress...\n");
	while (mypcnt < imgsize) {
		int res = dl_huffman_compress(cs, myaddr, imgsize-mypcnt, image16+mypcnt );
		mypcnt += res;
		myaddr += res*2;
	}
	
	printf( "encoded %d bytes\n",cs->pos);
	
	dl_cmd_sync(cs);
	dl_send_command(handle, cs, 1);
}

void write_pixel(dl_cmdstream* cs, int y, int x, dl_rle_word* color) {
	dl_gfx_rle(cs, y * XRES * 2 + x * 2, 1, color);
}

void update_screen(usb_dev_handle* handle, dl_cmdstream* cs) {
	screencap_info info = get_screencap_info();
	int w = info.width, h = info.height, f = info.format;
	int rw = h, rh = w;
	int pv = (YRES - rh) / 2, ph = (XRES - rw) / 2;
	int image_size = w * h, bpp = get_byte_per_pixel(f);
	int data_size = image_size * bpp;
	uint32_t* image = (uint32_t*) malloc(data_size);
	uint32_t* pixbuf32 = (uint32_t*) malloc(256 * bpp);
	//uint16_t* pixbuf16 = (uint16_t*) malloc(256 * bpp);
	uint8_t* pixbuf8 = (uint8_t*) malloc(256 * 2);
	while(1) {
		do_screencap(image, data_size);
		//dl_reg_set(cs, DL_REG_SYNC, 0xFF);
		int y;
		for (y = 0; y < rh; y++) {
			int x;
			for (x = 0; x < rw; x+= 256) {
				int pcount = rw - x > 256 ? 256 : rw - x;
				int c;
				for (c = 0; c < pcount; c++) {
					pixbuf32[c] = image[(x + c) * rh + y];
				}
				//rgba8888_to_rgb565_16(pixbuf32, pixbuf16, pcount);
				rgba8888_to_rgb565_8(pixbuf32, pixbuf8, pcount);
				int sx = ph + x, sy = YRES - (pv + y);
				dl_gfx_write(cs, sy * XRES * 2 + sx * 2, pcount, pixbuf8);
				//dl_huffman_compress(cs, sy * XRES * 2 + sx * 2, pcount, pixbuf16);
			}
		}
		dl_cmd_sync(cs);
		dl_send_command(handle, cs, 1);
		printf("drawed 1 frame\n");
	}
//	uint32_t pixel = 0;
//	dl_rle_word color = {0x01, 0x0000};
//	while (1) {
//		FILE* stream = popen(SCREENCAP_COMMAND, "r");
//		int i;
//		for (i = -3; i < screen_size; i++) {
//			fread(&pixel, 4, 1, stream);
//			if (i < 0) continue;
//			if (pixel != image[i]) {
//				image[i] = pixel;
//				int y = YRES - (pv + i % w);
//				int x = i / w + ph;
//				color.value = color_rgba8888_to_rgb565(pixel);
//				write_pixel(cs, y, x, &color);
//			}
//		}
//		pclose(stream);
//		dl_cmd_sync(cs);
//		dl_send_command(handle, cs, 1);
//		printf("drawed 1 frame\n");	
//	}
	free(image);
}
