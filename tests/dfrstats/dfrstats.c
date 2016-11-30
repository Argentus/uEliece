#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../../libuEliece/src/uEliece.h"

#define BATCH 1000

int randFile;

uint16_t rng_get16() {
        uint16_t num;
        read(randFile, &num, 2);
        return num;
}

int main(int argc, char *argv[]) {

	if (argc<2) {
		printf("Specify distance of errors\n");
		return -1;
	}


        uEl_PrivKey private_key;
        FILE* privkey_file = fopen("uEl_priv.key", "rb");
        if (fread( private_key[0], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
                return -1;
        if (fread( private_key[1], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
                return -1;
        fclose(privkey_file);

//preparing testing file
        int i,j, no ;
        int fails = 0;
        int distance;
        sscanf(argv[1], "%i", &distance);
        int n_err;
        sscanf(argv[2], "%i", &n_err);

        randFile = open("/dev/urandom", O_RDONLY );

        for (no = 0; no < BATCH; no++) {
                uint8_t *msg = malloc(UEL_M_BYTES*2);
                if (msg == NULL) {
                        printf("Alloc fail\n");
                        return -1;
                }
                memset(msg,0, UEL_M_BYTES*2);  

                /* 
                * Generate error vector
                */
                uint16_t errv[UEL_MDPC_T];
                uint8_t used;
                for (i=0;i<UEL_MDPC_T;i+=2) {
                        do {
                                do { 
                                        errv[i] = rng_get16();      
                                        errv[i] /= (uint16_t) (0xFFFF/(UEL_MDPC_N));
                                        errv[i + 1] =  (uint16_t) (errv[i] + distance) % UEL_MDPC_N;
                                } while (errv[i] > (UEL_MDPC_N-1));
                                used = 0;
                                for(j=0;j<((i)-1);j++) {
                                        if ((errv[i]==errv[j]) || (errv[i+1]==errv[j])) {
                                                used = 1;
                                                break;
                                        }
                                }
                        } while (used!=0);
                }

                /* 
                 * Apply error vector
                 */

                for (i=0;i<n_err;i++) {
                        if (errv[i] > 9857)
                                errv[i] += UEL_M_BYTE_PADDING;
                        msg[(errv[i]/8)] ^= 1<<(errv[i]%8);
                }

                if (uEliece_decode(msg, private_key) & UEL_DECODE_FAIL) {
                        printf("DF @ %i\n", no);
                        fails++;
                }   
                free(msg);
        }

        double dfr = (double)fails / (double)BATCH;
        printf("DFR: %0.4lf %c\n", dfr * 100, '%');

	return 0;
}
