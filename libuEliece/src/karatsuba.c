/*
	TEST - Karatsuba multiplication polynomial in GF(2)

	32bit numbers
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// Macro for INT SIZE
#define INT_SIZE 32
#define BYTES(i) (i / INT_SIZE) + 1
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/*
	Function print one poly block, index is index of the block in array,
	printPlus is help flag, if printing plus is neccessary
*/
uint32_t printPolyBlock(uint32_t block, uint32_t index, uint32_t printPlus ){
	
	uint32_t i, bitNumber = 1, pos = INT_SIZE;
	
//	fprintf(stderr, "Debug: block = %u, index = %u\n", block, index);
		
	if(printPlus > 1){
		fprintf(stderr, "Error: invalid flag - printPlus\n");
		exit(0);
	}

	//first check last valid bit for better printing
	for(i=0; i< INT_SIZE; i++){
		if( block & bitNumber ){
			pos = i;
		}
		bitNumber <<= 1;
	}
	// block does not contain any bit == 1 -> nothing to print
	if(pos == INT_SIZE){
//		fprintf(stderr, "Debug: koniec\n");
		return 1;
	}
	
	//set bitNumber again to 1, for printing the bits == 1
	bitNumber = 1;
	
	if( index > 0 && printPlus == 0){
		fprintf(stdout, " + ");
	}
	for(i=0; i< INT_SIZE; i++){
		if( block & bitNumber ){
			fprintf(stdout, "x^%i", (i + (index*INT_SIZE)));
			if( i != pos ){
				fprintf(stdout, " + ");
			}
		}
		bitNumber <<= 1;
	}
	return 0;
}

/*
	Function prints whole array as polynomial
*/
void printPoly(uint32_t* poly, int size){
	uint32_t i, response = 0;
	for(i = 0; i < size; i++){
		/*if( i > 0 && poly[i-1] ^ 0){
			fprintf(stdout, " + ");
		}*/
		response = printPolyBlock( poly[i], i, response);
	}
	printf("\n");
}

//----------------------------------------------------------------------------
// 				KARATSUBA MULTIPLICATION FUNCTIONS
//----------------------------------------------------------------------------

/*
	Function shifts left whole polynomial once
	poly - array of numbers,
	blockCount - count of blocks of the array
*/
uint32_t* shiftBlocksLeft(uint32_t* poly, uint32_t blockCount){
	uint32_t i, first = 0, last, bitNumber = 1;
	bitNumber <<= ( INT_SIZE-1 );
	
	for(i=0; i < blockCount; i++){
		if( bitNumber & poly[i] )
			last = 1;			
		else
			last = 0;

		poly[i] <<= 1;
		if( i > 0){
			poly[i] ^= first;
		}
		first = last;
	}	
	return poly;
}

//static uint64_t count = 0;

/*
	Function computes Ds,t 
	polyA - first polynomial as array
	polyB - second polynomial as array
	polyRes - result polynomial
	polyD - polynomial D as D(x) = A(x)B(x)
	blockCount - number of blocks for polyA and polyB
	degD - degree of polynomial D
*/
uint32_t* getDPart(uint32_t* polyA, uint32_t* polyB, uint32_t* polyRes, uint32_t* polyD, uint32_t blockCount, uint32_t degD){
	uint32_t i, j, k, shiftJ, shiftK, blockJ, blockK, blockRes, tmp, wA, wB;
	uint32_t shiftRes;
//	fprintf(stdout, "D part\n\n");	
	for(i=1; i < (2*degD); i++ ){
//		fprintf(stdout, "i = %u\n", i);
//		shiftRes = 1 << (i % INT_SIZE);
		blockRes = i / INT_SIZE;  		
		for(j=0; j <= i/2; j++){
			for(k = i; k > j; k--){
//				count++;
				if( k > degD){
					k = degD;
				}
				if( j+k == i && k > j ){
//					fprintf(stdout, " j = %u, k = %u\n", j, k);
					// tu je miesto pre zistenie Di,j
					shiftJ = 1 << (j % INT_SIZE);
					shiftK = 1 << (k % INT_SIZE);
					blockJ = j / INT_SIZE;
					blockK = k / INT_SIZE;
					
					wA = (polyA[blockJ] & shiftJ) == 0 ? 0 : 1; 
					wB = (polyA[blockK] & shiftK) == 0 ? 0 : 1; 
					tmp = wA ^ wB;  		
				
					wA = (polyB[blockJ] & shiftJ) == 0 ? 0 : 1; 
					wB = (polyB[blockK] & shiftK) == 0 ? 0 : 1; 
					tmp &= wA ^ wB;  		
///					fprintf(stdout, "D %u, %u = %u\n", j, k, tmp);
					shiftRes = tmp << (i % INT_SIZE);
//					fprintf(stdout, "D %u, %u shiftRes =  %u\n", j, k, shiftRes);
					polyRes[blockRes] ^= shiftRes;
				}
				else{
					continue;
				}
			}
		}
	}

	return polyRes; 
}

/*
	Function computes Karatsuba multiplication from polA and polyB and returns its product
polyA - first polynomial
polyB - second polynomial
blockCount - blockCount for polyA and polyB
degC - degree of result polynomial C
degD - degree of polynomial D
*/
uint32_t* KaratsubaMultiply(uint32_t* polyA, uint32_t* polyB, uint32_t blockCount, uint32_t degC, uint32_t degD ){
	uint32_t i, j;
	//fprintf(stderr, "Debug: bC = %u, degC = %u, degD = %u\n", blockCount, degC, degD);
	if( blockCount != BYTES(degD) ){
		fprintf(stderr, "Error: block count of polynomial D differs from block count of A or B\n");
		exit(0);
	}


	uint32_t* polyC = (uint32_t*) calloc( BYTES(degC), sizeof(uint32_t));
	uint32_t* work = (uint32_t*) calloc( BYTES(degC), sizeof(uint32_t));
	uint32_t* polyD = (uint32_t*) calloc( BYTES(degD), sizeof(uint32_t));

	//	get Di = Ai & Bi, for i := 0 : n-1, where n = deg(A) = deg(B)
	// and set those block to polyC as first step
	for(i=0; i< blockCount; i++){
		polyD[i] = polyA[i] & polyB[i];
		polyC[i] = polyD[i];
		work[i] = polyD[i];
	}
	
	//printPoly(polyD, BYTES(degD));
	
	// next we need to shift this polyD n times and XOR it with polyC
	for(i=0; i< degD; i++){
//		fprintf(stdout, "Debug: shiftBlock iter nr. = %u\n", i+1);
		work = shiftBlocksLeft(work, BYTES(degC) );
		for(j=0; j < BYTES(degC); j++){
			polyC[j] ^= work[j];
		}
	}

	// then we need to calculate all combinations Di,j where j > i >= 0 and i + j = k
	//	for k = 1 : 2n - 3
//	printPoly(polyC, BYTES(degC));	
 	polyD = getDPart( polyA, polyB, polyC, polyD, blockCount, degD);
//	fprintf(stdout, "Iteration count = %lu\n", count);
//	printPoly(polyD, blockCount);
	if( polyC == NULL){
		return NULL;
	}
	
	return polyD; 
}

/*
	Function checks degree of given polynomial
*/
uint32_t checkDegree( uint32_t* poly, uint32_t blockSize){
	uint32_t i, j, bitNumber, pos = INT_SIZE;
	for(i = blockSize-1; i >= 0; i--){
		bitNumber = 1;
		for(j=0; j < INT_SIZE; j++){
			if( bitNumber & poly[i]){
				pos = j;
			}
			bitNumber <<= 1;
		}
		if( pos != INT_SIZE || i == 0 ){
			break; 
		}
	}
	if( pos == INT_SIZE )
		return 0;
	else
		return i * INT_SIZE + pos; 
}

/*
	Function computes Karatsuba multiplication for two polynomial.
	polyA - array of integers, where each BIT represents one coefficient of polynomial
	blockCountA - count of blocks of polynomial A	

	polyB - array of integers, where each BIT represents one coefficient of polynomial
	blockCountB - count of blocks of polynomial B	
*/
uint32_t* Karatsuba(uint32_t* polyA, uint32_t blockCountA, uint32_t* polyB, uint32_t blockCountB ){
	uint32_t newSize, degA, degB, degC, degD;
	uint32_t *newPoly, *oldPoly;
	
	/*
		First, we need to know degree of the bigger polynomial of A or B and
		degree of the result polynomial C
	*/
	degA = checkDegree( polyA, blockCountA );
	degB = checkDegree( polyB, blockCountB );
	degD = degA >= degB ? degA : degB;
	degC = degA + degB;

//	fprintf(stderr, "Debug: degA = %u, degB = %u, degC = %u, degD = %u\n", degA, degB, degC, degD);
	/*
		If both polynomials have the same block count, we can multiply those polynomials
	*/	
	if( blockCountA == blockCountB ){
		return KaratsubaMultiply( polyA, polyB, blockCountA, degC, degD );
	}
	else{
		newSize = blockCountA > blockCountB ? blockCountA : blockCountB;
//		fprintf(stderr, "Debug: new size is %u\n", newSize);
		// if A is greater -> realloc B
		if( newSize == blockCountA ){
					
			newPoly = (uint32_t*) malloc( newSize * sizeof(uint32_t) );
			memcpy( newPoly, polyB, newSize);
			oldPoly = polyA;
		}	
		else{
			newPoly = (uint32_t*) malloc( newSize * sizeof(uint32_t) );
			memcpy( newPoly, polyA, newSize);
			oldPoly = polyB;
		}
	}
	if( newPoly == NULL ){
		fprintf(stderr, "Error: Can not realloc polynomial\n");
		free(newPoly);
		exit(0);
	}
	return KaratsubaMultiply( newPoly, oldPoly, newSize, degC, degD );
}

