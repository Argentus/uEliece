/*
 *	File:	"uEliece.h"
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
 * This header file defines the interface for using uEliece cryptosystem.
 *
 * 
 *-----------------------------------------------------
 */

#ifndef UELIECE_H
#define UELIECE_H

#include <stdint.h>

/*
 * Macro definitons:
 */

// Encryption parameters:
#define UEL_MDPC_N 19714 	
#define UEL_MDPC_N0 2
#define UEL_MDPC_M (UEL_MDPC_N/UEL_MDPC_N0)
#define UEL_MDPC_T 134
#define UEL_MDPC_W 142

// Macros for readability:
#define UEL_M_BYTE_PADDING	(8-(UEL_MDPC_M%8))
#define UEL_M_PADDED		(UEL_MDPC_M + UEL_M_BYTE_PADDING)
#define UEL_M_BYTES		(UEL_M_PADDED/8)
#define UEL_PARITY_START	(ctext_len_bytes - (( UEL_M_PADDED)/8))		// First byte of parity bits 
#define UEL_ENCODED_BLOCK_START (ctext_len_bytes - 2*(( UEL_M_PADDED )/8))		// First byte of ciphertext block used for McEliece encryption
#define UEL_ENCODED_BLOCK_END   (ctext_len_bytes - (( UEL_MDPC_M + UEL_M_BYTE_PADDING )/8) - 1)	// 	- Last byte  -||-
#define UEL_SSKEY_START		(ctext_len_bytes - UEL_M_BYTES - 1 - 32)

/* 
 * Type definitions:
 */


typedef uint8_t uEl_PubKey[(UEL_MDPC_M/8)+1];
typedef uint16_t uEl_PrivKey[UEL_MDPC_N0][UEL_MDPC_W/UEL_MDPC_N0];

typedef struct uEl_rng {
	uint8_t (*initRandom)();
	uint8_t (*getRandom)(void*, uint8_t);
	uint8_t (*closeRandom)();
} uEl_rng;

typedef uint8_t uEl_256bit[32];
typedef uint8_t uEl_Mbits[(UEL_MDPC_M/8)+1];
typedef uint16_t uEl_ErrorVector[UEL_MDPC_T];

/* 
 * Buildtype definitions:
 */

#ifdef BUILDTYPE_X64_LINUX
#define BUILDTYPE_LINUX
#define BUILDTYPE_64BIT
#endif //BUILDTYPE_X64_LINUX

#ifdef BUILDTYPE_X64_WINDOWS
#define BUILDTYPE_WINDOWS
#define BUILDTYPE_64BIT
#endif //BUILDTYPE_X64_WINDOWS

#ifdef BUILDTYPE_AVR8
#define BUILDTYPE_8BIT
#define BUILDTYPE_SHORTMSG
#endif //BUILDTYPE_AVR8

#ifdef BUILDTYPE_SHORTMSG
typedef uint16_t uEl_msglen_t;
#else
typedef uint64_t uEl_msglen_t;
#endif

#ifdef BUILDTYPE_LINUX
const uEl_rng uEl_default_rng();
#endif

#ifdef BUILDTYPE_64BIT
#define ARCH_BYTE_WIDTH 8
typedef uint64_t uint_opt_t;
#endif //BUILDTYPE_64BIT

#ifdef BUILDTYPE_32BIT
#define ARCH_BYTE_WIDTH 4
typedef uint32_t uint_opt_t;
#endif //BUILDTYPE_32BIT

#ifdef BUILDTYPE_16BIT
#define ARCH_BYTE_WIDTH 2
typedef uint16_t uint_opt_t;
#endif //BUILDTYPE_16BIT

#ifdef BUILDTYPE_8BIT
#define ARCH_BYTE_WIDTH 1
typedef uint8_t uint_opt_t;
#endif //BUILDTYPE_8BIT

/*
 * Function return flags
 */
#define UEL_BAD_INTEGRITY (1<<0)
#define UEL_MALLOC_FAIL (1<<1)
#define UEL_RNG_FAULT (1<<2)
#define UEL_DECODE_FAIL (1<<3)

/*
 * Quick use functions:
 */

/*
 * 	Function:  uEliece_decrypt 
 * --------------------
 * 	This function decrypts given message using the
 *	uEliece cryptosystem with specified private
 *	key. Resulting plaintext is stored in place
 *	of the original message.
 * _________________________________________________
 * @param1:	uint8_t** ctext 
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
uint8_t uEliece_decrypt( uint8_t** ctext, uEl_msglen_t ctext_len, uEl_msglen_t* len, const uEl_PrivKey privkey);

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
 *		NEEDS TO BE DYNAMICALLY ALLOCATED!
 *		the memory will be reallocated and used to
 *		store the resulting ciphertext.
 *
 * @param2:	uEl_msglen_t len
 *		- length of plaintext in bits
 *	
 * @param3:	uEl_msglen_t* result_len
 *		- pointer to where resulting ciphertext
 *		length is to be written. Can be NULL if
 *		this information is not desired.
 *
 * @param4:	uEl_PubKey pubkey
 *		- public key used for encryption
 *
 * @param5:	const uEl_rng rng
 *		- random number generator, NULL for default
 * ____________________________________________________
 * @returns:	0 if successful
 *
 */
uint8_t uEliece_encrypt( uint8_t** msg, uEl_msglen_t len, uEl_msglen_t* result_len, uEl_PubKey pubkey, const uEl_rng rng );


/*
 * Decoding helper functions
 */
uint8_t uEliece_syndrome( uint8_t* msg, const uEl_PrivKey privkey, uEl_Mbits syndrome );
uint16_t uEliece_count_upc( uint8_t* msg, const uEl_PrivKey privkey, const uEl_Mbits msg_syndrome, uint16_t index );

/*
 * Decoding algorithm functions
 */

/* 
 * 	Function type: uEliece_decode_f
 * ------------------------------- 
 *	Function of this type decodes encoded message,
 *	removing errors in the process.
 * ____________________________________________________
 * @param1:	uint8_t* msg
 *		- pointer to codeword, which will
 *		be overwritten by decoded message.
 *		Codeword length is UEL_M_BYTES*2 
 *
 * @param2:	const uEl_PrivKey privkey
 *		- private key for decoding
 * ____________________________________________________
 * @returns:	0 if successful
 *
 */
typedef  uint8_t (*uEliece_decode_f)(uint8_t*, const uEl_PrivKey);

/* 
 * uEliece_decode_bf1
 * Bit flipping algorithm, variant 1
 * 
 */
uint8_t uEliece_decode_bf1( uint8_t* msg, const uEl_PrivKey privkey);

/*
 * Decryption functions
 */
uint8_t uEliece_unwrap( uint8_t* msg, uEl_msglen_t ctext_len, uEl_msglen_t* len);
uint8_t uEliece_verify( uint8_t* msg, uEl_msglen_t ctext_len, uEl_msglen_t* len);

/*
 * Encryption functions
 */
uint8_t uEliece_encryption_prepare( uint8_t** msg, uEl_msglen_t len, uEl_msglen_t* result_len);
uint8_t uEliece_wrap( uint8_t* msg, uEl_msglen_t len, uEl_msglen_t* result_len, const uEl_rng* rng );
uint8_t uEliece_encode( uint8_t* msg, uEl_PubKey pubkey );
uint8_t uEliece_add_errors( uint8_t* msg, const uEl_rng* rng  );

#endif // UELIECE_H
