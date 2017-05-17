#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../libuEliece/src/uEliece.h"

int main(int argc, char *argv[]) {

	if (argc!=3) {
		perror("Usage: encdectest <message size in B> <path to key pair (without .priv or .pub extension)>");
		return -1;
	}

	char pubkey_path[128];
	char privkey_path[128];
	strcpy(pubkey_path, argv[2]);
	strcpy(privkey_path, argv[2]);
	strcat(pubkey_path, ".pub");
	strcat(privkey_path, ".priv");
	uEl_PubKey public_key;
	uEl_PrivKey private_key;

	int size = atoi(argv[1]);
	if (size > 268435455) {
		fprintf(stderr, "Requested message %iB is too big! Must be <256MiB\n", size);
		fflush(stderr);
		return -2;
	}

	FILE* pubkey_file = fopen(pubkey_path, "rb");
	if (pubkey_file == NULL) {
		fprintf(stderr, "Cannot open key file '%s'!\n", pubkey_path);
		fflush(stderr);
		return -2;
	}
	if (fread( public_key, sizeof(uint8_t), (UEL_MDPC_M/8)+1, pubkey_file) != (UEL_MDPC_M/8)+1) {
		fprintf(stderr, "Key file '%s' is invalid!\n", pubkey_path);
		fflush(stderr);
		return -2;
	}
	fclose(pubkey_file);

	FILE* privkey_file = fopen(privkey_path, "rb");
	if (fread( private_key[0], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2) {
		fprintf(stderr, "Cannot open key file '%s'!\n", privkey_path);
		fflush(stderr);
		return -2;
	}
	if (fread( private_key[1], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2) {
		fprintf(stderr, "Key file '%s' is invalid!\n", privkey_path);
		fflush(stderr);
		return -2;
	}
	fclose(privkey_file);

	uint8_t *msg = malloc(size);
	memset(msg,'a', size);

	uEl_msglen_t length;
	uEl_msglen_t len = size * 8;

	uEliece_encrypt( &msg, len, &length, public_key, uEl_default_rng() );

	if (uEliece_decrypt(&msg,length*8,&length,private_key ) & UEL_BAD_INTEGRITY) {
		fprintf(stderr, "Decoded message integrity check failed!\n");
		fflush(stderr);
		return -1;
	}

	int i;
	for(i = 0; i < size; i++) {
		if(msg[i] != 'a') {
			fprintf(stderr, "Decrypted message invalid!\n");
			fflush(stderr);
			return -1;
		}
	}

	return 0;
}
