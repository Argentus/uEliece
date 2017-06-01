#ifndef KARATSUBA_H_
#define KARATSUBA_H_

#define INT_SIZE 32
#define BYTES(i) (i / INT_SIZE) + 1
#include <stdint.h>

uint32_t printPolyBlock(uint32_t block, uint32_t index, uint32_t printPlus );
void printPoly(uint32_t* poly, int size);
uint32_t* shiftBlocksLeft(uint32_t* poly, uint32_t blockCount);

uint32_t* getDPart(uint32_t* polyA, uint32_t* polyB, uint32_t* polyRes, uint32_t* polyD, uint32_t blockCount, uint32_t degD);

uint32_t* KaratsubaMultiply(uint32_t* polyA, uint32_t* polyB, uint32_t blockCount, uint32_t degC, uint32_t degD );

uint32_t checkDegree( uint32_t* poly, uint32_t blockSize);

uint32_t* Karatsuba(uint32_t* polyA, uint32_t blockCountA, uint32_t* polyB, uint32_t blockCountB );

#endif
