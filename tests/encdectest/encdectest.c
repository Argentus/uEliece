#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../libuEliece/src/uEliece.h"

int main(int argc, char *argv[]) {

	if (argc!=2) {
		printf("Specify size in MB\n");
		return -1;
	}

	uEl_PubKey public_key;
	uEl_PrivKey private_key;

//preparing testing file
        printf("Creating file of given size ...\n");
        int i = 0;
        int FILESIZE = (argv[1][1] - '0') * 1<<20;
        printf("FILESIZE :%d\n", FILESIZE);
        uint8_t *msg = malloc(FILESIZE);
        memset(msg,'a', FILESIZE);

        printf("Preparing for encryption ...\n");

        FILE* pubkey_file = fopen("uEl_pub.key", "rb");
        if (pubkey_file == NULL)
                return -1;
        if (fread( public_key, sizeof(uint8_t), (UEL_MDPC_M/8)+1, pubkey_file) != (UEL_MDPC_M/8)+1)
                return -1;
        fclose(pubkey_file);

        uEl_msglen_t length;
        uEl_msglen_t len = FILESIZE * 8;


        printf("Encrypting\n"); fflush(stdout);
        uEliece_encrypt( &msg, len, &length, public_key, uEl_default_rng() );

        FILE* outfile = fopen("myfile.encrypted","wb");
        fwrite(msg, length, 1, outfile);
        fclose(outfile);

        printf("Encryption finished\n");  
        printf("Preparing for decryption ...\n"); 

        FILE* privkey_file = fopen("uEl_priv.key", "rb");
        if (fread( private_key[0], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
                return -1;
        if (fread( private_key[1], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
                return -1;
        fclose(privkey_file); 

        if (uEliece_decrypt(&msg,length*8,&length,private_key ) & UEL_BAD_INTEGRITY) {
                printf("Bad integrity");
        }
        printf("Decryption finished\n"); 

        printf("Validation starting ...\n"); 
        for(i = 0; i < FILESIZE; i++) {
                if(msg[i] != 'a') {
                        printf("Validation failed!\n");
                        return -1;
                }
        }
        printf("Validation succeeded. Everything works\n");


	return 0;
}
