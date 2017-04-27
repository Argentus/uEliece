#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define X 9857
#define w0 71
#define bytes (X / 32) + 1

#define SetBit(P,i)     ( (P)[( (i) / 32)] |=  (1 << ( (i) % 32)) )
#define ClearBit(P,i)   ( (P)[( (i) / 32)] &= ~(1 << ( (i) % 32)) )
#define TestBit(P,i)  !!( (P)[( (i) / 32)] &   (1 << ( (i) % 32)) )

int count = 0;
int secondaryCount = 0;

void printArray(int arr[], int size) {
    int i;

    for(i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    putc('\n', stdout);
}

void printBits(int pol[]) {
    int i;

    for(int i = 0; i < bytes * 32; i++) {
        printf("%d", TestBit(pol,i));
        if(i % 31 == 0 && i != 0)
            printf(" ");
    }
    printf("\n");
}

void printReadableBits(int arr[], int size) {
	int i;

    for(i = 0; i < size; i++) {
        if(TestBit(arr, i))
			printf("x%d ", i);
    }
}

void generateIndexes(int indexes[]) {
    int i, num;
    unsigned int mask[bytes] = {0};

    for(i = 0; i < w0; i++) {
        do {
            num = rand() % X;
        } while(TestBit(mask, num));
        SetBit(mask, num);
        indexes[i] = num;
    }
}

void createPolynomial(int indexes[], int arr[]) {
    int i;

    for(i = 0; i < w0; i++) {
        SetBit(arr, indexes[i]);
    }
}

void freeMemory(int **euclid, int size) {
	int i;

    for (i = 0; i < size; i++)
        free(euclid[i]);
    free(euclid);
}

void allocMemory(int ***euclid) {
	//TODO those two functions can be merged into one
    static int max = 0;

    if (count == max) {
        int newmax = (max + 2) * 2;   /* 4, 12, 28, 60, ... */
        int **newptr = (int **)realloc(*euclid, newmax * sizeof(*euclid));

        if (newptr == NULL) {
            freeMemory(*euclid, count);
            exit(EXIT_FAILURE);
        }
        max = newmax;
        *euclid = newptr;
    }

    (*euclid)[count] = (int *)calloc(bytes, sizeof(int));

    if ((*euclid)[count] == 0) {
        freeMemory(*euclid, count);
        exit(EXIT_FAILURE);
    }

    count++;
}

void allocSecondaryMemory(int ***euclid) {
    static int max = 0;

    if (count == max) {
        int newmax = (max + 2) * 2;   /* 4, 12, 28, 60, ... */
        int **newptr = (int **)realloc(*euclid, newmax * sizeof(*euclid));

        if (newptr == NULL) {
            freeMemory(*euclid, count);
            exit(EXIT_FAILURE);
        }
        max = newmax;
        *euclid = newptr;
    }

    (*euclid)[count] = (int *)calloc(bytes, sizeof(int));

    if ((*euclid)[count] == 0) {
        freeMemory(*euclid, count);
        exit(EXIT_FAILURE);
    }

    count++;
}

int getDegree(int euclid[]) {
    int i, j;

    for(i = bytes - 1; i >= 0; i--) {
        if(euclid[i]) {
            for(j = 32 -1; j >= 0; j--) {
                if(TestBit(&(euclid[i]),j)) {
                    return j + 32*(i) + 1;
                }
            }
        }
    }

    return 0;
}

void addingPolynomials(int augend[], int addend[], int total[]) {
    int i;

    for(i = 0; i < bytes; i++) {
        total[i] = augend[i] ^ addend[i];
    }
}

void shiftPolynomial(int originPoly[], int shiftedPoly[], int shift) {
    //test 16bit integer, then 8bit etc to speed it up
    int i, j;

    for(i = 0; i < bytes; i++) {
        if(originPoly[i]) {
            for(j = 0; j < 32; j++) {
                if(TestBit(originPoly + i,j))
                    SetBit(shiftedPoly, j + shift);
            }
        }
    }
}

void multiplePolynomials(int multiplier[], int multiplicant[], int product[]) {
  //TODO Russian Peasant Multiplication algorithm
  //TODO the same as in dividePolynomials, shift no needed, just add with offset
  int i, j, tempMultiplier[bytes] = {0};
    for(i = 0; i < bytes; i++) {
        if(multiplicant[i]) {
            for(j = 0; j < 32; j++) {
                if(TestBit(multiplicant + i,j)) {
                  shiftPolynomial(multiplier, tempMultiplier, (i+1)*j);
                  addingPolynomials(product, tempMultiplier, product);
                  memset(tempMultiplier, 0, sizeof(tempMultiplier));
                }
            }
        }
    }
}

void dividePolynomials(int divident[], int divisor[], int quotient[], int remainder[]) {
	//TODO we do not need to shift there, just add polynomials with offset
    //N, divident/tempDivident - divident will change, we want to keep original divident
    //D, divisor/tempDivisor - divisor is going to be shifted, we want to keep original divisor
    //q, quotient - quotient
    //r, remainder - remainder
    int degreeOfDivident, degreeOfDivisor, tempDivisor[bytes] = {0}, tempDivident[bytes] = {0}, dividentHolder[bytes] = {0};

    memcpy(tempDivident, divident, sizeof(tempDivident));

    while(1) {
        degreeOfDivident = getDegree(tempDivident); //get degree of divident/divisor
        degreeOfDivisor = getDegree(divisor);

        if(degreeOfDivident < degreeOfDivisor) {  //check wheter division is applicable
            memcpy(remainder, tempDivident, sizeof(tempDivident));

            return;
        }

        shiftPolynomial(divisor, tempDivisor, degreeOfDivident - degreeOfDivisor); //shift divisor to divident degree
        SetBit(quotient, degreeOfDivident - degreeOfDivisor); //set the difference between divisor and divident degree to quotient
        memcpy(dividentHolder, tempDivident, sizeof(tempDivident)); //tempDivident stores result of adding polynomials, so safe current tempDivident
        memset(tempDivident, 0, sizeof(tempDivident)); //clear tempDivident
        addingPolynomials(dividentHolder, tempDivisor, tempDivident); //add divident and divisor (substraction = addition in GF2)
        memset(tempDivisor, 0, sizeof(tempDivisor)); //clear tempDivisor
        memset(dividentHolder, 0, sizeof(dividentHolder)); //clear dividentHolder
    }
}

int checkZeroPolynomial(int pol[]) {
    int i;

    for(i = 0; i < bytes; i++) {
    	if(pol[i])
    		return 0;
    }

    return 1;
}

void euclidAlgorithm(int ***euclid) {
    int indexes[w0], i;

    srand(time(NULL));

    // generateIndexes(indexes);
    // printArray(indexes, w0);
    // allocMemory(euclid);
    // createPolynomial(indexes, (*euclid)[0]);
    // putc('\n', stdout);
    // printArray((*euclid)[0], bytes);
    // putc('\n', stdout);
    // allocMemory(euclid);
    // SetBit((*euclid)[1], X);
    // SetBit((*euclid)[1], 0);
    // printArray((*euclid)[1], bytes);

    // allocMemory(euclid);
    // (*euclid)[2][0] = 3;

    // printf("TOTO : %d \n", getDegree((*euclid)[2]));
//____________________________________________________________

    // allocMemory(euclid);
    // SetBit((*euclid)[0],0);
    // SetBit((*euclid)[0],1);
    // //SetBit((*euclid)[0],3);
    // //SetBit((*euclid)[0],1);
    // //SetBit((*euclid)[0],0);
    // allocMemory(euclid);
    // //SetBit((*euclid)[1],6);
    // //SetBit((*euclid)[1],4);
    // //SetBit((*euclid)[1],1);
    // SetBit((*euclid)[1],0);
    // allocMemory(euclid);
    // allocMemory(euclid);
    // dividePolynomials((*euclid)[0], (*euclid)[1], (*euclid)[2], (*euclid)[3]);

    // printf("[");
    // printReadableBits((*euclid)[0], 10);
    // printf("%s", "] : [");
    // printReadableBits((*euclid)[1], 10);
    // printf("]");

    // printf("%s", " = [");
    // printReadableBits((*euclid)[2], 10);
    // printf("%s ", "] zv. [");
    // printReadableBits((*euclid)[3], 10);
    // printf("]\n");

    // printf("smejo %d\n", checkZeroPolynomial((*euclid)[3]));

    //toto bude na vstupe
    allocMemory(euclid);
    SetBit((*euclid)[0],8);
    SetBit((*euclid)[0],4);
	SetBit((*euclid)[0],3);
	SetBit((*euclid)[0],1);
	SetBit((*euclid)[0],0);
    allocMemory(euclid);
    SetBit((*euclid)[1],6);
    SetBit((*euclid)[1],4);
    SetBit((*euclid)[1],1);
    SetBit((*euclid)[1],0);
	allocMemory(euclid);
	allocMemory(euclid);
	dividePolynomials((*euclid)[0], (*euclid)[1], (*euclid)[2], (*euclid)[3]);

    printf("[");
    printReadableBits((*euclid)[0], 10);
    printf("%s", "] : [");
    printReadableBits((*euclid)[1], 10);
    printf("]");

    printf("%s", " = [");
    printReadableBits((*euclid)[2], 10);
    printf("%s ", "] zv. [");
    printReadableBits((*euclid)[3], 10);
    printf("]\n");

	i = 1;

    while(!checkZeroPolynomial((*euclid)[i+2])) {
	 	allocMemory(euclid);
		allocMemory(euclid);
		dividePolynomials((*euclid)[i+0], (*euclid)[i+2], (*euclid)[i+3], (*euclid)[i+4]);

	    printf("[");
	    printReadableBits((*euclid)[i+0], 10);
	    printf("%s", "] : [");
	    printReadableBits((*euclid)[i+2], 10);
	    printf("]");

	    printf("%s", " = [");
	    printReadableBits((*euclid)[i+3], 10);
	    printf("%s ", "] zv. [");
	    printReadableBits((*euclid)[i+4], 10);
	    printf("]\n");

		i+=2; 	

    }

}



int main() {
    int **euclid = 0;
    int **bezout = 0;

    euclidAlgorithm(&euclid);

    freeMemory(euclid, count);
    freeMemory(bezout, secondaryCount);
    
    return 0;
}

