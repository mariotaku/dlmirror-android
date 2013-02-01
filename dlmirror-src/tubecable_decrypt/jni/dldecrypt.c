#include "tubecable.h"

unsigned char key[16];

int main( int argc, const char* argv[] ) {

	printf("dldecrypt 0.1 - strip displaylink on-wire encryption\n");

	if ((argc < 3) || (strlen(argv[1]) != 32)) {
		printf("usage: %s key_in_hex file1 file2 file3...\n",argv[0]);
		exit(1);
	}

	dl_crypt_generate_key( dl_crypt_keybuffer, dl_crypt_ofsbuffer );

	// read key
	int i1;
	for (i1 = 0; i1 < 16; i1++) {
		const char tmp[3] = { argv[1][2*i1], argv[1][2*i1+1], 0 };
		if ( sscanf( tmp, "%hhx", &(key[i1]) ) == 1 ) continue;
		printf("error in hex string\n"); exit(2);
	}

	// get start offset
	int crc = dl_crypt_crc12(key,16);
	int offset = dl_crypt_ofsbuffer[crc];
	printf("key crc: 0x%x, start offset: 0x%x\n",crc,offset);

	int i2;
	for (i2 = 2; i2 < argc; i2++) {

		char outfile[1024];
		snprintf( outfile, sizeof(outfile), "%s.dec", argv[i2] );

		printf("processing file #%d: %s -> %s\n", i2 - 1, argv[i2], outfile);

		FILE* in  = fopen( argv[i2], "r" );
		FILE* out = fopen( outfile, "w" );

		if (!in || !out) {
			printf("error opening file #%d\n",i2-1);
			exit(3);
		}

		while (!feof(in)) {
			unsigned char buffer[4095];
			int size = fread( buffer, 1, sizeof(buffer), in );
			int j;
			for (j = 0; j < size; j++) buffer[j] = buffer[j] ^ dl_crypt_keybuffer[j+offset];
			offset += size;
			offset %= 4095;
			fwrite( buffer, 1, size, out );
		}

		fclose(in);
		fclose(out);
	}
	return 0;
}

