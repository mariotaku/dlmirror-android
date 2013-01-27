#include "screencap_flinger.hpp"
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	
	screencap_init();
	while (1) {
		screencap_getdata();
		printf("updated one frame\n");
	}
	return 0;
}
