#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../libuEliece/src/uEliece.h"

int main(int argc, char *argv[]) {

	uEl_PubKey public_key;
	uEl_PrivKey private_key;

    int i = 0;
    int size = 0;
            
    for(i = 1; i < argc; i++) {
            switch (argv[i][1]) {
                    case 's' :
                            i++;
                            size = strtol(argv[i], NULL, 10);
                            break;
            }
    }

    uint8_t *msg = malloc(size);
    memset(msg,'a', size);

    printf("Preparing for encryption ...\n");

    FILE* pubkey_file = fopen("uEl_pub.key", "rb");
    if (pubkey_file == NULL){
            fprintf(stderr, "Public key not found\n");
            return -1;
    }
    if (fread( public_key, sizeof(uint8_t), (UEL_MDPC_M/8)+1, pubkey_file) != (UEL_MDPC_M/8)+1)
            return -1;
    fclose(pubkey_file);

    uEl_msglen_t length;
    uEl_msglen_t len = size * 8;


    printf("Encrypting\n"); fflush(stdout);
    uEliece_encrypt( &msg, len, &length, public_key, uEl_default_rng() );

    printf("Encryption finished\n");  
    printf("Preparing for decryption ...\n"); 

    FILE* privkey_file = fopen("uEl_priv.key", "rb");
    if (private_key == NULL){
        fprintf(stderr, "Public key not found\n");
        return -1;
    }
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
    for(i = 0; i < size; i++) {
            if(msg[i] != 'a') {
                    printf("Validation failed!\n");
                    return -1;
            }
    }
    printf("Validation succeeded. Everything works\n");


	return 0;
}
