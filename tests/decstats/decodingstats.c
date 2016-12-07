#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../../libuEliece/src/uEliece.h"


int main(int argc, char *argv[]) {

		uEl_PubKey public_key;
		uEl_PrivKey private_key;


        uint8_t msg[UEL_M_BYTES * 2 ];
        uint8_t msgbckp[UEL_M_BYTES];
        int i;
        char c;

        memset(msg,0,UEL_M_BYTES * 2);
        for (i = 0; i < UEL_M_BYTES; i++) {
            c = 'A' + (random() % 26);
            msg[i] = c;
            msgbckp[i] = c;

        }
        printf(msg); 
        printf("\n\nJA SOM PRVY CHAR: %c \n\n", msg[0]);       


        FILE* privkey_file = fopen("uEl_priv.key", "rb");
        if (fread( private_key[0], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
                return -1;
        if (fread( private_key[1], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
                return -1;
        fclose(privkey_file);

        printf("\n\nJA SOM PRVY CHAR: \"%c\"  \n\n", msg[0]);   

        FILE* pubkey_file = fopen("uEl_pub.key", "rb");
        if (pubkey_file == NULL)
                return -1;
        if (fread( public_key, sizeof(uint8_t), (UEL_MDPC_M/8)+1, pubkey_file) != (UEL_MDPC_M/8)+1)
                return -1;
        fclose(pubkey_file);

        printf("\n\nJA SOM PRVY CHAR: \"%c\"  \n\n", msg[0]);

        uEliece_encode(msg, public_key);


        printf("\n\nJA SOM POSLEDNY CHAR: \"%c\"  A JA SOM N+1 char \"%c\"  \n\n", msg[1232], msg[1233]);

		printf("return message : %d \n", uEliece_decode(msg, private_key));
        printf("\n-----------------------------\n");
        printf("\n\nJA SOM PRVY CHAR: \"%c\"  \n\n", msg[0]);
    	//printf(msg);  

		int counter = 0;

		for(i = 0; i < UEL_M_BYTES; i++) {
			if(msg[i] != msgbckp[i]) {
				//printf("Having %c, expecting %c \n", msg[i],msgbckp[i]);
				counter++;
			}
		}

		printf("\nPadol som : %d krat\n", counter);

	return 0;
}
