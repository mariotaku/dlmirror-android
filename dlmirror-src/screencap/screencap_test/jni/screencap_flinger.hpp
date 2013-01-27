#include <sys/types.h>

#ifdef __cplusplus

extern "C" {
#endif
	void const * screencap_getdata();
	int screencap_init();
	void screencap_release();
	uint32_t screencap_width();
	uint32_t screencap_height();
	int screencap_format();
	size_t screencap_size();
#ifdef __cplusplus
}

#endif
