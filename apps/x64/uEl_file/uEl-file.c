#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../../libuEliece/src/uEliece.h"

int main(int argc, char *argv[]) {

	if (argc!=4) {
		printf("Usage: uEl-file <-e | -d> <input file> <output file>\n");
		return -1;
	}

	uEl_PubKey public_key;
	uEl_PrivKey private_key;

	uint8_t mode = -1;
	if (argv[1][1]=='e') {
		mode = 0;
		FILE* pubkey_file = fopen("uEl_pub.key", "rb");
		if (pubkey_file == NULL)
			return -1;
		if (fread( public_key, sizeof(uint8_t), (UEL_MDPC_M/8)+1, pubkey_file) != (UEL_MDPC_M/8)+1)
			return -1;
		fclose(pubkey_file);
	}
	else if (argv[1][1]=='d') {
		mode = 1;
		FILE* privkey_file = fopen("uEl_priv.key", "rb");
		if (fread( private_key[0], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
			return -1;
		if (fread( private_key[1], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
			return -1;
		fclose(privkey_file);
	}
	else {
		printf("Usage: uEl-file <-e | -d> <input file> <output file>\n");
		return -1;
	}

	FILE* infile = fopen(argv[2],"rb");
	fseek(infile, 0, SEEK_END);
	uint64_t file_size = ftell(infile);
	fseek(infile, 0, SEEK_SET);
	uint8_t *msg = malloc(file_size);
	fread(msg, file_size, 1, infile);
	fclose(infile);

	uEl_msglen_t length;
	uEl_msglen_t len = file_size * 8;

	if (mode==0) {
		printf("Encrypting\n"); fflush(stdout);
		uEliece_encrypt( &msg, len, &length, public_key, uEl_default_rng() );
	} else {
		if (uEliece_decrypt( &msg, len, &length, private_key ) & UEL_BAD_INTEGRITY)
			printf("BAD INTEGRITY\n");
	}
	FILE* outfile = fopen(argv[3],"wb");
	fwrite(msg, length, 1, outfile);
	fclose(outfile);

	free(msg);

	return 0;
}
