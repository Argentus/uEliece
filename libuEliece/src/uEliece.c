/*
 *	File:	"uEliece.c"
 *	Version: 0.7_alpha
 *
 * GitHub:
 * 	https://github.com/Argentus/uEliece
 *
 * by authors:
 *		# Radovan Bezak
 *		radobezak@gmail.com
 *
 * 2016
 *----------------------------------------------------- 
 * QC-MDPC MECS parameters:
 * 	- 128 bit security
 *	- n0 = 2	(number of cyclic blocks in QC-MDPC matrix)
 *	- n = 19 714	(block length)
 *	- w = 142	(QC-MDPC matrix line weight)
 *	- t = 134	(number of errors inflicted on encoded message)
 *-----------------------------------------------------
 */

#include "Keccak/Keccak-compact.h"
#include "uEliece.h"
#include "uEliece-settings.h"
#include "uEliece-utils.h"

#include <stdlib.h>
#include <stdint.h>

#define MSG (*msg)

/*
 * 	Function:  uEliece_decrypt 
 * --------------------
 * 	This function decrypts given message using the
 *	uEliece cryptosystem with specified private
 *	key. Resulting plaintext is stored in place
 *	of the original message.
 * _________________________________________________
 * @param1:	uint8_t** msg 
 *		- pointer to pointer to ciphertext message.
 *		NEEDS TO BE DYNAMICALLY ALLOCATED!
 *		The memory will be reallocated and used to
 *		store the resulting plaintext.
 *
 * @param2:	uEl_msglen_t ctext_len
 *		- length of ciphertext in bits
 *	
 * @param3:	uEl_msglen_t* len
 *		- pointer to where resulting plaintext
 *		length (in bytes) is to be written. Can be NULL if
 *		this information is not desired.
 *
 * @param4:	uEl_PrivKey privkey
 *		- private key used for decryption
 * _________________________________________________
 * @returns: 0 if there was no error, otherwise flags:
 * 	
 */

uint8_t uEliece_decrypt( uint8_t** msg, uEl_msglen_t ctext_len, uEl_msglen_t* len, const uEl_PrivKey privkey) {

	uint8_t decryption_state = 0; 			// Return value, 0 correct, flags for errors
	const uEl_msglen_t ctext_len_bytes = ctext_len/8;

	decryption_state |= uEliece_decode(MSG+UEL_ENCODED_BLOCK_START, privkey);
	decryption_state |= uEliece_unwrap(MSG, ctext_len, len);
	decryption_state |= uEliece_verify(MSG, ctext_len, len);

	/*
	 * Reallocate message to correct length
	 */
	MSG = realloc(MSG,*len);

	return decryption_state;
}

/* 
 * 	Function: uEliece_encrypt
 * ------------------------------- 
 *	This function encrypts given message using the
 *	uEliece cryptosystem with specified public key.
 *	Resulting ciphertext is stored in place of the
 * 	original message.
 * ____________________________________________________
 * @param1:	uint8_t** msg
 *		- pointer to pointer to plaintext message.
 *		the memory will be reallocated and used to
 *		store the resulting ciphertext.
 *
 * @param2:	uint32_t len
 *		- length of plaintext in bits
 *	
 * @param3:	uint32_t* result_len
 *		- pointer to where resulting ciphertext
 *		length is to be written. Can be NULL if
 *		this information is not desired.
 *
 * @param4:	uEl_PubKey pubkey (see uEliece-utils.h)
 *		- public key used for encryption
 *
 * @param5:	const uEl_rng rng
 *		- random number generator, NULL for default if defined
 * ____________________________________________________
 * @returns:	0 if successful
 *
 */
uint8_t uEliece_encrypt( uint8_t** msg, uEl_msglen_t len, uEl_msglen_t* result_len, uEl_PubKey pubkey, const uEl_rng rng ) {
	
	uint8_t encryption_state = 0; 			// Return value, 0 correct, flags for errors

	encryption_state |= uEliece_encryption_prepare(msg, len, result_len);
	if (encryption_state & UEL_MALLOC_FAIL)
		return encryption_state;
	const uEl_msglen_t ctext_len_bytes = *result_len;
	encryption_state |= uEliece_wrap(MSG, len, result_len, &rng);
	encryption_state |= uEliece_encode(MSG+UEL_ENCODED_BLOCK_START, pubkey);
	encryption_state |= uEliece_add_errors(MSG+UEL_ENCODED_BLOCK_START, &rng);
	
	return encryption_state;
}

/* Decryption methods */

uint8_t uEliece_syndrome( uint8_t* msg, const uEl_PrivKey privkey, uEl_Mbits msg_syndrome ) {

	uint8_t return_state = 0; 			// Return value, 0 correct, flags for errors
	uint16_t i, j;
	uint32_t rotated_index;

	for (i=0;i<(UEL_M_PADDED/8);i++)		// Init syndrome to 0
		msg_syndrome[i]=0;

							// Multiply message by parity check matrix
	for (i=0;i<UEL_MDPC_M;i++) {			// first half (message part)
		
		if ( (msg[(i/8)] & (1 << (i%8))) ) {

			for (j=0;j<(UEL_MDPC_W/UEL_MDPC_N0);j++) {
				rotated_index = ((UEL_MDPC_M - privkey[0][j] + i )%UEL_MDPC_M );
				msg_syndrome[(rotated_index)/8] ^= (1<<((rotated_index)%8));	
			}

		}
	}

	for (i=0;i<UEL_MDPC_M;i++) {			// second half (parity part)
		
		if ( (msg[(UEL_M_PADDED/8) + (i/8)] & (1 << (i%8))) ) {
			for (j=0;j<(UEL_MDPC_W/UEL_MDPC_N0);j++) {
				rotated_index = ((UEL_MDPC_M - privkey[1][j] + i )% UEL_MDPC_M );
				msg_syndrome[(rotated_index)/8] ^= (1<<((rotated_index)%8));	
			}
		} 
	}

	return return_state;

}

uint8_t uEliece_decode( uint8_t* msg, const uEl_PrivKey privkey ) {

	uint8_t return_state = 0; 			// Return value, 0 correct, flags for errors
	uint16_t i, j, k;

	uEl_Mbits msg_syndrome;
	return_state |= uEliece_syndrome(msg, privkey, msg_syndrome);

	uint32_t n_upc;
	uint8_t syndromeZero;
	uint32_t rotated_index;
	for (i=0;i<UEL_BFA_MAX;i++) {

		syndromeZero=1;
		for (j=0;j<(UEL_M_PADDED/8);j++)
			if (msg_syndrome[j]!=0x00) {
				syndromeZero=0;
				break;
			}
		if (syndromeZero)
			break;
		
		for (j=0;j<UEL_MDPC_M;j++) {
			n_upc = 0;
			for(k=0;k<UEL_MDPC_W/UEL_MDPC_N0;k++) {
				rotated_index = ((UEL_MDPC_M - privkey[0][k] + j )% UEL_MDPC_M );
				if ( (msg_syndrome[(rotated_index/8)] & (1 << (rotated_index%8))) )
					n_upc++;
			}
			if (n_upc >= uel_bfa_flip_thresh[i]) {
				msg[(j/8)] ^= 1<<(j%8);
				for (k=0;k<(UEL_MDPC_W/UEL_MDPC_N0);k++) {
					rotated_index = ((UEL_MDPC_M - privkey[0][k] + j )% UEL_MDPC_M );
					msg_syndrome[(rotated_index)/8] ^= (1<<((rotated_index)%8));	
				}
				syndromeZero=1;
				for (k=0;k<(UEL_M_PADDED/8);k++)
					if (msg_syndrome[k]!=0x00) {
						syndromeZero=0;
						break;
					}
				if (syndromeZero) {
					break;
				}
			}
		}
		for (j=0;j<UEL_MDPC_M;j++) {
			if (syndromeZero)
				break;
			n_upc = 0;
			for(k=0;k<UEL_MDPC_W/UEL_MDPC_N0;k++) {
				rotated_index = ((UEL_MDPC_M - privkey[1][k] + j )% UEL_MDPC_M );
				if ( (msg_syndrome[(rotated_index/8)] & (1 << (rotated_index%8))) )
					n_upc++;
			}
			if (n_upc >= uel_bfa_flip_thresh[i]) {
				msg[(UEL_M_PADDED/8)+(j/8)] ^= 1<<(j%8);
				for (k=0;k<(UEL_MDPC_W/UEL_MDPC_N0);k++) {
					rotated_index = ((UEL_MDPC_M - privkey[1][k] + j )% UEL_MDPC_M );
					msg_syndrome[(rotated_index)/8] ^= (1<<((rotated_index)%8));	
				}
				syndromeZero=1;
				for (k=0;k<(UEL_M_PADDED/8);k++)
					if (msg_syndrome[k]!=0x00) {
						syndromeZero=0;
						break;
					}
				if (syndromeZero) {
					break;	
				}
			}
		}
	}

	if (!syndromeZero) {
		return_state |= UEL_DECODE_FAIL;
	}
	return return_state;
}

uint8_t uEliece_unwrap( uint8_t* msg, uEl_msglen_t ctext_len, uEl_msglen_t* len) {

	uint8_t return_state = 0; 			// Return value, 0 correct, flags for errors
	const uEl_msglen_t ctext_len_bytes = ctext_len/8;
	int i, j;

	/* 
	 * Get encrypted session key
	 */
	uEl_256bit sskey;
	for (i=0;i<32;i++)
		sskey[i] = msg[UEL_SSKEY_START+i];

	/* 
	 * Decrypt session key
	 */
	uEl_256bit key;
	crypto_hash( (unsigned char*) key, (unsigned char*) msg, (ctext_len - UEL_M_PADDED - 256 - 8)/8 );
	for (i=0;i<32;i++)
		sskey[i] ^= key[i];

	/* 
	 * Init PRNG with session key
	 */
	uEl_256bit PRNG_state;
	for (i=0;i<32;i++)
		PRNG_state[i] = sskey[i] ;
	
	/* 
	 * Decrypt message by 256-bit blocks
	 */
	const uEl_msglen_t full_blocks_to_decrypt = (ctext_len - UEL_M_PADDED - 256 - 8)/256;

	for (i=0;i<full_blocks_to_decrypt;i++)
	{
		crypto_hash( (unsigned char*) key, (unsigned char*) PRNG_state, 256/8 );
		PRNG_state[0]++;			// !!!!!!!!!!!!!!!!! PLACEHOLDER !!!!!!!!!!!!
		for (j=0;j<32;j++)
			msg[(32*i)+j] ^= key[j] ;
	}
	
	uint8_t remaining_bits_to_decrypt = (ctext_len - UEL_MDPC_M - 256)%256;
	const uint8_t remaining_full_bytes_to_decrypt = remaining_bits_to_decrypt/8;

	crypto_hash( (unsigned char*) key, (unsigned char*) PRNG_state, 256/8 );
	PRNG_state[0]++;			// !!!!!!!!!!!!!!!!! PLACEHOLDER !!!!!!!!!!!!
	for (i = 0; i<remaining_full_bytes_to_decrypt; i++)
		msg[(32*full_blocks_to_decrypt)+i] ^= key[i];

	return return_state;
}

uint8_t uEliece_verify( uint8_t* msg, uEl_msglen_t ctext_len, uEl_msglen_t* len) {

	uint8_t return_state = 0; 			// Return value, 0 correct, flags for errors
	uEl_msglen_t ctext_len_bytes = ctext_len/8;
	uEl_msglen_t i;

	/* 
	 * Find integrity check constant
	 */
	uint32_t ICK_last_byte_pos = -1;
	for (i = ((UEL_PARITY_START - 1 - 32))-1;i>=31;i--)
	{
		if (msg[i]!=0) {
			ICK_last_byte_pos = i;
			break;
		}
		if (i == 31)
			return_state |= UEL_BAD_INTEGRITY;
	}
	
	/*
	 * Verify message integrity
	 */
	for ( i = 0; i<32; i++ )
		if (uEL_ICK[i] != msg[ICK_last_byte_pos-31+i]) {
			return_state |= UEL_BAD_INTEGRITY;
		}

	/* 
	 * Find decrypted message length
	 */
	*len = (ICK_last_byte_pos-33);
	if (msg[(*len)-1]==0x80)
		(*len)--;

	return return_state;
			
}

/* Encryption methods */

uint8_t uEliece_encryption_prepare( uint8_t** msg, uEl_msglen_t len, uEl_msglen_t* result_len) {
	
	uint8_t return_state = 0; 			// Return value, 0 correct, flags for errors

	uEl_msglen_t i;					// iterators
	uEl_msglen_t ctext_len;
	uEl_msglen_t len_full_bytes;
	len_full_bytes = len / 8;

	/* 
	 * Calculate resulting ciphertext length
	 */

	if (len<(UEL_MDPC_M - 2*256 - 1))
		ctext_len = 2* UEL_M_PADDED;
	else
		ctext_len = len + (8 - (len%8)) +  2*256 + 8 + UEL_M_PADDED;

	const uEl_msglen_t ctext_len_bytes = ctext_len / 8;
	*result_len = ctext_len_bytes;

	/* 
	 * Allocate additional memory for ciphertext
	 */
	MSG = realloc(MSG,ctext_len_bytes*sizeof(uint8_t));
	if (MSG == NULL)
		return UEL_MALLOC_FAIL;		// Error: Could not allocate memory
	/* 
	 * Add padding byte
	 */
	if  (len%8 != 0 ) {
		MSG[len_full_bytes] |= 1 << len%8;
		for (i=(len%8)+1;i<8;i++)
			MSG[len_full_bytes] &= ~(1 << i);
	} else {
		MSG[len_full_bytes] = 0x80;
	}
	len_full_bytes += 1;

	/* 
	 * Append integrity check constant
	 */
	for (i=0; i<32; i++) 
		MSG[len_full_bytes+i]=uEL_ICK[i];
	
	/* 
	 * Set the rest of the message to 0
	 */
	for (i=len_full_bytes+32; i<=ctext_len_bytes; i++)
		MSG[i] = 0;

	return return_state;

}

uint8_t uEliece_wrap( uint8_t* msg, uEl_msglen_t len, uEl_msglen_t* result_len, uEl_rng* rng) {
	
	uint8_t return_state = 0; 			// Return value, 0 correct, flags for errors

	uint32_t i;					// iterators
	const uEl_msglen_t ctext_len_bytes = *result_len;

	/* 
	 * Generate session key
	 */
	rng->initRandom();
	uEl_256bit sskey;
	rng->getRandom(sskey, 32);
	rng->closeRandom();

	/* 
	 * Init PRNG with session key
	 */
	uEl_256bit PRNG_state;
	for (i=0;i<32;i++)
		PRNG_state[i] = sskey[i] ;
	
	/* 
	 * Encrypt message by 256-bit blocks
	 */
	uEl_256bit key;
	uint32_t j;
	const uint32_t full_blocks_to_encrypt = ((ctext_len_bytes*8) - UEL_M_PADDED - 256 - 8)/256;

	for (i=0;i<full_blocks_to_encrypt;i++)
	{
		crypto_hash( (unsigned char*) key, (unsigned char*) PRNG_state, 256/8 );
		PRNG_state[0]++;			// !!!!!!!!!!!!!!!!! PLACEHOLDER !!!!!!!!!!!!
		for (j=0;j<32;j++)
			msg[(32*i)+j] ^= key[j] ;
	}
	
	uint8_t remaining_bits_to_encrypt = ((ctext_len_bytes*8) - UEL_M_PADDED - 256 - 8)%256;
	const uint8_t remaining_full_bytes_to_encrypt = remaining_bits_to_encrypt/8;

	crypto_hash( (unsigned char*) key, (unsigned char*) PRNG_state, 256/8 );
	PRNG_state[0]++;			// !!!!!!!!!!!!!!!!! PLACEHOLDER !!!!!!!!!!!!
	for (i = 0; i<remaining_full_bytes_to_encrypt; i++)
		msg[(32*full_blocks_to_encrypt)+i] ^= key[i];

	/* 
	 * Encrypt session key
	 */ 
	crypto_hash( (unsigned char*) key, (unsigned char*) msg, 32*full_blocks_to_encrypt+remaining_full_bytes_to_encrypt );
	for (i=0;i<32;i++)
		sskey[i] ^= key[i] ;
	
	/* 
	 * Append encrypted session key
	 */
	for (i=0;i<32;i++)
		msg[i+UEL_SSKEY_START] = sskey[i];

	return return_state;
}

uint8_t uEliece_encode( uint8_t* msg, uEl_PubKey pubkey ) {
	
	uint8_t return_state = 0; 			// Return value, 0 correct, flags for errors

	uint32_t i, j;					// iterators

	/* 
	 * Encode - multiply by generator matrix
	 */	
	if (msg[0]&((uint8_t) 1))
		for(j=0;j<((UEL_M_PADDED)/8);j++) {
			msg[(UEL_M_PADDED/8)+j] ^= pubkey[j];
		}
	
	for (i=1;i<UEL_MDPC_M;i++) {

		uint8_t buff = pubkey[(UEL_M_PADDED/8)-1];	// Rotation of public key
		uint8_t buff2;
		uint16_t k;

		for(k=0;k<((UEL_M_PADDED)/8)-1;k++) {		
			buff |= pubkey[k]<<1;
			buff2 = pubkey[k]>>7;
			pubkey[k] = buff;
			buff = buff2;
		}	
		pubkey[((UEL_M_PADDED)/8)-1] = buff;

		if (msg[(i/8)]&(1<<(i%8)))
			for(j=0;j<((UEL_M_PADDED)/8);j++) {
				msg[(UEL_M_PADDED/8)+j] ^= pubkey[j];
			}
	}
	
	uint8_t buff = pubkey[(UEL_M_PADDED/8)-1];
	uint8_t buff2;
	uint16_t k;

	for(k=0;k<((UEL_MDPC_M+7)/8)-1;k++) {		// Rotation of public key back to original
		buff |= pubkey[k]<<1;
		buff2 = pubkey[k]>>7;
		pubkey[k] = buff;
		buff = buff2;
	}	
	pubkey[((UEL_MDPC_M+7)/8)-1] = buff;


	return return_state;
}

uint8_t uEliece_add_errors( uint8_t* msg, uEl_rng* rng ) {
	
	uint8_t return_state = 0; 			// Return value, 0 correct, flags for errors
	uint32_t i, j;					// iterators

	/* 
	 * Generate error vector
	 */
	uint16_t errv[UEL_MDPC_T];
	uint8_t used;
	rng->initRandom();
	for (i=0;i<UEL_MDPC_T;i++) {
		do {
			do { 
				rng->getRandom(errv+i, 2);	
        			errv[i] /= (0xFFFF/(UEL_MDPC_M));
    			} while (errv[i] > (UEL_MDPC_M-1));
			used = 0;
			for(j=0;j<(i-1);j++) {
				if ((errv[i]==errv[j])) {
					used = j;
					break;
				}
			}
		} while (used!=0);
	}
	rng->closeRandom();
	/* 
	 * Apply error vector
	 */
	for (i=0;i<UEL_MDPC_T;i++)
		msg[(errv[i]/8)] ^= 1<<(errv[i]%8);

	return return_state;
}


/* Default random number generators */

#ifdef BUILDTYPE_LINUX

#include <fcntl.h>
#include <unistd.h>

uint8_t uEl_linux_default_rng_file;

uint8_t uEl_linux_default_rng_init() {
	uEl_linux_default_rng_file = open("/dev/urandom", O_RDONLY );
	return 0;
}

uint8_t uEl_linux_default_rng_close() {
	close(uEl_linux_default_rng_file);
	return 0;
}

uint8_t uEl_linux_default_rng_get(void* buffer, uint8_t bytes) {
	read(uEl_linux_default_rng_file, buffer, bytes);
	return 0;
}

const uEl_rng uEl_default_rng() {
	uEl_rng defaultRng = {	.initRandom = &uEl_linux_default_rng_init,
				.closeRandom = &uEl_linux_default_rng_close,
				.getRandom = &uEl_linux_default_rng_get
			};
	return defaultRng;
}

#endif // BUILDTYPE_LINUX
