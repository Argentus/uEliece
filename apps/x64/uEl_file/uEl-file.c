#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../../libuEliece/All/uEliece.h"

#include "uEl_keys.h"

int main(int argc, char *argv[]) {

	if (argc!=4) {
		printf("Usage: uEl-file <-e | -d> <input file> <output file>\n");
		return -1;
	}

	uint8_t mode = -1;
	if (argv[1][1]=='e')
		mode = 0;
	else if (argv[1][1]=='d')
		mode = 1;
	else {
		printf("Usage: uEl-file <-e | -d> <input file> <output file>\n");
		return -1;
	}

		printf("g\n"); fflush(stdout);
	FILE* infile = fopen(argv[2],"rb");
		printf("g\n"); fflush(stdout);
	fseek(infile, 0, SEEK_END);
		printf("g\n"); fflush(stdout);
	long file_size = ftell(infile);
		printf("g\n"); fflush(stdout);
	fseek(infile, 0, SEEK_SET);
		printf("g\n"); fflush(stdout);
	
	uint8_t *msg = malloc(file_size);
		printf("g\n"); fflush(stdout);
	fread(msg, file_size, 1, infile);
		printf("g\n"); fflush(stdout);
	fclose(infile);
		printf("g\n"); fflush(stdout);
	uint32_t length;

	if (mode==0) {
		printf("Encrypting\n"); fflush(stdout);
		uEliece_encrypt( &msg, file_size*8, &length, public_key );
	} else {
		if (uEliece_decrypt( &msg, file_size*8, &length, private_key ) & UEL_BAD_INTEGRITY)
			printf("WRONG\n");
	}

	FILE* outfile = fopen(argv[3],"wb");
	fwrite(msg, length, 1, outfile);
	fclose(outfile);

	free(msg);

	return 0;
}
