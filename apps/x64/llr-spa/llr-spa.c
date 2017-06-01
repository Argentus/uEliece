#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "../../../libuEliece/src/uEliece.h"

#define ERROR_PROB (((UEL_MDPC_N) - (UEL_MDPC_T))/(UEL_MDPC_T))

double checkNodeRatio[UEL_MDPC_M][UEL_MDPC_W];
double variableNodeRatio[UEL_MDPC_M][UEL_MDPC_N];


uint8_t uEliece_decode_llr_spa(uint8_t* msg, const uEl_PrivKey privkey, uEl_PubKey pubkey){

	uint8_t return_state = 0;
	uint16_t syndromeZero, i, j, k, index;
		
	uint8_t* tmpMsg =(uint8_t*) calloc((UEL_MDPC_N/8 +1),sizeof(uint8_t));		
	uint8_t* tmp =(uint8_t*) calloc((UEL_MDPC_N/8 +1),sizeof(uint8_t));

	uint8_t order = 0;
        uint8_t maxIterations = 10;	
	double ratioSum;	
	double sum;	

	double *lratio;
	
	uEl_Mbits msg_syndrome;	

	memcpy(tmpMsg,msg,UEL_MDPC_N/8+1);	
	lratio = malloc(UEL_MDPC_N*sizeof(long double));

	for( i = 0; i < UEL_MDPC_M; i++){	
		lratio[i] = ((tmpMsg[i/8] >> (i%8)) & 1) == 0 ? log(ERROR_PROB)
							   : 0 - log(ERROR_PROB); 
		lratio[UEL_MDPC_M + i] = ((tmpMsg[(UEL_M_PADDED +i)/8] >> (i%8)) & 1) == 0 ? log(ERROR_PROB)  										: 0 - log(ERROR_PROB);
	}

	for( i = 0; i < UEL_MDPC_M; i++)
		for( j = 0; j < UEL_MDPC_W; j++)
			checkNodeRatio[i][j] = 0;

	for( i = 0; i < UEL_MDPC_M; i++)
		for( j = 0; j < UEL_MDPC_M; j++){
			variableNodeRatio[i][j] = lratio[j];
			variableNodeRatio[i][j + UEL_MDPC_M] = lratio[j + UEL_MDPC_M];
		}
	
	while(order < maxIterations){	
	
	//for each m (riadok) and for each n in N(m) - tam kde v riadku m a v stlpci n su jednotky 	
	for ( i = 0; i < UEL_MDPC_M; i++){
		
		for( j = 0; j < UEL_MDPC_W; j++){
			sum = 1;
			for ( k = 0; k < UEL_MDPC_W/UEL_MDPC_N0; k++){
				if(k != j){
					//sum *= tanh(variableNodeRatio[k][(privkey[0][k] + i) % UEL_MDPC_M]/2);
					sum = sum * (tanh(variableNodeRatio[i][(privkey[0][k] + i) % UEL_MDPC_M]/2));
				}
			}

			for ( k = UEL_MDPC_W/UEL_MDPC_N0; k < UEL_MDPC_W; k++){
				if(k != j){
				//sum = sum * (tanh(variableNodeRatio[k-UEL_MDPC_W/UEL_MDPC_N0][(privkey[0][k - UEL_MDPC_W/UEL_MDPC_N0] + i) % UEL_MDPC_M]/2));
					
				sum = sum * (tanh(variableNodeRatio[i][UEL_MDPC_M+ ((privkey[1][k] + i) % UEL_MDPC_M)]/2));
				}	
			}
		
			checkNodeRatio[i][j] = 2/tanh(sum);
			//checkNodeRatio[i][j] =log((1+sum)/(1-sum));	
				
		}
	}
	
		
	
	//for each n (stlpec)
	memcpy(tmpMsg,msg,UEL_MDPC_N/8+1);
	for( i = 0; i < UEL_MDPC_M; i++){
		ratioSum = lratio[i];
		for( index = 0; index < UEL_MDPC_W/UEL_MDPC_N0; index++){
			for( j = 0; j < UEL_MDPC_M; j++){
				if((privkey[0][index] + j) % UEL_MDPC_M == i){
					ratioSum += checkNodeRatio[j][index];	
					break;
				}
			}
		}
//		printf("%f\n",ratioSum);
		if(ratioSum >= 0){
			tmpMsg[i/8] &= ~(1 << (i % 8));
		}
		else
			tmpMsg[i/8] |= (1 << (i % 8));
	


		ratioSum = lratio[i + UEL_MDPC_M];
		for( index = 0; index < UEL_MDPC_W/UEL_MDPC_N0; index++){
			for( j = 0; j < UEL_MDPC_M; j++){
				if((privkey[1][index] + j) % UEL_MDPC_M == i){
					ratioSum += checkNodeRatio[j][index + UEL_MDPC_W/UEL_MDPC_N0];	
					break;
				}
			}
		}

		if(ratioSum >= 0)
			tmpMsg[(i + UEL_M_PADDED)/8] &= ~(1 << (i % 8));
		else
			tmpMsg[(i + UEL_M_PADDED)/8] |= (1 << (i % 8));

	}
	

	// velkost msg_syndrome je UEL_MDPC_M/8 + 1
	return_state |= uEliece_syndrome(tmpMsg, privkey, msg_syndrome);
	syndromeZero = 0;
	for(i = 0; i < UEL_MDPC_M/8; i++)
		if(msg_syndrome[i] != 0x00){
			syndromeZero = 0;
			break;
		}
	if(syndromeZero) {
		printf("finish\n");
		//break;
	}
	else{

		for( i = 0; i < UEL_MDPC_M; i++){
			for( j = UEL_MDPC_W/UEL_MDPC_N0  ; j > 0 ; j--){
				sum = lratio[i];	
				for( k = 0; k < UEL_MDPC_M; k++){
					if((privkey[0][j-1] + k) % UEL_MDPC_M == i && k != UEL_MDPC_M - privkey[0][j-1] + UEL_MDPC_W/UEL_MDPC_N0 - j){						
						sum += checkNodeRatio[k][j - 1];
					}else if((privkey[0][j-1] + k) % UEL_MDPC_M == i)
						index = k;
				}
				
			}
			variableNodeRatio[index][i] = sum;


			for( j = UEL_MDPC_W/UEL_MDPC_N0  ; j > 0 ; j--){
				sum = lratio[i + UEL_MDPC_M];	
				for( k = 0; k < UEL_MDPC_M; k++){
					if((privkey[1][j-1] + k) % UEL_MDPC_M == i && k != UEL_MDPC_M - privkey[1][j-1] + UEL_MDPC_W/UEL_MDPC_N0 - j){
						sum += checkNodeRatio[k][j - 1 + UEL_MDPC_W/UEL_MDPC_N0];
					}
					else if((privkey[1][j-1] + k) % UEL_MDPC_M == i)
						index = k;
				}
			
			}
			variableNodeRatio[index][i + UEL_MDPC_M] = sum;
			
		}
		order++;
	

	//na porovnanie s druhym dekodovanim kolko bitov sa zmenilo
	memcpy(tmp,msg,UEL_MDPC_N/8+1);
	uEliece_decode_bf1(msg,privkey);

	int cnt = 0;
	for(i = 0; i < UEL_MDPC_N/8 + 1; i++){
	
		if( ((tmpMsg[i/8] >> (i%8)) & 1) != ((tmp[i/8] >>(i%8)) & 1))
			cnt++;
		if(((tmpMsg[(UEL_M_PADDED +i )/8] >> (i%8)) & 1) != ((tmp[(UEL_M_PADDED + i)/8] >>(i%8)) & 1))
			cnt++;	
	}

	printf("Pocet rozdielnych bitov medzi povodnou spravou a dekodovanou: %d\n",cnt);
	}
	}
	return return_state;

}

int main(){
	
	uEl_PrivKey private_key;
	uEl_PubKey public_key;

	FILE* pubkey_file = fopen("uEl_pub.key","rb");
	if(pubkey_file == NULL)
		return -1;
	if(fread( public_key, sizeof(uint8_t), (UEL_MDPC_M/8)+1, pubkey_file) != (UEL_MDPC_M/8)+1)
		return -1;
	fclose(pubkey_file);

	FILE* privkey_file = fopen("uEl_priv.key","rb");	
	if (fread( private_key[0], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
		return -1;
	if (fread( private_key[1], sizeof(uint16_t), UEL_MDPC_W/2, privkey_file) != UEL_MDPC_W/2)
                return -1;
        fclose(privkey_file);
	
	FILE* infile = fopen("in.txt","rb");

	if(infile == 0){
		return 0;

	}
	fseek(infile, 0, SEEK_END);
	uint64_t file_size = ftell(infile);
	fseek(infile, 0, SEEK_SET);
	uint8_t *msg = malloc(file_size);
	fread(msg, file_size, 1, infile);
	fclose(infile);

	uint8_t *tmp = malloc((UEL_MDPC_N/8 +1)*sizeof(uint8_t));
	memcpy(tmp,msg,file_size);
	printf("%d\n",memcmp(tmp,msg,file_size));
	uEliece_encode(msg, public_key);
	//uEliece_decode_llr_spa(msg, private_key, public_key);
	int i, cnt;
	cnt = 0;
	for( i = 0; i < 10000; i++){
	uEliece_decode_bf1(msg,private_key);
	if(memcmp(msg,tmp,file_size) == 0)
		cnt++;
	else
		memcpy(msg,tmp,UEL_MDPC_N/8+1);
	}
		printf("%d\n",cnt);
	

	return 0;
}
