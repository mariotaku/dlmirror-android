#include "tubecable.h"
#include "helper.h"

#define XRES 1280
#define YRES 1024

inline void write_pixel(dl_cmdstream* cs, int i, int j, dl_rle_word* color);
inline void update_screen(usb_dev_handle* handle);
inline void clear_screen(usb_dev_handle* handle);

int main(int argc, char* argv[]) {

	dl_cmdstream cs;
	dl_create_stream( &cs, 1024 * 1024);

	// load huffman table
	dl_huffman_load_table("tubecable_huffman.bin");

	usb_dev_handle* handle = dl_get_supported_device_handle();

	if (!handle) {
		printf("Couldn't find supported device; exiting.\n");
		return 1;
	}

	// startup control messages & decompressor table
	dl_init(handle);

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

void clear_screen(usb_dev_handle* handle) {
	dl_cmdstream cs;
	dl_rle_word black = { 0x00, 0x0000 };
	dl_create_stream( &cs, XRES * YRES * 16);
	for (int i = 0; i < YRES; i++) {
		int count = XRES;
		int offs = 0;
		while (count > 0) {
			dl_gfx_rle( &cs, i*XRES*2+offs, 0, &black );
			offs += 2*256;
			count -= 256;
		}
	}
	dl_cmd_sync(&cs);
	dl_send_command(handle, &cs);
	dl_destroy_stream(&cs);
}

void update_screen(usb_dev_handle* handle) {
	clear_screen(handle);
	dl_cmdstream cs;
	dl_rle_word color = { 0x1, 0x0000 };
	FILE* stream = popen("screencap", "r");
	uint16_t* header = (uint16_t*) malloc(SCREENCAP_HEADER_SIZE);
	fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
	int w = header[SCREENCAP_HEADER_INDEX_WIDTH];
	int h = header[SCREENCAP_HEADER_INDEX_HEIGHT];
	int f = header[SCREENCAP_HEADER_INDEX_FORMAT];
	int screen_size = w * h;
	int bpp = get_byte_per_pixel(f);
	int data_size = screen_size * bpp;
	uint32_t* image = (uint32_t*) malloc(data_size);
	while (1) {
		stream = popen("screencap", "r");
		fread(header, SCREENCAP_HEADER_SIZE, 1, stream);
		fread(image, data_size, 1, stream);
		dl_create_stream( &cs, XRES * YRES * 16);
		for (int i = 0; i < screen_size; i++) {
			int y = YRES - (YRES - w) / 2 - i % w;
			int x = i / w;
			color.value = color_rgba8888_to_rgb565(image[i]);
			write_pixel(&cs, y, x, &color);
		}
		dl_cmd_sync(&cs);
		dl_send_command(handle, &cs);
		dl_destroy_stream(&cs);
		pclose(stream);
	}
}
