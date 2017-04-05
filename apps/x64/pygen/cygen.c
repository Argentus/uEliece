#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define X 9857
#define w0 71
#define bytes (X / 32) + 1

#define SetBit(P,i)     ( (P)[( (i) / 32)] |=  (1 << ( (i) % 32)) )
#define ClearBit(P,i)   ( (P)[( (i) / 32)] &= ~(1 << ( (i) % 32)) )
#define TestBit(P,i)  !!( (P)[( (i) / 32)] &   (1 << ( (i) % 32)) )


void printArray(int arr[], int size) {
    int i;

    for(i = 0; i < size; i++) {
        printf("%u ", arr[i]) ;
    }
    putc('\n', stdout);
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

void createPolynomial(int indexes[], int pol[]) {
    int i;

    for(i = 0; i < w0; i++) {
        SetBit(pol, indexes[i]); 
    }
}

int main() {
    int indexes[w0], i;
    unsigned int pol[(bytes] = {0};
 
    srand(time(NULL));

    generateIndexes(indexes);
    printArray(indexes, w0);
    createPolynomial(indexes, pol);
    putc('\n', stdout);
    printArray(pol, bytes;
    
    return 0;
}

