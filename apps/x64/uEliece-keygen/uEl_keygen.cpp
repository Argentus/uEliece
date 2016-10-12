#include <NTL/GF2X.h>
#include <iostream>

#include <random>
#include <stdint.h>
#include <stdio.h>

#define UEL_MDPC_M (9857)
#define UEL_MDPC_W0 (71)



void printUsage() {
	printf("Usage: uEl-keygen [-Options]\n--------------------------------\nUse option ? for help.\n");
}

void printHelp() {
	printf("\nuEl-keygen : Key pair generator for uEliece cryptosystem.\n---------------------------------------\nUsage: uEl-keygen [-Options]\n\
-------------------\nOptions:\n B - output in binary file format\n H - output in ASCII hex file format\n K - output in ASCII hex with ':' delimiters format\n C - output in C array file format\n-------------------\nKey pair is saved to files: uEl_pub.key and uEl_priv.key\n-------------------\n");
	return;
}

NTL::GF2X transpose_Matrix(NTL::GF2X& subject, int length) {
	NTL::GF2X transposed;
	transposed.SetLength(length);
	for (int i=0;i<length;i++) {
		if (coeff(subject,i)!=0)
			SetCoeff(transposed,(length-i)%length,1);
	}
	return transposed;

}

NTL::GF2X transpose_Matrix(NTL::GF2X subject, int length) {	// Overload: GF2X not reference
	NTL::GF2X transposed;
	transposed.SetLength(length);
	for (int i=0;i<length;i++) {
		if (coeff(subject,i)!=0)
			SetCoeff(transposed,(length-i)%length,1);
	}
	return transposed;
}

void fprint_GF2X_sparse(NTL::GF2X& subject, int length, FILE* file, char mode) {

	long w = weight(subject);
	int current = 0;
	for (uint16_t i=0;i<length;i++) {
		if (coeff(subject,i)!=0) {
			current++;
			switch (mode) {
				case 'C':			
					fprintf(file," 0x%x,",i);
					break;
				case 'H':
					fprintf(file,"%02x",i);
					break;
				case 'K':
					fprintf(file,"%02x",i);
					if (current!=(w))					
						fprintf(file,":");
					break;
				case 'B':
					fwrite(&i,sizeof(i),1,file);
					break;

			}
		}
	}
}

void fprint_GF2X(NTL::GF2X& subject, int length, FILE* file, char mode) {
	
	uint8_t	byte = 0;
	for (int i=0;i<length;i++) {
		if (coeff(subject,i)!=0) {
			byte |= 1<<(i%8);
		}
		if (i%8==7) {
			switch (mode) {
				case 'C':
					fprintf(file," 0x%02x,",byte);
					break;
				case 'H':
					fprintf(file,"%02x",byte);
					break;
				case 'K':
					fprintf(file,"%02x:",byte);
					break;
				case 'B':
					fwrite(&byte,sizeof(byte),1,file);
					break;
			}
			byte=0;
		}
	}
	if (length%8) {
		switch(mode) {
			case 'C':
				fprintf(file," 0x%02x,",byte);
				break;
			case 'H':	// fall through, same output
			case 'K':
				fprintf(file,"%02x",byte);
				break;
			case 'B':
				fwrite(&byte,sizeof(byte),1,file);
				break;
		}
	}
}

int main (int argc,  char** argv) {


	char mode = 'H';
	int interactive = 0;
	int cmode = 0;
	if (argc>1) {
	char arg;
		if(argv[1][0]=='-') {
			int i=1;
			arg = argv[1][i];
			 do {
				arg = argv[1][i];
				switch (arg) {
					case 'B':
						mode='B';
						break;
					case 'H':
						mode='H';
						break;
					case 'K':
						mode='K';
						break;
					case 'C':
						mode='C';
						break;
					case '?':
						printHelp();
						return 0;

					default:
						printUsage();
						return -1;
				}
				i++;
				arg = argv[1][i];
			} while (arg!='\0');
		}
	} else {
		printUsage();
		return -1;
	}

	char priv_file_name[] = "uEl_priv.key";
	char pub_file_name[] = "uEl_pub.key";
	
	printf("Generating key pair for uEliece cryptosystem.\n");

	NTL::GF2X H0;
	NTL::GF2X H1;
	NTL::GF2X modPoly;

	std::mt19937 rng;			// Init randomness generator
	rng.seed(std::random_device()());	// and uniform distribution 0-M	
	std::uniform_int_distribution<std::mt19937::result_type> distM(0, (UEL_MDPC_M-1));

	H0.SetLength( UEL_MDPC_M );
	H1.SetLength( UEL_MDPC_M );
	
	modPoly.SetLength( UEL_MDPC_M+1 );	// Create poly x^M+1
	SetCoeff(modPoly,0,1);			// for modulo
	SetCoeff(modPoly,UEL_MDPC_M,1);	//	

	int random_index;				//
	for (int i=0;i<UEL_MDPC_W0;i++) {		// Generate random poly H0
							// of length M and weight W 
		random_index = distM(rng);		//
    							//
		if ( coeff(H0,random_index)==0 )	//
			SetCoeff(H0,random_index,1);	//
		else					//
			i--;				//
	}						//

	NTL::GF2X d;
	NTL::GF2X H1_inv;
	NTL::GF2X t;
	
	do {							//
		for (int i=0;i<UEL_MDPC_W0;i++) {		// Generate random invertible poly H1
								// of length M and weight W
			random_index = distM(rng);		//
    								//
			if ( coeff(H1,random_index)==0 )	//
				SetCoeff(H1,random_index,1);	//
			else					//
				i--;				//
		}						//
		H1.normalize();					//
		XGCD(d, H1_inv, t, H1, modPoly );		// Calculate inverse of H1
	} while (!IsOne(d));					//

	H1_inv.normalize();

	NTL::GF2X Q = transpose_Matrix((H1_inv*H0)%modPoly,UEL_MDPC_M);			// Calculate right side of generator matrix

	FILE* priv_f = fopen(priv_file_name, "w");	// Open files
	FILE* pub_f = fopen(pub_file_name, "w");	//


	switch (mode) {

		case 'C':
			fprintf(priv_f,"{ {");				// Print private key to file
			fprint_GF2X_sparse(H0,UEL_MDPC_M,priv_f,mode);	
			fprintf(priv_f,"}, { ");
			fprint_GF2X_sparse(H1,UEL_MDPC_M,priv_f,mode);
			fprintf(priv_f,"} }");
			
			fprintf(pub_f,"{ ");				// Print public key to file
			fprint_GF2X(Q,UEL_MDPC_M,pub_f,mode);
			fprintf(pub_f,"}");
			break;

		default:
			fprint_GF2X(Q,UEL_MDPC_M,pub_f,mode);
			fprint_GF2X_sparse(H0,UEL_MDPC_M,priv_f,mode);			
			fprint_GF2X_sparse(H1,UEL_MDPC_M,priv_f,mode);
			break;

	}
	fclose(priv_f);
	fclose(pub_f);

	printf("Key pair saved to files \"uEl_pub.key\" and \"uEl_priv.key\".\n");

	


}
