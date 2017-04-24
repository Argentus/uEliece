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
    /**TODO: This function could be done much faster in assembly
        or after checking 32bit integer, test 16bit integer, then 8bit
        etc to speed it up**/
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

void dividePolynomials(int divident[], int divisor[], int quotient[], int remainder[]) {
    //N, divident/tempDivident - divident will change, we want to keep original divident
    //D, divisor/tempDivisor - divisor is going to be shifted, we want to keep original divisor
    //q, quotient - quotient
    //r, remainder - remainder
    int degreeOfDivident, degreeOfDivisor, tempDivisor[bytes] = {0}, tempDivident[bytes] = {0}, dividentHolder[bytes] = {0};

    memcpy(tempDivident, divident, sizeof(tempDivident));

    while(1) {
        // printf("%s\n", "tempDivident");
        // printBits(tempDivident);

        degreeOfDivident = getDegree(tempDivident);
        degreeOfDivisor = getDegree(divisor);

        //printf("%d %d\n", degreeOfDivident, degreeOfDivisor);

        if(degreeOfDivident < degreeOfDivisor) {
            memcpy(remainder, tempDivident, sizeof(tempDivident));

            return;
        }



        shiftPolynomial(divisor, tempDivisor, degreeOfDivident - degreeOfDivisor);

        printf("%s\n", "tempDivisor");
        printBits(tempDivisor);

        SetBit(quotient, degreeOfDivident - degreeOfDivisor);

        memcpy(dividentHolder, tempDivident, sizeof(tempDivident));
        memset(tempDivident, 0, sizeof(tempDivident));
        addingPolynomials(dividentHolder, tempDivisor, tempDivident);





        // memset(tempDivisor, 0, sizeof(tempDivisor));
        // memset(dividentHolder, 0, sizeof(dividentHolder));

    }

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

    allocMemory(euclid);
    SetBit((*euclid)[0],0);
    SetBit((*euclid)[0],1);
    allocMemory(euclid);
    SetBit((*euclid)[1],0);
    allocMemory(euclid);
    allocMemory(euclid);
    dividePolynomials((*euclid)[0], (*euclid)[1], (*euclid)[2], (*euclid)[3]);

    printf("%s\n", "asd");

    printBits((*euclid)[2]);
    printf("%s\n", "______________");
    printBits((*euclid)[3]);
}



int main() {
    int **euclid = 0;

    euclidAlgorithm(&euclid);

    freeMemory(euclid, count);

    
    return 0;
}

